#include"Support.h"

int hex2Int(BYTE bArr[512], int start, int num) {
	int result = 0;
	for (int i = start + num - 1; i >= start; i--) {
		result = result * 256 + bArr[i];
	}
	return result;
}

string hex2string(BYTE bArr[], int start, int l) {
	string res;
	for (int i = start; i < start + l; i++)
		res += bArr[i];
	return res;
}

// This is referenced by one of code see on geakForgeak website
void removeSpaces(string& str)
{
    int n = str.length();
    int i = 0, j = -1;

    bool spaceFound = false;


    while (++j < n && str[j] == ' ');

    while (j < n)
    {
        if (str[j] != ' ')
        {
            if ((str[j] == '.' || str[j] == ',' ||
                str[j] == '?') && i - 1 >= 0 &&
                str[i - 1] == ' ')
                str[i - 1] = str[j++];

            else
                str[i++] = str[j++];
            spaceFound = false;
        }
        else if (str[j++] == ' ')
        {
            if (!spaceFound)
            {
                str[i++] = ' ';
                spaceFound = true;
            }
        }
    }

    // Remove trailing spaces
    if (i <= 1)
        str.erase(str.begin() + i, str.end());
    else
        str.erase(str.begin() + i - 1, str.end());
}