#pragma once
#include"Bootsector.h"

#include<vector>

#define BUFFER 50

const int DELETED_FILE = 0xE5;
const uint8_t SUB_ENTRY = 0x0F;



struct FileS {
	string NAME;
	uint8_t Attr;
	uint16_t DIR_FstClusHI;
	uint16_t DIR_FstClusLO;
	uint32_t DIR_FstClus;		
	uint32_t DIR_FileSize;		
	uint32_t START_SECTOR;
	uint32_t END_SECTOR;
	string Content;
	vector<FileS> ChildFolder;
public:
	FileS() {
		NAME = "";
		Attr = -1;
		DIR_FstClus = -1;
		DIR_FileSize = -1;
		START_SECTOR = -1;
		END_SECTOR = -1;
		Content = "Not txt file!";
	}
	string fileType(uint8_t ATTRIBUTE);
	void readRDET(Bootsector boot, BYTE FAT_TABLE_1[]);
	void READ_FAT_TABLE(FileS* theFile, Bootsector boot, BYTE FAT_TABLE[]);
	void READ_TXT_FILE(FileS* theTXTfile);

	
};

struct MainEntry {
	string SHORT_NAME;			// Ten chinh			Offset 0		8 byte
	string NAME_EXTENSION;		// Ten mo rong			Offset 8		3 byte

public:
	MainEntry() {
		SHORT_NAME = ' ';
		NAME_EXTENSION = ' ';
	}
};

struct SubEntry {
	string SUB_NAME_5;			// Mot phan ten file(5)	Offset 1		10 byte
	uint8_t Attr;					// Entry phu = 0F		Offset 11		1 byte
	string SUB_NAME_6;			// Mot phan ten file(6)	Offset 14		12 byte
	string SUB_NAME_2;			// Mot phan ten file(2)	Offset 28		4 byte

public:
	SubEntry() {
		SUB_NAME_5 = ' ';
		Attr = -1;
		SUB_NAME_6 = ' ';
		SUB_NAME_2 = ' ';
	}
	void ReadSubEntry(int Offset);
};