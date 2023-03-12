#pragma once
#include"Bootsector.h"
#include<stack>

#define BUFFER 50

const int DELETED_FILE = 0xE5;
const uint8_t END_FILE = 0x0FFFFFFF;
const uint8_t SUB_ENTRY = 0x0F;

const uint8_t ATTR_READ_ONLY = 0x01;		// 0000 0001
const uint8_t ATTR_HIDDEN = 0x02;			// 0000 0010
const uint8_t ATTR_SYSTEM = 0x04;			// 0000 0100
const uint8_t ATTR_VOLUME_ID = 0x08;		// 0000 1000
const uint8_t ATTR_DIRECTORY = 0x16;		// 0001 0000
const uint8_t ATTR_ARCHIVE = 0x20;			// 0010 0000



struct File {
	string NAME;
	int Attr;
	uint32_t DIR_FstClus;		
	uint16_t DIR_FileSize;		
	uint32_t START_SECTOR;
	uint32_t END_SECTOR;


public:
	string fileType(uint8_t ATTRIBUTE);
	void readRDET(uint32_t BPB_RootClus, uint16_t BPB_RsvdSecCnt);
	void READ_FAT_TABLE(File& theFile, BYTE BootSector[], BYTE FAT_TABLE[],  uint32_t DIR_FstClus);
	void READ_TXT_FILE(File& theTXTfile);
};

struct MainEntry {
	string SHORT_NAME;			// Ten chinh			Offset 0		8 byte
	string NAME_EXTENSION;		// Ten mo rong			Offset 8		3 byte
	int Attr;					// Thuoc tinh			Offset 11		1 byte
	uint32_t DIR_FstClusHI;		// Phan word cao		Offset 14		2 byte
	uint32_t DIR_FstClusLO;		// Phan word thap		Offset 1A		2 byte
	uint16_t DIR_FileSize;		// Kich thuoc file		Offset 28		4 byte

public:
	void ReadMainEntry(int Offset);
};

struct SubEntry {
	int order;					// Thu tu entry			Offset 0		1 byte
	string SUB_NAME_5;			// Mot phan ten file(5)	Offset 1		10 byte
	int Attr;					// Entry phu = 0F		Offset 11		1 byte
	string SUB_NAME_6;			// Mot phan ten file(6)	Offset 14		12 byte
	string SUB_NAME_2;			// Mot phan ten file(2)	Offset 28		4 byte

public:
	void ReadSubEntry(int Offset);
};