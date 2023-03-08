#pragma once
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

struct FAT32 {

    // Boot sector info
    string BS_FilSysType;


    uint16_t BPB_BytsPerSec;    // byte/sector, sector size,                    Offset 11
    int BPB_SecPerClus;         // Sector per cluster,                          Offset 13
    uint16_t BPB_RsvdSecCnt;    // Sector in Bootsector, Reserved sector,       Offset 14
    int BPB_NumFATs;            // FAT table numbers,                           Offset 16
    uint16_t BPB_RootEntCnt;    // entry number,                                Offset 17
    uint32_t BPB_TotSec32;      // Volume size                                  Offset 32
    uint32_t BPB_FATSz32;       // FAT table size (sector/FAT)                  Offset 36
    uint32_t BPB_RootClus;      // starting cluster of RDET;                    Offset 44




    // Calculated info from boot sector info
    int RootDirSectors; // RDET size, count of sectors occupied by the root directory
    int FirstDataSector; // The first sector of data region


    // Other
    uint32_t total_sector;
    int byCluster = 0; // byte/cluster, cluster size
public:
    void ReadBS(BYTE b[]);

    string FAT_type(BYTE bArr[]);                       
    uint16_t so_byte_moi_sector(BYTE bArr[]);
    int so_sector_tren_cluster(BYTE bArr[]);
    uint16_t so_sector_trong_bootsector(BYTE bArr[]);
    int so_bang_FAT(BYTE bArr[]);
    uint16_t entryRDET(BYTE bArr[]);
    uint32_t kich_thuoc_volume(BYTE bArr[]);
    uint32_t kick_thuoc_bang_FAT(BYTE bArr[]);


    uint32_t root_cluster(BYTE bArr[]);
 
    int RDET_size();



    int k_cluster_to_i_sector(int k);
    uint32_t TotalSector(BYTE bArr[]);
    int cluster_size();
};