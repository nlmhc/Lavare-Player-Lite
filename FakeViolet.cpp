// FakeViolet.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "FakeViolet.h"

// 全局变量:
HINSTANCE hInst;
LPCWSTR szTitle = L"FakeViolet";
LPCWSTR szVer = L"0.5_Lite";
LPCWSTR szBuild = L"182";
LPCWSTR szBTime = L"230225-2141";
LPCWSTR szBranch = L"wtf_release";
LPCWSTR szWindowClass = L"com.nlmhc.FakeViolet.MainWnd";
LPCWSTR lpPageTitle[VIOLET_PAGES] = { L"主页", L"设置" };
LPCWSTR lpOptPanelPageTitle[VIOLET_OPTPANEL_PAGES] = { L"关于"};
INT nPage = 0;
INT nOptPageOptPanelPage = 0;
NLGUIWnd pGUI;
HWND hMainWnd;
INT nTmpWidth, nTmpHeight;
BOOL bPlayPage;
NLGUIObj* hDownObj;

NLPlayerMusic pMusic;

UINT_PTR uTimerID_Music;

HGUIIMG hImg_Cover;
HGUIIMG hImg_PlayBtn[2];


//主界面
NLGUIObj_Label hObj_MainTitle;
NLGUIObj_Page hObj_SideBar;
NLGUIObj_Btn2 hObj_SideBar_Btn[VIOLET_PAGES];
NLGUIObj_Inst hObj_SideBar_Inst;
NLGUIObj_Page hObj_MainPage;
NLGUIObj_Page* hObj_Pages[VIOLET_PAGES];

//主-主页
NLGUIObj_Page hObj_HomePage;
NLGUIObj_Btn1 hObj_HomePage_OpenBtn[2];
NLGUIObj_Img hObj_HomePage_Img;
NLGUIObj_Label hObj_HomePage_HisLabel;

//主-设置
NLGUIObj_Page hObj_OptPage;
NLGUIObj_Con1 hObj_OptPage_OptPanel;
NLGUIObj_Label hObj_OptPage_OptPanel_Title;
NLGUIObj_Btn3 hObj_OptPage_OptPanel_Btn[VIOLET_OPTPANEL_PAGES];
NLGUIObj_Page* hObj_OptPage_OptPanel_Pages[VIOLET_OPTPANEL_PAGES];

NLGUIObj_Page hObj_OptPage_OptPanel_AboutPage;
NLGUIObj_Img hObj_OptPage_OptPanel_AboutPage_LogoImg;
NLGUIObj_Label hObj_OptPage_OptPanel_AboutPage_NameLabel;
NLGUIObj_Label hObj_OptPage_OptPanel_AboutPage_VerLabel;
NLGUIObj_Label hObj_OptPage_OptPanel_AboutPage_ThanksLabel;
NLGUIObj_Label hObj_OptPage_OptPanel_AboutPage_AuthorLabel;
NLGUIObj_Btn1 hObj_OptPage_OptPanel_AboutPage_AuthorBtn[3];

NLGUIObj_Page hObj_PlayPage;
NLGUIObj_Label hObj_PlayPage_Title1;
NLGUIObj_Label hObj_PlayPage_Title2;
NLGUIObj_Label hObj_PlayPage_Title3;
NLGUIObj_Img hObj_PlayPage_Cover;

NLGUIObj_Page hObj_PlayCon;
NLGUIObj_Con2 hObj_PlayCon_Panel;
NLGUIObj_Page hObj_PlayCon_TitlePage;
NLGUIObj_CoverBtn hObj_PlayCon_Cover;
NLGUIObj_Btn2 hObj_PlayCon_SwitchPageBtn;
NLGUIObj_Label hObj_PlayCon_Title1;
NLGUIObj_Label hObj_PlayCon_Title2;
NLGUIObj_Page hObj_PlayCon_ConPage;
NLGUIObj_Btn2 hObj_PlayCon_PlayBtn;
NLGUIObj_Btn2 hObj_PlayCon_PrevBtn;
NLGUIObj_Btn2 hObj_PlayCon_NextBtn;
NLGUIObj_Label hObj_PlayCon_Time[2];
NLGUIObj_Slider hObj_PlayCon_ProgressBar;

NLGUIObj_Btn1 hObj_test;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	NLGUI_Init();

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FAKEVIOLET));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FAKEVIOLET));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	hMainWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hMainWnd)
	{
		return FALSE;
	}

	NLPlayer_Init(hMainWnd);
	pGUI.Bind(hMainWnd);

	LPCWSTR sbIcon[] = {
	L"skin/page_home.png",
	L"skin/page_options.png"
	};
	hImg_Cover = NLGUI_ImgLoadFile(L"skin/cover.png");
	hImg_PlayBtn[0] = NLGUI_ImgLoadFile(L"skin/player_play.png");
	hImg_PlayBtn[1] = NLGUI_ImgLoadFile(L"skin/player_pause.png");

	//主界面
	hObj_MainPage.Create(&pGUI, 0, 0, 1000, 1000);

	//主-主页
	hObj_HomePage.Create(&hObj_MainPage, 65, 0, 1000, 1000);
	hObj_Pages[0] = &hObj_HomePage;
	hObj_HomePage_OpenBtn[0].Create(&hObj_HomePage, L"打开文件", NLGUI_ImgLoadFile(L"skin/open_file.png"), 10, 15, 20, 0, 0, 64, 300, 50);
	hObj_HomePage_OpenBtn[0].SetEvent(Wnd_ObjEvent);
	hObj_HomePage_OpenBtn[1].Create(&hObj_HomePage, L"打开文件夹", NLGUI_ImgLoadFile(L"skin/open_folder.png"), 10, 15, 20, 0, 0, 122, 300, 50);
	hObj_HomePage_OpenBtn[1].SetEvent(Wnd_ObjEvent);
	hObj_HomePage_HisLabel.Create(&hObj_HomePage, L"最近播放", 18, TRUE, 0, 0, NLGUI_THEMECOLOR_TEXT1, FALSE, 0, 195, 1000, 225);
	hObj_HomePage_Img.Create(&hObj_HomePage, NLGUI_ImgLoadFile(L"skin/impress.png"), 1, 12, FALSE, TRUE, FALSE, 0, 0, 0, 0);

	//主-设置
	hObj_OptPage.Create(&hObj_MainPage, 65, 0, 1000, 1000);
	hObj_Pages[1] = &hObj_OptPage;
	hObj_OptPage_OptPanel.Create(&hObj_OptPage, 280, 40, 1000, 500);
	hObj_OptPage_OptPanel_Title.Create(&hObj_OptPage, L"", 17, TRUE, 0, 2, NLGUI_THEMECOLOR_TEXT1, FALSE, 295, 40, 1000, 40);

	hObj_OptPage_OptPanel_AboutPage.Create(&hObj_OptPage, 280, 80, 1000, 1000);
	hObj_OptPage_OptPanel_Pages[0] = &hObj_OptPage_OptPanel_AboutPage;
	hObj_OptPage_OptPanel_AboutPage_NameLabel.Create(&hObj_OptPage_OptPanel_AboutPage, L"FakeViolet Player", 17, TRUE, 0, 2, NLGUI_THEMECOLOR_TEXT1, FALSE, 60, 8, 1000, 32);
	hObj_OptPage_OptPanel_AboutPage_LogoImg.Create(&hObj_OptPage_OptPanel_AboutPage, NLGUI_ImgLoadFile(L"skin/FakeViolet.ico"), 0, 0, FALSE, FALSE, FALSE, 15, 8, 32, 32);
	std::wstring lpAboutVer = L"FakeViolet Player ";
	lpAboutVer += APP_GetVerInfo();
	lpAboutVer += L"\n\n";
	lpAboutVer += APP_GetBuildInfo();
	hObj_OptPage_OptPanel_AboutPage_VerLabel.Create(&hObj_OptPage_OptPanel_AboutPage, lpAboutVer.c_str(), 14, FALSE, 0, 0, NLGUI_THEMECOLOR_TEXT1, TRUE, 15, 48, 1000, 80);
	hObj_OptPage_OptPanel_AboutPage_ThanksLabel.Create(&hObj_OptPage_OptPanel_AboutPage, L"FakeViolet Player 的音频播放功能基于 BassLib 开发。", 14, FALSE, 0, 0, NLGUI_THEMECOLOR_TEXT1, TRUE, 15, 148, 1000, 40);
	hObj_OptPage_OptPanel_AboutPage_AuthorLabel.Create(&hObj_OptPage_OptPanel_AboutPage, L"By：nlmhc", 14, FALSE, 0, 0, NLGUI_THEMECOLOR_TEXT1, TRUE, 0, 0, 1000, 40);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[0].Create(&hObj_OptPage_OptPanel_AboutPage, L"GitHub", NLGUI_ImgLoadFile(L"skin/about_github.png"), 5, 13, 16, 0, 0, 64, 300, 50);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[0].SetEvent(Wnd_ObjEvent);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[1].Create(&hObj_OptPage_OptPanel_AboutPage, L"bilibili", NLGUI_ImgLoadFile(L"skin/about_bilibili.png"), 5, 13, 16, 0, 0, 64, 300, 50);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[1].SetEvent(Wnd_ObjEvent);
	for (INT i = 0; i < VIOLET_OPTPANEL_PAGES; i++)
	{
		hObj_OptPage_OptPanel_Btn[i].Create(&hObj_OptPage, lpOptPanelPageTitle[i], 6, 15, 32, 0, 80 + i * 45, 250, 41);
		hObj_OptPage_OptPanel_Btn[i].SetEvent(Wnd_OptPageOptPanelBtnEvent);
	}
	Wnd_OptPageOptPanelBtnEvent(&hObj_OptPage_OptPanel_Btn[nOptPageOptPanelPage], NLGUI_EVENT_MOUSELUP, 0, 0, 0, 0);

	//主-主标题及侧栏
	hObj_MainTitle.Create(&hObj_MainPage, L"MainTitle", 20, TRUE, 0, 2, NLGUI_THEMECOLOR_TEXT1, FALSE, 65, 12, 350, 40);
	
	hObj_SideBar.Create(&hObj_MainPage, 0, 0, 65, 1000);
	for (INT i = 0; i < VIOLET_PAGES; i++)
	{
		hObj_SideBar_Btn[i].Create(&hObj_SideBar, NLGUI_ImgLoadFile(sbIcon[i]), 6, 18, 4, 35 + i * 45, 41, 41);
		hObj_SideBar_Btn[i].SetEvent(Wnd_SideBarEvent);
	}
	hObj_SideBar_Inst.Create(&hObj_SideBar, 2, 0, 0, 0, 0);

	//播放页
	hObj_PlayPage.Create(&pGUI, 0, 0, 1000, 1000);
	hObj_PlayPage_Cover.Create(&hObj_PlayPage, hImg_Cover, 1, 20, TRUE, FALSE, FALSE, 65, 125, 200, 200);
	hObj_PlayPage_Title1.Create(&hObj_PlayPage, L"", 19, TRUE, 0, 2, NLGUI_THEMECOLOR_TEXT1, FALSE, 65, 345, 200, 30);
	hObj_PlayPage_Title2.Create(&hObj_PlayPage, L"", 16, FALSE, 0, 2, NLGUI_THEMECOLOR_TEXT2, FALSE, 65, 375, 200, 30);
	hObj_PlayPage_Title3.Create(&hObj_PlayPage, L"", 16, FALSE, 0, 2, NLGUI_THEMECOLOR_TEXT2, FALSE, 65, 405, 200, 30);

	//播放器控制栏
	hObj_PlayCon.Create(&pGUI, 0, 0, 0, 115);
	hObj_PlayCon_Panel.Create(&hObj_PlayCon, 0, 0, 0, 0);
	hObj_PlayCon_ConPage.Create(&hObj_PlayCon, 0, 0, 0, 115);
	hObj_PlayCon_TitlePage.Create(&hObj_PlayCon, 72, 0, 0, 115);
	hObj_PlayCon_Title1.Create(&hObj_PlayCon_TitlePage, L"Title", 19, TRUE, 0, 2, NLGUI_THEMECOLOR_TEXT1, FALSE, 100, 25, 130, 30);
	hObj_PlayCon_Title2.Create(&hObj_PlayCon_TitlePage, L"Title", 16, FALSE, 0, 2, NLGUI_THEMECOLOR_TEXT2, FALSE, 100, 58, 130, 30);
	hObj_PlayCon_Cover.Create(&hObj_PlayCon_TitlePage, hImg_Cover, NLGUI_ImgLoadFile(L"skin/playpage_up.png"), 0, 12, 92, 92);
	hObj_PlayCon_Cover.SetEvent(Wnd_ObjEvent);
	hObj_PlayCon_SwitchPageBtn.Create(&hObj_PlayCon, NLGUI_ImgLoadFile(L"skin/playpage_down.png"), 6, 16, 12, 68, 38, 38);
	hObj_PlayCon_SwitchPageBtn.SetEvent(Wnd_ObjEvent);
	hObj_PlayCon_Time[0].Create(&hObj_PlayCon_ConPage, L"00:00", 14, FALSE, 0, 2, NLGUI_THEMECOLOR_TEXT2, FALSE, 0, 25, 350, 30);
	hObj_PlayCon_Time[1].Create(&hObj_PlayCon_ConPage, L"00:00", 14, FALSE, 1, 2, NLGUI_THEMECOLOR_TEXT2, FALSE, 0, 25, 350, 30);
	hObj_PlayCon_PlayBtn.Create(&hObj_PlayCon_ConPage, hImg_PlayBtn[0], 20, 16, 0, 0, 40, 40);
	hObj_PlayCon_PlayBtn.SetEvent(Wnd_ObjEvent);
	hObj_PlayCon_PlayBtn.SetSelect(TRUE);
	hObj_PlayCon_PrevBtn.Create(&hObj_PlayCon_ConPage, NLGUI_ImgLoadFile(L"skin/player_prev.png"), 20, 16, 0, 0, 40, 40);
	hObj_PlayCon_PrevBtn.SetEvent(Wnd_ObjEvent);
	hObj_PlayCon_NextBtn.Create(&hObj_PlayCon_ConPage, NLGUI_ImgLoadFile(L"skin/player_next.png"), 20, 16, 0, 0, 40, 40);
	hObj_PlayCon_NextBtn.SetEvent(Wnd_ObjEvent);
	hObj_PlayCon_ProgressBar.Create(&hObj_PlayCon_ConPage, TRUE, FALSE, 0, 45, 0, 20);
	hObj_PlayCon_ProgressBar.SetInfo(0, 100, 0);
	hObj_PlayCon_ProgressBar.SliderSetPos(0);
	hObj_PlayCon_ProgressBar.SetEvent(Wnd_MusicTimeObjEvent);

	//显示窗口
	Wnd_SideBarEvent(&hObj_SideBar_Btn[nPage], NLGUI_EVENT_MOUSELUP, 0, 0, 0, 0);
	Wnd_SwitchPage(FALSE);
	Wnd_MusicClose(TRUE);
	SetWindowPos(hMainWnd, NULL, 0, 0, 1070, 640, SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
	ShowWindow(hMainWnd, nCmdShow);
	//UpdateWindow(hMainWnd);

	return TRUE;
}

std::wstring APP_GetVer()
{
	std::wstring str = szVer;
	return str;
}

std::wstring APP_GetVerInfo()
{
	std::wstring str = APP_GetVer() + L" / C++ Edition";
	return str;
}

std::wstring APP_GetBuildInfo()
{
	std::wstring str;
	std::wstring build = szBuild;
	std::wstring branch = szBranch;
	std::wstring btime = szBTime;
	str += L"Build " + build;
	str += L"." + branch;
	str += L"." + btime;
	return str;
}

void Wnd_MusicClose(BOOL setInfo)
{
	Wnd_MusicTimeRun(FALSE);
	hObj_PlayCon_Cover.SetImg(NULL);
	hObj_PlayPage_Cover.SetImg(NULL);
	NLGUI_ImgFree(hImg_Cover);
	hImg_Cover = NULL;
	pMusic.Free();

	if (setInfo)
	{
		hObj_PlayCon_Title1.SetTitle(szTitle);
		hObj_PlayCon_Title2.SetTitle(L"Player");
		hObj_PlayPage_Title1.SetTitle(szTitle);
		hObj_PlayPage_Title2.SetTitle(L"Player");
		hObj_PlayPage_Title3.SetTitle(L"Title");

		hImg_Cover = NLGUI_ImgLoadFile(L"skin/cover.png");
		hObj_PlayCon_Cover.SetImg(hImg_Cover);
		hObj_PlayPage_Cover.SetImg(hImg_Cover);
	}
}

void Wnd_MusicOpen(LPCWSTR url)
{
	std::vector<CHAR> data;
	NLPlayerMusic pMusic_tmp;
	if (pMusic_tmp.Load(url))
	{
		Wnd_MusicClose(FALSE);
		pMusic = pMusic_tmp;

		hObj_PlayCon_Title1.SetTitle(NL_GetFileName(url).c_str());
		hObj_PlayCon_Title2.SetTitle(L"");
		hObj_PlayPage_Title1.SetTitle(NL_GetFileName(url).c_str());
		hObj_PlayPage_Title2.SetTitle(L"");
		hObj_PlayPage_Title3.SetTitle(L"");

		hImg_Cover = NLGUI_ImgLoadFile(L"skin/cover.png");
		hObj_PlayCon_Cover.SetImg(hImg_Cover);
		hObj_PlayPage_Cover.SetImg(hImg_Cover);

		Wnd_MusicPlayPause();

		Wnd_MusicTimeRun(TRUE);
	}
}

void Wnd_MusicPlayPause()
{
	pMusic.Pause();

	if (pMusic.IsPlaying())
	{
		hObj_PlayCon_PlayBtn.SetImg(hImg_PlayBtn[1]);
	}
	else
	{
		hObj_PlayCon_PlayBtn.SetImg(hImg_PlayBtn[0]);
	}
	Wnd_MusicTimeRun(TRUE);
}

void Wnd_MusicTimeRun(BOOL bRun) 
{
	if (bRun && pMusic.IsPlaying())
	{
		if (uTimerID_Music == 0)
		{
			uTimerID_Music = NL_SetTimer(250, Wnd_MusicTimeTimerProc);
		}
	}
	else 
	{
		NL_KillTimer(uTimerID_Music);
		uTimerID_Music = 0;
	}
}

void CALLBACK Wnd_MusicTimeTimerProc(HWND hwnd, UINT message, UINT_PTR uID, DWORD dwTime)
{
	hObj_PlayCon_ProgressBar.SetInfo(0, pMusic.GetLength(), 0);
	hObj_PlayCon_ProgressBar.SliderSetPos(pMusic.GetPos());
	hObj_PlayCon_Time[0].SetTitle(NL_Seconds2Str(pMusic.GetPos()).c_str());
	hObj_PlayCon_Time[1].SetTitle(NL_Seconds2Str(pMusic.GetLength()).c_str());
	hObj_PlayCon_ProgressBar.Redraw();
}

void CALLBACK Wnd_MusicTimeObjEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSELDOWN:
		//Wnd_MusicTimeRun(FALSE);
		break;
	case NLGUI_EVENT_MOUSELUP:
		//pMusic.SetPos(((NLGUIObj_Slider*)pObj)->GetPos());
		//Wnd_MusicTimeRun(TRUE);
		break;
	case NLGUI_EVENT_SLIDER_POS:
		//hObj_PlayCon_Time[0].SetTitle(NL_Seconds2Str(((NLGUIObj_Slider*)pObj)->GetPos()).c_str());
		break;
	}
}

void Wnd_Size(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	nTmpWidth = GET_X_LPARAM(lParam);
	nTmpHeight = GET_Y_LPARAM(lParam);
	INT width = nTmpWidth, height = nTmpHeight;

	MARGINS mar{};
	mar.cyTopHeight = -1;
	DwmExtendFrameIntoClientArea(hWnd, &mar);

	Wnd_ObjSize();

	pGUI.Resize(0, 0);
	pGUI.Paint();
}

void Wnd_ObjSize()
{
	INT width = nTmpWidth, height = nTmpHeight;

	hObj_MainPage.SetPos(0, 0, width, height);
	hObj_HomePage.SetPos(65, 0, width - 65, height);
	hObj_HomePage_Img.SetPos((width - 65) / 2 - 65 + 80, 64, (width - 65) / 2 - 65, height - 214);
	hObj_HomePage_OpenBtn[0].SetPos(0, 64, (width - 65) / 2 - 45, 50);
	hObj_HomePage_OpenBtn[1].SetPos(0, 122, (width - 65) / 2 - 45, 50);
	hObj_OptPage.SetPos(65, 0, width - 65, height);
	hObj_OptPage_OptPanel.SetPos(280, 40, width - 375, height - 175);
	hObj_OptPage_OptPanel_AboutPage_AuthorLabel.SetPos(15, height - 175 - 115, 300, 30);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[0].SetPos(15, height - 175 - 85, 115, 35);
	hObj_OptPage_OptPanel_AboutPage_AuthorBtn[1].SetPos(145, height - 175 - 85, 115, 35);

	hObj_SideBar.SetPos(0, 0, 65, height);

	hObj_PlayPage.SetPos(0, 0, width, height);

	FLOAT playconLeft = bPlayPage ? 110 : 315;
	FLOAT playconRight = bPlayPage ? 100 : 65;
	FLOAT playconCover = bPlayPage ? 0 : 92;
	hObj_PlayCon.SetPos(0, height - 115, width, 115);
	hObj_PlayCon_Panel.SetPos(0, 0, width, 115);
	hObj_PlayCon_ConPage.SetPos(playconLeft, 0, width - playconLeft - playconRight, 115);
	hObj_PlayCon_Time[0].SetPos(0, 20, width - playconLeft - playconRight, 30);
	hObj_PlayCon_Time[1].SetPos(0, 20, width - playconLeft - playconRight, 30);
	hObj_PlayCon_PlayBtn.SetPos((width - playconLeft - playconRight) / 2 - 40 / 2, 65, 40, 40);
	hObj_PlayCon_PrevBtn.SetPos((width - playconLeft - playconRight) / 2 - 40 / 2 - 62, 65, 40, 40);
	hObj_PlayCon_NextBtn.SetPos((width - playconLeft - playconRight) / 2 - 40 / 2 + 62, 65, 40, 40);
	hObj_PlayCon_ProgressBar.SetPos(0, 45, width - playconLeft - playconRight, 20);
	hObj_PlayCon_Cover.SetPos(0, 12, playconCover, 92);
	hObj_PlayCon_Title1.SetPos(playconCover + 16, 25, 120, 30);
	hObj_PlayCon_Title2.SetPos(playconCover + 16, 58, 120, 30);
}

void Wnd_SwitchPage(BOOL playPage) 
{
	bPlayPage = playPage;
	if (bPlayPage)
	{
		hObj_PlayPage.SetVisible(TRUE);
		hObj_MainPage.SetVisible(FALSE);
		hObj_PlayCon_Panel.SetVisible(FALSE);
		hObj_PlayCon_TitlePage.SetVisible(FALSE);
		hObj_PlayCon_SwitchPageBtn.SetVisible(TRUE);
		hObj_PlayPage.Redraw();
	}
	else 
	{
		hObj_MainPage.SetVisible(TRUE);
		hObj_PlayPage.SetVisible(FALSE);
		hObj_PlayCon_Panel.SetVisible(TRUE);
		hObj_PlayCon_TitlePage.SetVisible(TRUE);
		hObj_PlayCon_SwitchPageBtn.SetVisible(FALSE);
		hObj_MainPage.Redraw();
	}
	Wnd_ObjSize();
}

void CALLBACK Wnd_SideBarEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSELUP:
		for (INT i = 0; i < VIOLET_PAGES; i++)
		{
			if (&hObj_SideBar_Btn[i] == pObj)
			{
				nPage = i;
				hObj_SideBar_Btn[i].SetSelect(TRUE);
				hObj_Pages[i]->SetVisible(TRUE);
				hObj_MainTitle.SetTitle(lpPageTitle[i]);
				hObj_SideBar_Inst.SetPos(4, 35 + i * 45 + 10, 4, 22);
			}
			else 
			{
				hObj_SideBar_Btn[i].SetSelect(FALSE);
				hObj_Pages[i]->SetVisible(FALSE);
			}
		}
		hObj_MainPage.Redraw();
		break;
	}
}

void CALLBACK Wnd_OptPageOptPanelBtnEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSELUP:
		for (INT i = 0; i < VIOLET_OPTPANEL_PAGES; i++)
		{
			if (&hObj_OptPage_OptPanel_Btn[i] == pObj)
			{
				nOptPageOptPanelPage = i;
				hObj_OptPage_OptPanel_Btn[i].SetSelect(TRUE);
				hObj_OptPage_OptPanel_Pages[i]->SetVisible(TRUE);
				hObj_OptPage_OptPanel_Title.SetTitle(lpOptPanelPageTitle[i]);
			}
			else
			{
				hObj_OptPage_OptPanel_Btn[i].SetSelect(FALSE);
				hObj_OptPage_OptPanel_Pages[i]->SetVisible(FALSE);
			}
		}
		hObj_OptPage.Redraw();
		break;
	}
}

void CALLBACK Wnd_ObjEvent(LPVOID pObj, INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSELUP:
		if (pObj == &hObj_PlayCon_Cover)
		{
			Wnd_SwitchPage(TRUE);
		}
		else if (pObj == &hObj_PlayCon_SwitchPageBtn)
		{
			Wnd_SwitchPage(FALSE);
		}
		else if (pObj == &hObj_PlayCon_PlayBtn)
		{
			Wnd_MusicPlayPause();
		}
		else if (pObj == &hObj_HomePage_OpenBtn[0])
		{
			std::wstring file;
			if (NL_DialogOpenFile(hMainWnd, &file))
			{
				Wnd_SwitchPage(TRUE);
				Wnd_MusicOpen(file.c_str());
			}
		}
		else if (pObj == &hObj_OptPage_OptPanel_AboutPage_AuthorBtn[0])
		{
			ShellExecuteW(NULL, L"open", L"https://github.com/nlmhc", NULL, NULL, SW_SHOWNORMAL);
		}
		else if (pObj == &hObj_OptPage_OptPanel_AboutPage_AuthorBtn[1])
		{
			ShellExecuteW(NULL, L"open", L"https://space.bilibili.com/3140596", NULL, NULL, SW_SHOWNORMAL);
		}
		break;
	}

	//output((INT)hObj, nEvent);
}

void Wnd_Destroy() 
{
	PostQuitMessage(0);
}

void Wnd_NcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		NCCALCSIZE_PARAMS nc{};
		memmove(&nc, (LPVOID)lParam, sizeof(NCCALCSIZE_PARAMS));
		INT cw = GetSystemMetrics(SM_CXSIZEFRAME), ch = GetSystemMetrics(SM_CYSIZEFRAME);
		if (IsZoomed(hWnd))
		{
			nc.rgrc[0].left = nc.rgrc[0].left + cw * 2;
			nc.rgrc[0].top = nc.rgrc[0].top;
			nc.rgrc[0].right = nc.rgrc[0].right - cw * 2;
			nc.rgrc[0].bottom = nc.rgrc[0].bottom - ch * 2;
		}
		else
		{
			nc.rgrc[0].left = nc.rgrc[0].left + cw * 2;
			nc.rgrc[0].top = nc.rgrc[0].top;
			nc.rgrc[0].right = nc.rgrc[0].right - cw * 2;
			nc.rgrc[0].bottom = nc.rgrc[0].bottom - ch * 2;
		}
		memmove((LPVOID)lParam, &nc, sizeof(NCCALCSIZE_PARAMS));
	}
}

INT Wnd_NcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT ret = 0;
	POINT pos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	RECT rect{};
	GetWindowRect(hWnd, &rect);
	INT cw = GetSystemMetrics(SM_CXSIZEFRAME), ch = GetSystemMetrics(SM_CYSIZEFRAME);

	ret = NL_HitTestSys({ rect.left, rect.top - 1, rect.right - rect.left, rect.bottom - rect.top }, pos);
	return ret;
}

INT Wnd_NcLDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return -1;
}

INT Wnd_NcLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return -1;
}

void Wnd_MouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT ret = 0;
	POINT pos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	pGUI.MouseMove(pos);
}

void Wnd_MouseLDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT ret = 0;
	POINT pos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	hDownObj = (NLGUIObj*)pGUI.HitTest(pos);
	pGUI.MouseLDown(pos);
}

void Wnd_MouseLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT ret = 0;
	POINT pos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	NLGUIObj* hitObj = hDownObj;
	NLGUIObj* parentObj = nullptr;
	if (hitObj)
	{
		parentObj = hitObj->GetParent();
	}
	pGUI.MouseLUp(pos);
}

void Wnd_MouseLDBClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT ret = 0;
	POINT pos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	pGUI.MouseLDBClick(pos);
}

void Wnd_MouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
	pGUI.MouseWheel(delta);
}

void Wnd_Command(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void Wnd_SysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == hMainWnd) 
	{
		LRESULT dwmResult = NULL;
		BOOL dwmHandled = FALSE;
		dwmHandled = DwmDefWindowProc(hWnd, message, wParam, lParam, &dwmResult);
		if (dwmHandled)
		{
			return dwmResult;
		}

		switch (message)
		{
		case WM_PAINT:
		{
			//PAINTSTRUCT ps;
			//HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			//EndPaint(hWnd, &ps);
			pGUI.Paint();
			ValidateRect(hMainWnd, NULL);
			return 0;
		}
		break;
		case WM_DESTROY:
			Wnd_Destroy();
			break;
		case WM_SYSCOMMAND:
			Wnd_SysCommand(hWnd, message, wParam, lParam);
			break;
		case WM_NCCALCSIZE:
		{
			Wnd_NcSize(hWnd, message, wParam, lParam);
			return 0;
		}
		break;
		case WM_NCHITTEST:
			return Wnd_NcHitTest(hWnd, message, wParam, lParam);
			break;
		case WM_NCLBUTTONDOWN:
			break;
		case WM_NCLBUTTONUP:		
			break;
		case WM_MOUSEMOVE:
			Wnd_MouseMove(hWnd, message, wParam, lParam);
			break;
		case WM_LBUTTONDBLCLK:
			Wnd_MouseLDBClick(hWnd, message, wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			Wnd_MouseLDown(hWnd, message, wParam, lParam);
			break;
		case WM_LBUTTONUP:
			Wnd_MouseLUp(hWnd, message, wParam, lParam);
			break;
		case WM_MOUSEWHEEL:
			Wnd_MouseWheel(hWnd, message, wParam, lParam);
			break;
		case WM_SIZE:
			Wnd_Size(hWnd, message, wParam, lParam);
			break;
		case WM_COMMAND:
			Wnd_Command(hWnd, message, wParam, lParam);
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
