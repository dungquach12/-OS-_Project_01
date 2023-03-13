#include<iostream>
#include<Windows.h>
#include<string>

using namespace std;

void removeSpaces(string& str);
string hex2string(BYTE bArr[], int start, int l);
int hex2Int(BYTE bArr[512], int start, int num);
