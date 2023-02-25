#pragma once
#include "framework.h"

typedef LPVOID HGUIIMG;

typedef VOID(CALLBACK* OBJEVENT) (LPVOID, INT, LPVOID, LPVOID, LPVOID, LPVOID);

struct GUIRENDERINFO
{
	LPVOID pLocaleName;
	IDXGIDevice1* pDXGIDevice;
	IDXGIFactory2* pDXGIFactory;
	IDWriteFactory* pDWriteFactory;
	IWICImagingFactory* pWICFactory;
	ID2D1Factory1* pD2DFactory;
	ID2D1Device* pD2DDevice;
	ID2D1DeviceContext* pD2DDeviceContext;
	ID2D1GdiInteropRenderTarget* pGDIInterop;
	ID2D1ImageBrush* pImgBrush;
	ID2D1Bitmap* pBitmap;
};

class NLGUIBase
{
public:
	INT m_baseType;
};

class NLGUIWnd : public NLGUIBase
{
public:
	HWND m_hWnd;
	ID2D1DeviceContext* m_pD2DDeviceContext;
	ID2D1GdiInteropRenderTarget* m_pGDIInterop;
	IDXGISwapChain1* m_pDXGISwapChain;
	ID2D1Bitmap1* m_pTargetBitmap;
	ID2D1SolidColorBrush* m_pBrush;
	BOOL m_mouseLDown;
	INT m_objHit;
	LPCWSTR m_lpFont;
	std::vector<LPVOID> m_pObjs;

	void Bind(HWND hWnd);
	void Destroy();
	void Resize(INT width, INT height);
	void Paint();
	void MouseMove(POINT Pos);
	void MouseLDown(POINT Pos);
	void MouseLUp(POINT Pos);
	void MouseLDBClick(POINT Pos);
	void MouseWheel(INT Delta);
	LPVOID HitTest(POINT Pos);
	void _AddObj(LPVOID pObj);
	void _SetContextTarget(ID2D1Bitmap* target);
	void _ObjDraw();
	INT _ObjHitTest(POINT Pos);
	D2D1_COLOR_F _GetThemeColor(INT nType, FLOAT alpha = 1.0F);
};

class NLGUIObj : public NLGUIBase
{
public:
	NLGUIWnd* m_pWnd;
	INT m_objType;
	FLOAT m_left;
	FLOAT m_top;
	FLOAT m_right;
	FLOAT m_bottom;
	FLOAT m_w_left;
	FLOAT m_w_top;
	FLOAT m_w_right;
	FLOAT m_w_bottom;
	BOOL m_bVisible;
	BOOL m_w_bVisible;
	LPVOID m_pParent;
	BOOL m_bUse;
	BOOL m_bExtDrag;
	OBJEVENT m_Event;

	void _Create(NLGUIBase* pGUI, INT nType, BOOL bUse, BOOL bExtDrag, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _Event(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
	void _RunEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
	void Redraw();
	void SetPosA(INT left, INT top, INT right, INT bottom);
	void SetPos(INT left, INT top, INT width, INT height);
	void SetVisible(BOOL bVisible);
	BOOL GetVisible();
	void SetEvent(OBJEVENT Event);
	NLGUIObj* GetParent();
};

class NLGUIObj_Page : public NLGUIObj
{
public:
	std::vector<LPVOID> p_pObjs;

	void Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _AddObj(LPVOID pObj);
};

class NLGUIObj_Label : public NLGUIObj
{
public:
	LPCWSTR p_lpTitle;
	INT p_nSize;
	BOOL p_bBold;
	INT p_nTextAlign;
	INT p_nLineAlign;
	INT p_nColor;
	INT p_bWrap;

	void Create(NLGUIBase* pGUI, LPCWSTR lpTitle, INT nSize, BOOL bBold, INT nTextAlign, INT nLineAlign, INT nColor, BOOL bWrap, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetTitle(LPCWSTR lpTitle);
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_Con1 : public NLGUIObj
{
public:
	void Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_Btn1 : public NLGUIObj
{
public:
	LPCWSTR p_lpTitle;
	HGUIIMG p_pImage;
	FLOAT p_fRadius;
	INT p_nTextSize;
	FLOAT p_fImgSize;
	INT p_nStyle;
	INT p_nState;

	void Create(NLGUIBase* pGUI, LPCWSTR lpTitle, HGUIIMG hImg, FLOAT fRadius, INT nTextSize, FLOAT fImgSize, INT nStyle, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetTitle(LPCWSTR lpTitle);
	void _Draw(ID2D1DeviceContext* pContext);
	void _ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
};

class NLGUIObj_Btn2 : public NLGUIObj
{
public:
	HGUIIMG p_pImage;
	FLOAT p_fRadius;
	FLOAT p_fImgSize;
	INT p_nState;
	BOOL p_bSelect;

	void Create(NLGUIBase* pGUI, HGUIIMG hImg, FLOAT fRadius, FLOAT fImgSize, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetImg(HGUIIMG hImg);
	void SetSelect(BOOL bSelect);
	void _Draw(ID2D1DeviceContext* pContext);
	void _ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
};

class NLGUIObj_Img : public NLGUIObj
{
public:
	HGUIIMG p_pImage;
	FLOAT p_fRadius;
	INT p_nScaleMode;
	BOOL p_bShadow;
	BOOL p_bStroke;
	BOOL p_bMask;
	FLOAT p_fBlur;

	void Create(NLGUIBase* pGUI, HGUIIMG hImg, INT nScaleMode, FLOAT fRadius, BOOL bShadow, BOOL bStroke, BOOL bMask, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetImg(HGUIIMG hImg);
	void _UpdateImg();
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_Inst : public NLGUIObj
{
public:
	FLOAT p_fRadius;

	void Create(NLGUIBase* pGUI, FLOAT fRadius, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_Con2 : public NLGUIObj
{
public:
	void Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_Btn3 : public NLGUIObj
{
public:
	LPCWSTR p_lpTitle;
	FLOAT p_fRadius;
	INT p_nTextSize;
	INT p_nTextLeft;
	INT p_nState;
	BOOL p_bSelect;

	void Create(NLGUIBase* pGUI, LPCWSTR lpTitle, FLOAT fRadius, INT nTextSize, INT nTextLeft, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetSelect(BOOL bSelect);
	void SetTitle(LPCWSTR lpTitle);
	void _Draw(ID2D1DeviceContext* pContext);
	void _ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
};

class NLGUIObj_Con3 : public NLGUIObj
{
public:
	BOOL p_bBlur;
	BOOL p_bShadow;

	void Create(NLGUIBase* pGUI, BOOL bShadow, BOOL bBlur, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void _Draw(ID2D1DeviceContext* pContext);
};

class NLGUIObj_CoverBtn : public NLGUIObj
{
public:
	HGUIIMG p_pImage;
	HGUIIMG p_pBtnImage;
	INT p_nState;

	void Create(NLGUIBase* pGUI, HGUIIMG hImg, HGUIIMG hBtnImg, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SetImg(HGUIIMG hImg);
	void _Draw(ID2D1DeviceContext* pContext);
	void _ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
};

class NLGUIObj_Slider : public NLGUIObj
{
public:
	INT p_nState;
	BOOL p_bDown;
	FLOAT p_fSlider;
	LONG p_fMin;
	LONG p_fMax;
	LONG p_fPos;
	LONG p_fWheel;

	void Create(NLGUIBase* pGUI, BOOL bAutoSmall, BOOL bVertical, FLOAT left, FLOAT top, FLOAT width, FLOAT height);
	void SliderSetPos(LONG fPos);
	LONG GetPos();
	void SetInfo(LONG fMin, LONG fMax, LONG fWheel);
	void _Draw(ID2D1DeviceContext* pContext);
	void _ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4);
};

extern GUIRENDERINFO g_Ri;

#define NLGUI_BASETYPE_WND 0
#define NLGUI_BASETYPE_OBJ 1

#define NLGUI_OBJTYPE_PAGE 0
#define NLGUI_OBJTYPE_LABEL 1
#define NLGUI_OBJTYPE_CON1 2
#define NLGUI_OBJTYPE_BTN1 3
#define NLGUI_OBJTYPE_BTN2 4
#define NLGUI_OBJTYPE_IMG 5
#define NLGUI_OBJTYPE_INST 6
#define NLGUI_OBJTYPE_CON2 7
#define NLGUI_OBJTYPE_BTN3 8
#define NLGUI_OBJTYPE_CON3 9
#define NLGUI_OBJTYPE_COVERBTN 10
#define NLGUI_OBJTYPE_SLIDER 11
#define NLGUI_OBJTYPE_SELECT 12
#define NLGUI_OBJTYPE_THEMESELECT 13
#define NLGUI_OBJTYPE_LIST 14
#define NLGUI_OBJTYPE_STARTSCREEN 15
#define NLGUI_OBJTYPE_TAB 16
#define NLGUI_OBJTYPE_SYSBTN 17

#define NLGUI_THEMECOLOR_SYS 0
#define NLGUI_THEMECOLOR_BKG 1
#define NLGUI_THEMECOLOR_STROKE 2
#define NLGUI_THEMECOLOR_TEXT1 3
#define NLGUI_THEMECOLOR_TEXT2 4
#define NLGUI_THEMECOLOR_CON1_TITLEBAR 100
#define NLGUI_THEMECOLOR_BTN1_NORMAL 200
#define NLGUI_THEMECOLOR_BTN1_ENTER 201
#define NLGUI_THEMECOLOR_BTN1_DOWN 202
#define NLGUI_THEMECOLOR_BTN2_SELECT 300
#define NLGUI_THEMECOLOR_BTN2_ENTER 301
#define NLGUI_THEMECOLOR_BTN2_DOWN 302
#define NLGUI_THEMECOLOR_BTN3_SELECT 400
#define NLGUI_THEMECOLOR_BTN3_ENTER 401
#define NLGUI_THEMECOLOR_BTN3_DOWN 402
#define NLGUI_THEMECOLOR_COVERBTN_ENTER 500
#define NLGUI_THEMECOLOR_COVERBTN_DOWN 501
#define NLGUI_THEMECOLOR_SLIDER_BKG 600
#define NLGUI_THEMECOLOR_SLIDER_SLIDERBKG 601
#define NLGUI_THEMECOLOR_SELECT_BKG 700
#define NLGUI_THEMECOLOR_SELECT_SELECTBKG 701
#define NLGUI_THEMECOLOR_SELECT_SELECTCHECK 702
#define NLGUI_THEMECOLOR_THEMESELECT_ENTER 800
#define NLGUI_THEMECOLOR_THEMESELECT_DOWN 801
#define NLGUI_THEMECOLOR_THEMESELECT_BKG 802
#define NLGUI_THEMECOLOR_LIST_ENTERBKG 900
#define NLGUI_THEMECOLOR_LIST_DOWNBKG 901
#define NLGUI_THEMECOLOR_LIST_SELECTBKG 902
#define NLGUI_THEMECOLOR_LIST_SELECTDOWNBKG 903
#define NLGUI_THEMECOLOR_LIST_SLIDER 904
#define NLGUI_THEMECOLOR_STARTSCREEN 1000
#define NLGUI_THEMECOLOR_IMG_MASK 1100
#define NLGUI_THEMECOLOR_SYSBTN_DEFAULT_ENTER 1200
#define NLGUI_THEMECOLOR_SYSBTN_DEFAULT_DOWN 1201
#define NLGUI_THEMECOLOR_SYSBTN_CLOSE_ENTER 1202
#define NLGUI_THEMECOLOR_SYSBTN_CLOSE_DOWN 1203

#define NLGUI_EVENT_MOUSEENTER 1
#define NLGUI_EVENT_MOUSELEAVE 2
#define NLGUI_EVENT_MOUSELUP 3
#define NLGUI_EVENT_MOUSELDOWN 4
#define NLGUI_EVENT_MOUSERUP 5
#define NLGUI_EVENT_MOUSERDOWN 6
#define NLGUI_EVENT_MOUSEMOVE 7
#define NLGUI_EVENT_MOUSEWHEEL 8
#define NLGUI_EVENT_MOUSELDBLCLK 9
#define NLGUI_EVENT_MOUSERDBLCLK 10
#define NLGUI_EVENT_POS 11
#define NLGUI_EVENT_SLIDER_POS 100
#define NLGUI_EVENT_LIST_DBLCLK 200
#define NLGUI_EVENT_STARTSCREEN_LOAD 300

#define NLGUI_BTN_STATE_NORMAL 0
#define NLGUI_BTN_STATE_ENTER 1
#define NLGUI_BTN_STATE_DOWN 2

void NLGUI_Init();
void NLGUI_UnInit();

ID2D1ImageBrush* NLGUI_ResImgBrush(ID2D1Image* hImg, INT width, INT height);
ID2D1Bitmap* NLGUI_ResBitmap(INT width, INT height, FLOAT dpi);

ID2D1Bitmap* NLGUI_CvsCreateBitmap(INT width, INT height, FLOAT dpi);
void NLGUI_CvsCreateCvsBmp(INT width, INT height, ID2D1Bitmap** cvsBmp, FLOAT dpi);
void NLGUI_CvsDrawCvsBmp(ID2D1DeviceContext* pContext, ID2D1Bitmap* cvsBmp, INT left, INT top, FLOAT dpi, D2D1::Matrix3x2F* matrix);
void _NLGUI_CvsCreateText(ID2D1DeviceContext* pContext, LPCWSTR lpText, LPCWSTR lpFont, INT nSize, BOOL bBold, D2D1_RECT_F rect, DWRITE_PARAGRAPH_ALIGNMENT parAlign, DWRITE_TEXT_ALIGNMENT textAlign, BOOL bWrap, IDWriteTextFormat** pTextFormat, IDWriteTextLayout** pTextLayout);
void NLGUI_CvsDrawText(ID2D1DeviceContext* pContext, LPCWSTR lpText, LPCWSTR lpFont, INT nSize, BOOL bBold, ID2D1Brush* pBrush, D2D1_RECT_F rect, DWRITE_PARAGRAPH_ALIGNMENT parAlign, DWRITE_TEXT_ALIGNMENT textAlign, BOOL bWrap);
void NLGUI_CvsDrawImage(ID2D1DeviceContext* pContext, ID2D1Image* pImage, D2D1_SIZE_F imgSize, INT scaleMode, FLOAT radius, FLOAT alpha, D2D1_RECT_F rect);
void NLGUI_CvsDrawBitmap(ID2D1DeviceContext* pContext, HGUIIMG hImg, INT scaleMode, FLOAT radius, FLOAT alpha, D2D1_RECT_F rect);
void NLGUI_CvsDrawIconBitmap(ID2D1DeviceContext* pContext, HGUIIMG hImg, FLOAT alpha, D2D1_RECT_F rect);

void NLGUI_ImgFree(HGUIIMG hImg);
HGUIIMG NLGUI_ImgLoadFile(LPCWSTR filePath);
