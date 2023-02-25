#include "help.h"

LPVOID NL_MemAlloc(size_t dwSize)
{
	return HeapAlloc(GetProcessHeap(), 8, dwSize);
}

LPVOID NL_MemReAlloc(LPVOID hMem, size_t dwSize)
{
	return HeapReAlloc(GetProcessHeap(), 8, hMem, dwSize);
}

BOOL NL_MemFree(LPVOID hMem)
{
	if (hMem != nullptr)
	{
		return HeapFree(GetProcessHeap(), 0, hMem);
	}
	return FALSE;
}

BOOL NL_PtInRectWnd(RECT WndRect, RECT Rect, POINT Pos) 
{
	if (Pos.x <= Rect.left + WndRect.left) 
		return FALSE;
	if (Pos.x >= Rect.right + Rect.left + WndRect.left)
		return FALSE;
	if (Pos.y <= Rect.top + WndRect.top)
		return FALSE;
	if (Pos.y >= Rect.bottom + Rect.top + WndRect.top)
		return FALSE;
	return TRUE;
}

INT NL_HitTestSys(RECT WndRect, POINT Pos)
{
	RECT LeftTopRc{ 0, 0, 8, 8 };
	RECT RightTopRc{ WndRect.right - 8, 0, 8, 8 };
	RECT LeftBottomRc{ 0, WndRect.bottom - 8, 8, 8 };
	RECT RightBottomRc{ WndRect.right - 8, WndRect.bottom - 8, 8, 8 };

	RECT TopRc{ 0, 0, WndRect.right, 8 };
	RECT LeftRc{ 0, 0, 8, WndRect.bottom };
	RECT RightRc{ WndRect.right - 8, 0, 8, WndRect.bottom };
	RECT BottomRc{ 0, WndRect.bottom - 8, WndRect.right, 8 };

	RECT TitleRc{ 0, 8, WndRect.right, 30 };

	if (NL_PtInRectWnd(WndRect, LeftTopRc, Pos))
		return HTTOPLEFT;
	if (NL_PtInRectWnd(WndRect, RightTopRc, Pos))
		return HTTOPRIGHT;
	if (NL_PtInRectWnd(WndRect, LeftBottomRc, Pos))
		return HTBOTTOMLEFT;
	if (NL_PtInRectWnd(WndRect, RightBottomRc, Pos))
		return HTBOTTOMRIGHT;

	if (NL_PtInRectWnd(WndRect, TopRc, Pos))
		return HTTOP;
	if (NL_PtInRectWnd(WndRect, LeftRc, Pos))
		return HTLEFT;
	if (NL_PtInRectWnd(WndRect, RightRc, Pos))
		return HTRIGHT;
	if (NL_PtInRectWnd(WndRect, BottomRc, Pos))
		return HTBOTTOM;

	if (NL_PtInRectWnd(WndRect, TitleRc, Pos))
		return HTCAPTION;

	return HTCLIENT;
}

BOOL NL_ReadFile(LPCWSTR filePath, std::vector<CHAR>* retData)
{
	BOOL fOK = FALSE;
	if (filePath != L"")
	{
		HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD nSize = GetFileSize(hFile, NULL);
			if (nSize != INVALID_FILE_SIZE)
			{
				(*retData).resize(nSize);
				fOK = ReadFile(hFile, &(*retData)[0], nSize, &nSize, NULL);
			}
			CloseHandle(hFile);
		}
	}
	return fOK;
}

BOOL NL_DialogOpenFile(HWND hWnd, std::wstring* fileName) 
{
	BOOL ret = FALSE;

	std::vector<WCHAR> file;
	file.resize(MAX_PATH + 1);
	std::vector<WCHAR> defile;
	defile.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(defile.size(), defile.data());

	OPENFILENAMEW ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = file.data();
	ofn.nMaxFile = file.size();
	//ofn.lpstrFilter = L"All Files(*.*)*.*";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = L"/";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_DONTADDTORECENT;
	if (GetOpenFileNameW(&ofn))
	{
		*fileName = file.data();
		ret = TRUE;
	}
	SetCurrentDirectoryW(defile.data());

	return ret;
}

UINT_PTR NL_SetTimer(UINT uElapse, TIMERPROC lpTimerFunc) 
{
	UINT_PTR ret = NULL;
	ret = SetTimer(NULL, NULL, uElapse, lpTimerFunc);
	return ret;
}

void NL_KillTimer(UINT_PTR uID)
{
	KillTimer(NULL, uID);
}

std::wstring NL_NumSupZero(INT num)
{
	std::wstring str = std::to_wstring(num);
	return str;
}

std::wstring NL_Seconds2Str(INT num)
{
	INT minute = 0;
	INT second = 0;
	INT fan1 = num;
	std::wstring str;
	second = fan1 - 6000 * minute;
	while (second > 59)
	{
		second -= 60;
		minute += 1;
	}
	str = NL_NumSupZero(minute) + L":" + NL_NumSupZero(second);
	return str;
}

std::wstring NL_GetFileName(std::wstring url)
{
	std::wstring::size_type iPos = url.find_last_of(L"\\") + 1;
	std::wstring filename = url.substr(iPos, url.length() - iPos);
	std::wstring name;
	name = filename;
	return name;
}

std::wstring NL_GetFileDirectory(std::wstring url)
{
	std::wstring::size_type iPos = url.rfind(L"\\");
	std::wstring filename = url.substr(0, iPos);
	std::wstring name = filename.substr(0, filename.rfind(L"."));
	return name;
}

INT NL_Str2Int(const std::wstring& str)
{
	if (!str.empty()) 
	{
		return std::stoi(str);
	}
	return 0;
}

RECT NL_WndGetClientRect(HWND hWnd)
{
	WINDOWINFO pwi;
	pwi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hWnd, &pwi);
	return pwi.rcClient;
}

std::wstring NL_GetPrivateProfile(LPCTSTR lpFileName, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault)
{
	std::wstring ret;
	ret.resize(2048);
	DWORD size = 0;
	size = GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, (LPWSTR)ret.c_str(), 2048, lpFileName);
	ret.resize(size);
	return ret;
}
