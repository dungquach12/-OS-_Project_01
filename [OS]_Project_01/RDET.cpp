#include"REDT.h"

string FileS::fileType(uint8_t ATTRIBUTE) {
	string Attr;
	bool checkSet[8] = { false };
	// check if bit is set or not
	for (int i = 0; i < 8; i++) {
		if (ATTRIBUTE & (1 << i) && i == 0)
			Attr += "READ_ONLY/";
		if (ATTRIBUTE & (1 << i) && i == 1)
			Attr += "HIDDEN/";
		if (ATTRIBUTE & (1 << i) && i == 2)
			Attr += "SYSTEM/";
		if (ATTRIBUTE & (1 << i) && i == 3)
			Attr += "VOLUME_ID/";
		if (ATTRIBUTE & (1 << i) && i == 4)
			Attr += "DIRECTORY/";
		if (ATTRIBUTE & (1 << i) && i == 5)
			Attr += "ARCHIVE/";
	}
	Attr += '\0';
	return Attr;
}

void FileS::READ_FAT_TABLE(FileS* theFile, Bootsector boot, BYTE FAT_TABLE[]) {
	uint32_t S_SECTOR = boot.k_cluster_to_i_sector(theFile->DIR_FstClus);
	uint32_t E_SECTOR = S_SECTOR;
	uint32_t FATOffset = theFile->DIR_FstClus * 4;
	uint32_t FATEntOffset = FATOffset / boot.BPB_BytsPerSec;
	uint32_t N_SECTOR;
	memcpy(&N_SECTOR, FAT_TABLE + FATEntOffset, 4);
	bool IsEOF = FALSE;
	if(N_SECTOR >= 0x0FFFFFF8)
		IsEOF = TRUE;
	while (!IsEOF) {
		FATEntOffset += 4;
		memcpy(&N_SECTOR, FAT_TABLE + FATEntOffset, 4);
		E_SECTOR += boot.BPB_SecPerClus;
		if (N_SECTOR >= 0x0FFFFFF8)
			IsEOF = TRUE;
	}
	if(S_SECTOR != E_SECTOR)
		E_SECTOR--;
	theFile->START_SECTOR = S_SECTOR;
	theFile->END_SECTOR = E_SECTOR;
}

void FileS::READ_TXT_FILE(FileS* theFile) {
	string content = "";
	bool EnOfFile = false;
	for (uint32_t i = theFile->START_SECTOR; i <= theFile->END_SECTOR && EnOfFile == false; i++) {
		BYTE sector[512];
		ReadSector(L"\\\\.\\E:", i, sector);
		for (int j = 0; j < 512; j++) {
			content += hex2string(sector, j, 1);
			if (hex2Int(sector, j, 1) == 0) {
				EnOfFile = true;
				break;
			}
		}
	}
	theFile->Content = content + '\0';
}

bool findTxt(string str) {
	bool haveIT = false;
	for (int i = 0; i < str.length() - 3; i++) {
		if (str[i] == 't' && str[i + 1] == 'x' && str[i + 2] == 't')
			haveIT = true;
	}
	return haveIT;
}

void FileS::readRDET(Bootsector boot, BYTE FAT_TABLE_1[]) {
	BYTE RDET_TABLE[512];

	ReadSector(L"\\\\.\\E:", boot.FirstDataSector, RDET_TABLE);


	FileS files[BUFFER];	// Chua tat ca cac file trong RDET
	int fileCount = 0;
	int byteCount = 0;		// Dem so byte da doc
	while (byteCount < 512) {
		int entry_status;
		entry_status = hex2Int(RDET_TABLE, 0 + byteCount, 1);
		if (entry_status == DELETED_FILE) {
			byteCount += 32;		// 32 is size of an entry
			continue;
		}
		fileCount++;

		files[fileCount - 1].Attr = hex2Int(RDET_TABLE, 11 + byteCount, 1);

		if (files[fileCount - 1].Attr == SUB_ENTRY) {
			int countSubEntry = 1;
			while (files[fileCount - 1].Attr == SUB_ENTRY) {  // Stop when get file type/ Attribute of main entry

				memcpy(&files[fileCount - 1].Attr, RDET_TABLE + 0xB + byteCount + 32 * countSubEntry, 1);
				countSubEntry += 1;
			}
			// Get file name
			SubEntry* Stmp = new SubEntry[countSubEntry];
			int count = countSubEntry;
			count -= 2;
			for (int i = 0; i < countSubEntry - 1; i++) {
				Stmp[i].SUB_NAME_5 = hex2string(RDET_TABLE, 0x1 + byteCount + 32 * count, 10);
				Stmp[i].SUB_NAME_6 = hex2string(RDET_TABLE, 0xE + byteCount + 32 * count, 12);
				Stmp[i].SUB_NAME_2 = hex2string(RDET_TABLE, 0x1C + byteCount + 32 * count, 4);

				files[fileCount - 1].NAME += Stmp[i].SUB_NAME_5 + Stmp[i].SUB_NAME_6 + Stmp[i].SUB_NAME_2;  // Set file name and extension
				count--;
			}
			removeSpaces(files[fileCount - 1].NAME);
			files[fileCount - 1].NAME += '\0';
			delete[] Stmp;

			MainEntry Mtmp;
			// Get start cluster
			
			memcpy(&files[fileCount - 1].DIR_FstClusHI, RDET_TABLE + (0x14 + byteCount + 32 * (countSubEntry - 1)), 2);
			memcpy(&files[fileCount - 1].DIR_FstClusLO, RDET_TABLE + (0x1A + byteCount + 32 * (countSubEntry - 1)), 2);

			files[fileCount - 1].DIR_FstClus = (files[fileCount - 1].DIR_FstClusHI * 256 * 256) + files[fileCount - 1].DIR_FstClusLO; // Set file start cluster

			//Get file size
			memcpy(&files[fileCount - 1].DIR_FileSize, RDET_TABLE + (0x1C + byteCount + 32 * (countSubEntry - 1) ), 4); // Set file size


			// Get start sector
			READ_FAT_TABLE(&files[fileCount - 1], boot, FAT_TABLE_1);

			// Read txt file
			bool haveIT = false;
			// fine txt extension
			for (int i = 0; i < files[fileCount - 1].NAME.length(); i++) {
				if (files[fileCount - 1].NAME[i] == 't' && files[fileCount - 1].NAME[i + 2] == 'x' && files[fileCount - 1].NAME[i + 4] == 't')
					haveIT = true;
			}
			cout << endl;
			if (haveIT) {
				READ_TXT_FILE(&files[fileCount - 1]);
			}
			byteCount += 32 * countSubEntry;
		}
		else {

			MainEntry Mtmp;
			// Get file name
			Mtmp.SHORT_NAME = hex2string(RDET_TABLE + (0x0 + byteCount), 0, 8);
			Mtmp.NAME_EXTENSION = hex2string(RDET_TABLE + (0x8 + byteCount), 0, 3);
			Mtmp.NAME_EXTENSION += '\0';
			files[fileCount - 1].NAME = Mtmp.SHORT_NAME + '.' + Mtmp.NAME_EXTENSION + '\0';
			removeSpaces(files[fileCount - 1].NAME);
			// Get file type/ Attribute
			memcpy(&files[fileCount - 1].Attr, RDET_TABLE + 0xB + byteCount, 1);

			// Get file  start cluster
			
			memcpy(&files[fileCount - 1].DIR_FstClusHI, RDET_TABLE + (0x14 + byteCount), 2);
			memcpy(&files[fileCount - 1].DIR_FstClusLO, RDET_TABLE + (0x1A + byteCount), 2);
			files[fileCount - 1].DIR_FstClus = (files[fileCount - 1].DIR_FstClusHI * 256 * 256) + files[fileCount - 1].DIR_FstClusLO;

			// Get file size
			memcpy(&files[fileCount - 1].DIR_FileSize, RDET_TABLE + (0x1C + byteCount), 4);

			// Get sector
			READ_FAT_TABLE(&files[fileCount - 1], boot, FAT_TABLE_1);

			// Read txt file
			removeSpaces(Mtmp.NAME_EXTENSION);
			if (Mtmp.NAME_EXTENSION == "TXT") {
				READ_TXT_FILE(&files[fileCount - 1]);
			}
			byteCount += 32;
		}
	}
	cout << endl;
	for (int i = 0; i < fileCount; i++) {
		if (i == 4) {
			cout << "Continue?(enter)" << endl;
			cin.ignore();
		}
		cout << "Ten File:					" << files[i].NAME << endl;
		cout << "Thuoc tinh:				" << fileType(files[i].Attr) << endl;
		cout << "Cluster bat dau:			" << files[i].DIR_FstClus << endl;
		cout << "Kich thuoc file:			" << files[i].DIR_FileSize << endl;
		cout << "Sector bat dau/ket thuc:		" << files[i].START_SECTOR << "/" << files[i].END_SECTOR << endl;
		cout << "Noi dung txt neu co:			" << '\n' << files[i].Content << endl;
		cout << '\n';
	}
}

