#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100
#define WM_VIOLET_ANIM 114514
#define VIOLET_PAGES 2
#define VIOLET_OPTPANEL_PAGES 1

#define VIOLET_REPEAT_ALL 0
#define VIOLET_REPEATSIZE 1

std::wstring APP_GetVer();
std::wstring APP_GetVerInfo();
std::wstring APP_GetBuildInfo();

void Wnd_Destroy();

void Wnd_NcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT Wnd_NcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT Wnd_NcLDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT Wnd_NcLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_MouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_MouseLDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_MouseLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_MouseLDBClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_MouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_Command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Wnd_SysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void Wnd_SwitchPage(BOOL playPage);
void CALLBACK Wnd_SideBarEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
void CALLBACK Wnd_OptPageOptPanelBtnEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
void CALLBACK Wnd_ObjEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
void Wnd_ObjSize();

void Wnd_MusicClose(BOOL setInfo);
void Wnd_MusicOpen(LPCWSTR fileName);
void Wnd_MusicPlayPause();
void Wnd_MusicTimeRun(BOOL bRun);
void CALLBACK Wnd_MusicTimeObjEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
void CALLBACK Wnd_MusicTimeTimerProc(HWND hwnd, UINT message, UINT_PTR uID, DWORD dwTime);
