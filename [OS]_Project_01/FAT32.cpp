#include"FAT32.h"

string hex2string(BYTE bArr[], int start, int l) {
    string res;
    for (int i = start; i < start + l; i++)
        res += bArr[i];
    return res;
}

// Boot sector info

string FAT32::FAT_type(BYTE bArr[]) {
    this->BS_FilSysType = hex2string(bArr, 5 * 16 + 2, 8); //Offset 52; byte 8
    return this->BS_FilSysType;
}

uint16_t FAT32::so_byte_moi_sector(BYTE bArr[]) {
    memcpy(&this->BPB_BytsPerSec, bArr + 11, 2);
    return this->BPB_BytsPerSec;
}

int FAT32::so_sector_tren_cluster(BYTE bArr[]) { 
    this->BPB_SecPerClus = bArr[13]; //Offset D, byte 1
    return this->BPB_SecPerClus;
}

uint16_t FAT32::so_sector_trong_bootsector(BYTE bArr[]) {
    memcpy(&this->BPB_RsvdSecCnt, bArr + 14, 2); //Offset E, byte 2
    return this->BPB_RsvdSecCnt;
}

int FAT32::so_bang_FAT(BYTE bArr[]) {
    this->BPB_NumFATs = bArr[16]; //Offset 10, byte 1
    return this->BPB_NumFATs;
}

uint16_t FAT32::entryRDET(BYTE bArr[]) {
    memcpy(&BPB_RootEntCnt, bArr + 16 + 2, 2);
    return this->BPB_RootEntCnt;
}

uint32_t FAT32::kich_thuoc_volume(BYTE bArr[]) {
    memcpy(&this->BPB_TotSec32, bArr + (16 * 2 + 0), 4);
    return this->BPB_TotSec32;
}

uint32_t FAT32::kick_thuoc_bang_FAT(BYTE bArr[512]) {
    memcpy(&this->BPB_FATSz32, bArr + (16 * 2 + 4), 4); //Offset 24, byte 4
    return this->BPB_FATSz32;
}

uint32_t FAT32::root_cluster(BYTE bArr[]) {
    memcpy(&this->BPB_RootClus, bArr + (16 * 2 + 12), 4);
    return this->BPB_RootClus;
}

// Other 
int FAT32::k_cluster_to_i_sector(int k) {
    int res;
    res = this->BPB_RsvdSecCnt + this->BPB_FATSz32 * this->BPB_NumFATs + this->RootDirSectors + (k - 2) * this->BPB_SecPerClus;
    return res;
}

uint32_t FAT32::TotalSector(BYTE bArr[]) {
    memcpy(&this->total_sector, bArr + 16 * 2, 4);
    return this->total_sector;
}

int FAT32::cluster_size() {
    this->byCluster = this->BPB_SecPerClus * this->BPB_BytsPerSec;
    return this->byCluster;
}

// Main function

void FAT32::ReadBS(BYTE b[]) {
    // Boot sector info
    FAT_type(b);
    so_byte_moi_sector(b);
    so_sector_tren_cluster(b);
    so_sector_trong_bootsector(b);
    so_bang_FAT(b);
    entryRDET(b);
    kich_thuoc_volume(b);
    kick_thuoc_bang_FAT(b);
    root_cluster(b);

    // Calculated info from boot sector
    this->RootDirSectors = (this->BPB_RootEntCnt * 32) / this->BPB_BytsPerSec;
    this->FirstDataSector = this->BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz32) + this->RootDirSectors;

    // Other
    cluster_size();
    TotalSector(b);
}

