/*
    This project is reference from fatgen103.docx document,
    take from website: https://download.microsoft.com›fatgen103
*/


#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include"FAT32.h"

using namespace std;

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
    }
    else
    {
        printf("Success!\n");
    }
}

string hexTOstring(BYTE bArr[], int start, int l) {
    string res;
    for (int i = start; i < start + l; i++)
        res += bArr[i];
    return res;
}

int main(int argc, char** argv)
{

    BYTE sector[512];
    ReadSector(L"\\\\.\\E:", 0, sector);

    FAT32 boot;
    boot.ReadBS(sector);

    // Xuat ra man hinh
    cout << "Loai Fat:                              " << boot.BS_FilSysType << endl;
    cout << "-------------------------------------------" << endl;
    cout << "So byte tren moi sector(byte):         " << boot.BPB_BytsPerSec << endl;
    cout << "So sector tren cluster:                " << boot.BPB_SecPerClus << endl;
    cout << "So sector thuoc vung bootsector:       " << boot.BPB_RsvdSecCnt << endl;
    cout << "So bang Fat:                           " << boot.BPB_NumFATs << endl;
    cout << "So entry tren bang RDET:               " << boot.BPB_RootEntCnt << endl;
    cout << "Kich thuoc volume(byte):               " << boot.BPB_TotSec32 << endl;
    cout << "Kich thuoc bang FAT(sector/FAT):       " << boot.BPB_FATSz32 << endl;
    cout << "Cluster bat dau cua RDET:              " << boot.BPB_RootClus << endl;
    cout << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Kich thuoc bang RDET(sector):          " << boot.RootDirSectors << endl;
    cout << "Sector dau tien vung data:             " << boot.FirstDataSector << endl;

    /*cout << "So byte tren moi cluster(byte):        " << boot.byCluster << endl;
    cout << "Tong so sector:                        " << boot.total_sector << endl;*/


    /*BYTE sectorRDET[512];
    ReadSector(L"\\\\.\\E:", 8192, sectorRDET);
    cout << hexTOstring(sectorRDET, 1, 9);*/

    return 0;
}