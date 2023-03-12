#include"REDT.h"

string File::fileType(uint8_t ATTRIBUTE) {
	switch (ATTRIBUTE) {
	case ATTR_READ_ONLY:
		return "READ_ONLY";
		break;
	case ATTR_HIDDEN:
		return "HIDDEN";
		break;
	case ATTR_SYSTEM:
		return "SYSTEM";
		break;
	case ATTR_VOLUME_ID:
		return "VOLUME_ID";
		break;
	case ATTR_DIRECTORY:
		return "DIRECTORY";
		break;
	case ATTR_ARCHIVE:
		return "ARCHIVE";
		break;
	}
	return "Other type/ Attribute!";
}

int hex2Int(BYTE bArr[512], int start, int num) {
	int result = 0;
	for (int i = start + num - 1; i >= start; i--) {
		result = result * 256 + bArr[i];
	}
	return result;
}

void File::readRDET(uint32_t FirstDataSector, uint16_t BPB_RsvdSecCnt) {
	BYTE RDET_TABLE[512];
	BYTE FAT_TABLE_1[512];
	ReadSector(L"\\\\.\\E:", FirstDataSector, RDET_TABLE);
	ReadSector(L"\\\\.\\E:", BPB_RsvdSecCnt, FAT_TABLE_1);

	File files[BUFFER];	// Chua tat ca cac file trong RDET
	int fileCount = 0;
	int byteCount = 0;		// Dem so byte da doc

	cout << "------------" << endl;


	while (byteCount < 512) {
		int entry_type;
		entry_type = hex2Int(RDET_TABLE, 0 + byteCount, 1);
		if (entry_type == DELETED_FILE) {
			byteCount += 32;		// 32 is size of an entry
			continue;
		}
		fileCount++;

		files[fileCount - 1].Attr = hex2Int(RDET_TABLE, 11 + byteCount, 1);

		if (files[fileCount - 1].Attr == SUB_ENTRY) {
			int countSubEntry = 1;
			while (files[fileCount - 1].Attr == SUB_ENTRY) {  // Stop when get file type/ Attribute of main entry
				files[fileCount - 1].Attr = hex2Int(RDET_TABLE, 11 + byteCount + 32 * countSubEntry, 1);
				countSubEntry++;
			}
			// Get file name
			SubEntry* Stmp = new SubEntry[countSubEntry];
			int count = countSubEntry;
			count -= 2;
			for (int i = 0; i < countSubEntry - 1; i++) {
				Stmp[i].SUB_NAME_5 = hex2string(RDET_TABLE, 1 + byteCount + 32 * count, 10);
				Stmp[i].SUB_NAME_6 = hex2string(RDET_TABLE, 14 + byteCount + 32 * count, 12);
				Stmp[i].SUB_NAME_2 = hex2string(RDET_TABLE, 28 + byteCount + 32 * count, 4);

				files[fileCount - 1].NAME += Stmp[i].SUB_NAME_5 + Stmp[i].SUB_NAME_6 + Stmp[i].SUB_NAME_2;  // Set file name and extension
				count--;
			}

			// Get start cluster
			MainEntry Mtmp;
			memcpy(&Mtmp.DIR_FstClusHI, RDET_TABLE + 14 + byteCount + 32 * countSubEntry, 2);
			memcpy(&Mtmp.DIR_FstClusLO, RDET_TABLE + 0x1A + byteCount + 32 * countSubEntry, 2);
			files[fileCount - 1].DIR_FstClus = (Mtmp.DIR_FstClusHI * 256 * 256) + Mtmp.DIR_FstClusLO; // Set file start cluster

			//Get file size
			memcpy(&Mtmp.DIR_FileSize, RDET_TABLE + 28 + 32 * countSubEntry, 4);
			files[fileCount - 1].DIR_FileSize = Mtmp.DIR_FileSize; // Set file size
			delete[] Stmp;

			//READ_FAT_TABLE(files[fileCount - 1], BootSector, FAT_TABLE_1, files[fileCount - 1].DIR_FstClus);

			byteCount += 32 * countSubEntry;
		}
		else {

			MainEntry Mtmp;
			Mtmp.SHORT_NAME = hex2string(RDET_TABLE + 0 + byteCount, 0, 8);
			Mtmp.NAME_EXTENSION = hex2string(RDET_TABLE + 8 + byteCount, 0, 3);
			Mtmp.Attr = hex2Int(RDET_TABLE, 11 + byteCount, 1);
			memcpy(&Mtmp.DIR_FstClusHI, RDET_TABLE + 14 + byteCount, 2);
			memcpy(&Mtmp.DIR_FstClusLO, RDET_TABLE + 0x1A + byteCount, 2);
			memcpy(&Mtmp.DIR_FileSize, RDET_TABLE + 28 + byteCount, 4);

			// Set file
			files[fileCount - 1].NAME = Mtmp.SHORT_NAME + '.' + Mtmp.NAME_EXTENSION;
			files[fileCount - 1].Attr = Mtmp.Attr;
			files[fileCount - 1].DIR_FstClus = (Mtmp.DIR_FstClusHI * 256 * 256) + Mtmp.DIR_FstClusLO;
			files[fileCount - 1].DIR_FileSize = Mtmp.DIR_FileSize;
			byteCount += 32;
		}
	}
	cout << endl;
	for (int i = 0; i < fileCount; i++) {
		cout << "Ten File:						" << files[i].NAME << endl;
		cout << "Thuoc tinh:					" << fileType(files[i].Attr) << endl;
		cout << "Cluster bat dau:				" << files[i].DIR_FstClus << endl;
		cout << "Kich thuoc file:				" << files[i].DIR_FileSize << endl;
		//cout << "Sector bat dau va ket thuc:	" << files[i].START_SECTOR << " -> " << files[i].END_SECTOR << endl;
	}
}

//void File::READ_FAT_TABLE(File& theFile, BYTE BootSector[], BYTE FAT_TABLE[], uint32_t DIR_FstClus) {
//	Bootsector b;
//	b.ReadBS(BootSector);
//	int S_SECTOR = b.k_cluster_to_i_sector(DIR_FstClus);
//	int E_SECTOR = S_SECTOR;
//	int FATOffset = DIR_FstClus * 4;
//	while (true) {
//		int N_SECTOR = hex2Int(FAT_TABLE, FATOffset, 4);
//		FATOffset += 4;
//		END_SECTOR += b.BPB_SecPerClus;
//		if (N_SECTOR == END_FILE)
//			break;
//	}
//
//	theFile.START_SECTOR = S_SECTOR;
//	theFile.END_SECTOR = E_SECTOR;
//}