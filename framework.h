// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
//#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

#define WM_TASKBAR_MENU 0x0313

// Windows 头文件
#include <windows.h>
#include <windowsx.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>

#include <Shlobj.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "olectl.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <wincodec.h>
#include <dxgi1_3.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "dxguid.lib")

#include <bass.h>
#pragma comment(lib, "bass.lib")

#include "help.h"
#include "gui.h"
#include "player.h"
