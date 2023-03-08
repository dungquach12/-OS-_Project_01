#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <stack>
using namespace std;
typedef long long ll;
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
        //printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, numSector * 512, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        //printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        //printf("\nSuccess!\n");
    }
}
string hexToString(BYTE b[], int start, int num) {
    string byteString;

    for (int i = start; i < start + num; i++) {
        byteString += b[i];
    }
    return byteString;
}
ll hexToInt(BYTE b[512], int start, int num) {
    ll result = 0;
    for (int i = start + num - 1; i >= start; i--) {
        result = result * 256 + b[i];
    }
    return result;
}
string fatType(BYTE b[]) {
    return hexToString(b, 5 * 16 + 2, 8);
}
ll sectorPerCluster(BYTE b[]) {
    return hexToInt(b, 13, 1);
}
ll sectorInBootSector(BYTE b[]) {
    return hexToInt(b, 14, 2);
}
ll numFatTable(BYTE b[]) {
    return hexToInt(b, 16, 1);
}


ll sectorPerFatTable(BYTE b[]) {
    return hexToInt(b, 16 * 2 + 4, 4);
}
ll volumnSize(BYTE b[]) {
    return hexToInt(b, 16 * 2 + 0, 4);
}
ll sectorByCluster(BYTE b[], ll k) {
    return sectorInBootSector(b) + sectorPerFatTable(b) * numFatTable(b) + (k - 2) * sectorPerCluster(b);
}
ll startSectorOfRDET(BYTE b[]) {
    ll k = hexToInt(b, 2 * 16 + 12, 4);
    return sectorByCluster(b, k);
}
ll startSectorOfFAT(BYTE b[]) {
    return sectorInBootSector(b);
}
ll fatSize(BYTE b[]) {

    return hexToInt(b, 16 * 2 + 4, 4);
}
string readName(stack<string>& s) {
    string name;
    while (!s.empty()) {
        name += s.top();
        s.pop();
    }
    return name;
}
int readRDET(BYTE b[]) {
    stack<string> nameEntry;
    ll k = 0;
    ll currentOfFAT = 8;
    int isStarted = 0;
    ll start;
    while (k < 100) {
        BYTE FATSector[512];
        BYTE RDETSector[512];
        ReadSector(L"\\\\.\\E:", startSectorOfRDET(b) + k, RDETSector);
        ReadSector(L"\\\\.\\E:", startSectorOfFAT(b), FATSector);

        start = 0;
        while (start < 512) {
            ll type = hexToInt(RDETSector, start + 11, 1);
            ll firstByte = hexToInt(RDETSector, start, 1);
            if (firstByte == 229) {
                start += 32;
                continue;
            }
            if (firstByte == 0) {
                cout << endl << k;
                return -1;
            }
            string name = hexToString(RDETSector, start, 8);
            nameEntry.push(name);

            string extension = hexToString(RDETSector, start + 8, 3);
            if (type != 15) {
                isStarted++;
                if (isStarted <= 2) {
                    start += 32;
                    continue;
                }

                ll startClustorHigh = hexToInt(RDETSector, start + 16 + 4, 2);
                ll startClustorLow = hexToInt(RDETSector, start + 16 + 10, 2);
                ll startClustor = startClustorHigh * 256 * 256 + startClustorLow;


                name = readName(nameEntry);

                cout << "Name: " << name << '\n' << "Type: " << type << "\n" << "Extension: " << extension << '\n';
                cout << "Start clustor: " << startClustor << '\n';
                cout << "Start at: " << start / 32 << '\n';

                ll currentClustor = startClustor;
                ll startSector = sectorByCluster(b, currentClustor);
                ll endSector = startSector;
                while (true) {
                    ll nextClustor = hexToInt(FATSector, currentOfFAT, 4);
                    currentOfFAT += 4;
                    endSector += sectorPerCluster(b);
                    if (nextClustor == 268435455)
                        break;
                }
                endSector--;

                cout << "Start sector: " << startSector << " End sector: " << endSector << '\n' << '\n';
            }
            start += 32;
        }
        k++;
    }

    return 1;

}

//clustor bat dau bang FAT -> tim clustor ket thuc
int main(int argc, char** argv)
{

    BYTE sector[512];
    ReadSector(L"\\\\.\\E:", 0, sector);
    /* cout << "Fat type: " << fatType(sector);
      cout << endl << "Volumn size: " << volumnSize(sector);

       cout << endl << "Sector per cluster: " << sectorPerCluster(sector);
     cout << endl << "Number of sector in boot sector: " << sectorInBootSector(sector);
     cout << endl << "Number of fat table: " << numFatTable(sector);
     cout << endl << "Fat size: " << fatSize(sector);
     cout << endl << "Number of sector in each cluster: " << sectorByCluster(sector, 3);
     cout << endl << "Start sector of RDET: " << startSectorOfRDET(sector);
     BYTE RDETSector[512];
     ReadSector(L"\\\\.\\E:", startSectorOfRDET(sector) + 3, RDETSector);
     for (int i = 0; i < 512; i++)
         cout << RDETSector[i];*/
    cout << endl << "Sector per cluster: " << sectorPerCluster(sector);
    readRDET(sector);
    return 0;
}