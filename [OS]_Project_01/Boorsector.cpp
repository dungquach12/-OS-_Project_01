#include"Bootsector.h"

int ReadSector(LPCWSTR  drive, int numSector, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, numSector * 512, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
        return 1;
    }
    else
    {
        //printf("Success!\n");
        return 0;
    }
}

// Boot sector info

string Bootsector::FAT_type(BYTE bArr[]) {
    this->BS_FilSysType = hex2string(bArr, 0x52, 8); //Offset 52; byte 8
    this->BS_FilSysType += '\0';
    return this->BS_FilSysType;
}

uint16_t Bootsector::so_byte_moi_sector(BYTE bArr[]) {
    memcpy(&this->BPB_BytsPerSec, bArr + 0xB, 2);
    return this->BPB_BytsPerSec;
}

int Bootsector::so_sector_tren_cluster(BYTE bArr[]) {
    this->BPB_SecPerClus = bArr[0xD]; //Offset D, byte 1
    return this->BPB_SecPerClus;
}

uint16_t Bootsector::so_sector_trong_bootsector(BYTE bArr[]) {
    memcpy(&this->BPB_RsvdSecCnt, bArr + 0xE, 2); //Offset E, byte 2
    return this->BPB_RsvdSecCnt;
}

int Bootsector::so_bang_FAT(BYTE bArr[]) {
    this->BPB_NumFATs = bArr[0x10]; //Offset 10, byte 1
    return this->BPB_NumFATs;
}

uint16_t Bootsector::entryRDET(BYTE bArr[]) {
    memcpy(&BPB_RootEntCnt, bArr + 0x11, 2);
    return this->BPB_RootEntCnt;
}

uint32_t Bootsector::kich_thuoc_volume(BYTE bArr[]) {
    memcpy(&this->BPB_TotSec32, bArr + 0x20, 4);
    return this->BPB_TotSec32;
}

uint32_t Bootsector::kick_thuoc_bang_FAT(BYTE bArr[512]) {
    memcpy(&this->BPB_FATSz32, bArr + 0x24, 4); //Offset 24, byte 4
    return this->BPB_FATSz32;
}

uint32_t Bootsector::root_cluster(BYTE bArr[]) {
    memcpy(&this->BPB_RootClus, bArr + 0x2C, 4);
    return this->BPB_RootClus;
}

// Other 
int Bootsector::k_cluster_to_i_sector(int k) {
    int res;
    res = this->BPB_RsvdSecCnt + this->BPB_FATSz32 * this->BPB_NumFATs + this->RootDirSectors + (k - 2) * this->BPB_SecPerClus;
    return res;
}

// Main function

void Bootsector::ReadBS(BYTE b[]) {
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
}

