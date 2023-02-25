#pragma once
#include "framework.h"

typedef struct ICONDIRENTRY
{
	BYTE bWidth;         // Width, in pixels, of the image
	BYTE bHeight;        // Height, in pixels, of the image
	BYTE bColorCount;    // Number of colors in image (0 if >=8bpp)
	BYTE bReserved;      // Reserved ( must be 0)
	WORD wPlanes;        // Color Planes
	WORD wBitCount;      // Bits per pixel
	DWORD dwBytesInRes;  // How many bytes in this resource?
	DWORD dwImageOffset; // Where in the file is this image?
} ICONDIRENTRY, * LPICONDIRENTRY;

typedef struct ICONDIR
{
	WORD idReserved;           // Reserved (must be 0)
	WORD idType;               // Resource Type (1 for icons)
	WORD idCount;              // How many images?
	ICONDIRENTRY idEntries[1]; // An entry for each image (idCount of 'em)
} ICONDIR, * LPICONDIR;

typedef struct ICONIMAGE
{
	BITMAPINFOHEADER icHeader; // DIB header
	RGBQUAD icColors[1];       // Color table
	BYTE icXOR[1];             // DIB bits for XOR mask
	BYTE icAND[1];             // DIB bits for AND mask
} ICONIMAGE, * LPICONIMAGE;

LPVOID NL_MemAlloc(size_t dwSize);
LPVOID NL_MemReAlloc(LPVOID hMem, size_t dwSize);
BOOL NL_MemFree(LPVOID hMem);
BOOL NL_PtInRectWnd(RECT WndRect, RECT Rect, POINT Pos);
INT NL_HitTestSys(RECT WndRect, POINT Pos);
BOOL NL_ReadFile(LPCWSTR filePath, std::vector<CHAR>* retData);
BOOL NL_DialogOpenFile(HWND hWnd, std::wstring* fileName);
UINT_PTR NL_SetTimer(UINT uElapse, TIMERPROC lpTimerFunc);
void NL_KillTimer(UINT_PTR uID);
std::wstring NL_NumSupZero(INT num);
std::wstring NL_Seconds2Str(INT num);
std::wstring NL_GetFileName(std::wstring url);
std::wstring NL_GetFileDirectory(std::wstring url);
INT NL_Str2Int(const std::wstring& str);
RECT NL_WndGetClientRect(HWND hWnd);
std::wstring NL_GetPrivateProfile(LPCTSTR lpFileName, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault);
