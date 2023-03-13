/*
    This project is reference from fatgen103.docx document,
    take from website: https://download.microsoft.com›fatgen103
*/

#include"Bootsector.h"
#include"REDT.h"

using namespace std;

int main(int argc, char** argv)
{

    BYTE sector[512];
    ReadSector(L"\\\\.\\E:", 0, sector);

    Bootsector boot;
    boot.ReadBS(sector);

    // Xuat ra man hinh
    cout << "Loai Fat:                              " << boot.BS_FilSysType << endl;
    cout << "-------------------------------------------" << endl;
    cout << "So byte tren moi sector(byte):         " << boot.BPB_BytsPerSec << endl;
    cout << "So sector tren cluster:                " << boot.BPB_SecPerClus << endl;
    cout << "So sector thuoc vung bootsector:       " << boot.BPB_RsvdSecCnt << endl;
    cout << "So bang Fat:                           " << boot.BPB_NumFATs << endl;
    cout << "So entry tren bang RDET:               " << boot.BPB_RootEntCnt << endl;
    cout << "Kich thuoc volume(sector):             " << boot.BPB_TotSec32 << endl;
    cout << "Kich thuoc bang FAT(sector/FAT):       " << boot.BPB_FATSz32 << endl;
    cout << "Cluster bat dau cua RDET:              " << boot.BPB_RootClus << endl;
    cout << endl;
    cout << "-------------------------------------------" << endl;
    cout << "Kich thuoc bang RDET(sector):          " << boot.RootDirSectors << endl;
    cout << "Sector dau tien vung data:             " << boot.FirstDataSector << endl;

    cout << "-------------------------------------------" << endl;

    cout << "Xem noi dung trong bang RDET? (Press Enter)!" << endl;
    cin.ignore();
    cout << endl;
    File f;
    f.readRDET(boot);

    return 0;
}