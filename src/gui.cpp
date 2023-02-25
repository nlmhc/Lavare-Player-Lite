#include "gui.h"

GUIRENDERINFO g_Ri;

void NLGUI_Init()
{
	HRESULT hr = NULL;
	CoInitialize(NULL);
	ID3D11Device* pD3DDevice = nullptr;
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, 0, D3D11_SDK_VERSION, &pD3DDevice, NULL, NULL);
	if (SUCCEEDED(hr))
	{
		hr = pD3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (LPVOID*)&g_Ri.pDXGIDevice);
		IDXGIAdapter* pDXGIAdapter = nullptr;
		g_Ri.pDXGIDevice->GetAdapter(&pDXGIAdapter);
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (LPVOID*)&g_Ri.pDXGIFactory);
		if (SUCCEEDED(hr))
		{
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &g_Ri.pD2DFactory);
			if (SUCCEEDED(hr))
			{
				hr = g_Ri.pD2DFactory->CreateDevice(g_Ri.pDXGIDevice, &g_Ri.pD2DDevice);
				if (SUCCEEDED(hr))
				{
					g_Ri.pImgBrush = nullptr;
					g_Ri.pBitmap = nullptr;
					g_Ri.pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_Ri.pD2DDeviceContext);
					hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&g_Ri.pWICFactory);
					hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_Ri.pDWriteFactory);
				}
			}
		}
		pD3DDevice->Release();
	}
}

void NLGUI_UnInit()
{
	CoUninitialize();
}

ID2D1ImageBrush* NLGUI_ResImgBrush(ID2D1Image* hImg, INT width, INT height)
{
	D2D1_RECT_F rect = D2D1::RectF(0, 0, width, height);
	if (!g_Ri.pImgBrush)
	{
		g_Ri.pD2DDeviceContext->CreateImageBrush(hImg, D2D1::ImageBrushProperties(rect), &g_Ri.pImgBrush);
	}
	else 
	{
		g_Ri.pImgBrush->SetSourceRectangle(&rect);
		g_Ri.pImgBrush->SetTransform(D2D1::Matrix3x2F::Translation(0, 0));
		g_Ri.pImgBrush->SetInterpolationMode(D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		g_Ri.pImgBrush->SetOpacity(1);
		g_Ri.pImgBrush->SetImage(hImg);
	}
	return g_Ri.pImgBrush;
}

ID2D1Bitmap* NLGUI_ResBitmap(INT width, INT height, FLOAT dpi)
{
	NLGUI_CvsCreateCvsBmp(width, height, &g_Ri.pBitmap, dpi);
	return g_Ri.pBitmap;
}

ID2D1Bitmap* NLGUI_CvsCreateBitmap(INT width, INT height, FLOAT dpi)
{
	D2D1_SIZE_U size;
	size.width = round(width * dpi);
	size.height = round(height * dpi);

	D2D1_BITMAP_PROPERTIES1 pro = {};
	pro.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	pro.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	pro.dpiX = 96;
	pro.dpiY = 96;

	pro.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE;

	ID2D1Bitmap1* pBitmap = nullptr;
	g_Ri.pD2DDeviceContext->CreateBitmap(size, NULL, 0, pro, &pBitmap);
	return pBitmap;
}

void NLGUI_CvsCreateCvsBmp(INT width, INT height, ID2D1Bitmap** cvsBmp, FLOAT dpi)
{
	D2D1_SIZE_F cvsSize;
	BOOL cvsResize;
	if (*cvsBmp)
	{
		cvsSize = (*cvsBmp)->GetSize();
		D2D1_SIZE_F size = D2D1::SizeF((INT)round(width * dpi), (INT)round(height * dpi));
		cvsResize = cvsSize.width != size.width || cvsSize.height != size.height;
	}
	else
	{
		cvsResize = TRUE;
	}
	if (cvsResize)
	{
		if (*cvsBmp)
		{
			(*cvsBmp)->Release();
		}
		*cvsBmp = NLGUI_CvsCreateBitmap(width < 1 ? 1 : width, height < 1 ? 1 : height, dpi);
	}
}

void NLGUI_CvsDrawCvsBmp(ID2D1DeviceContext* pContext, ID2D1Bitmap* cvsBmp, INT left, INT top, FLOAT dpi, D2D1::Matrix3x2F* matrix)
{
	D2D1_SIZE_F cvsSize = cvsBmp->GetSize();
	D2D1_POINT_2F cvsPos = D2D1::Point2F((INT)(left * dpi), (INT)(top * dpi));
	pContext->SetDpi(96, 96);
	FLOAT cvsWidth = pContext->GetSize().width;
	D2D1_MATRIX_3X2_F originMatrix;
	pContext->GetTransform(&originMatrix);
	if (matrix)
	{
		pContext->SetTransform(*matrix * D2D1::Matrix3x2F::Scale(1, 1));
	}
	else
	{
		pContext->SetTransform(D2D1::Matrix3x2F::Scale(1, 1));
	}
	pContext->DrawBitmap(cvsBmp,
		D2D1::RectF(cvsPos.x, cvsPos.y, cvsPos.x + cvsSize.width, cvsPos.y + cvsSize.height)
	);
	pContext->SetTransform(&originMatrix);
	pContext->SetDpi(96 * dpi, 96 * dpi);
}

void _NLGUI_CvsCreateText(ID2D1DeviceContext* pContext, LPCWSTR lpText, LPCWSTR lpFont, INT nSize, BOOL bBold, D2D1_RECT_F rect, DWRITE_PARAGRAPH_ALIGNMENT parAlign, DWRITE_TEXT_ALIGNMENT textAlign, BOOL bWrap, IDWriteTextFormat** pTextFormat, IDWriteTextLayout** pTextLayout)
{
	FLOAT cvsWidth = pContext->GetSize().width;
	D2D1_RECT_F prect = rect;
	g_Ri.pDWriteFactory->CreateTextFormat(lpFont, NULL, bBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, nSize, L"zh-cn", pTextFormat);
	if (*pTextFormat)
	{
		(*pTextFormat)->SetParagraphAlignment(parAlign);
		(*pTextFormat)->SetTextAlignment(textAlign);
		(*pTextFormat)->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
		(*pTextFormat)->SetWordWrapping(bWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
		g_Ri.pDWriteFactory->CreateTextLayout(lpText, lstrlenW(lpText), (*pTextFormat), prect.right - prect.left, prect.bottom - prect.top, pTextLayout);
	}
}

void NLGUI_CvsDrawText(ID2D1DeviceContext* pContext, LPCWSTR lpText, LPCWSTR lpFont, INT nSize, BOOL bBold, ID2D1Brush* pBrush, D2D1_RECT_F rect, DWRITE_PARAGRAPH_ALIGNMENT parAlign, DWRITE_TEXT_ALIGNMENT textAlign, BOOL bWrap)
{
	FLOAT cvsWidth = pContext->GetSize().width;
	D2D1_RECT_F prect = rect;
	IDWriteTextFormat* pTextFormat = nullptr;
	IDWriteTextLayout* pTextLayout = nullptr;
	_NLGUI_CvsCreateText(pContext, lpText, lpFont, nSize, bBold, rect, parAlign, textAlign, bWrap, &pTextFormat, &pTextLayout);
	if (pTextFormat)
	{
		if (pTextLayout)
		{
			pContext->DrawTextLayout(D2D1::Point2F(prect.left, prect.top), pTextLayout, pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
			pTextLayout->Release();
		}
		pTextFormat->Release();
	}
}

void NLGUI_CvsDrawImage(ID2D1DeviceContext* pContext, ID2D1Image* pImage, D2D1_SIZE_F imgSize, INT scaleMode, FLOAT radius, FLOAT alpha, D2D1_RECT_F rect)
{
	FLOAT width = rect.right - rect.left, height = rect.bottom - rect.top;
	ID2D1ImageBrush* pImgBrush = NLGUI_ResImgBrush(pImage, imgSize.width, imgSize.height);
	if (pImgBrush)
	{
		D2D1::Matrix3x2F mx;
		if (scaleMode == 0)
		{
			mx = D2D1::Matrix3x2F::Scale(width / imgSize.width, height / imgSize.height);
			mx = mx * D2D1::Matrix3x2F::Translation(rect.left, rect.top);
		}
		else if (scaleMode == 1)
		{
			mx = D2D1::Matrix3x2F::Scale(width / imgSize.width, height / imgSize.height);
			mx = mx * D2D1::Matrix3x2F::Translation(rect.left, rect.top);
		}
		pImgBrush->SetTransform(mx);
		if (radius > 0)
		{
			pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), radius, radius), pImgBrush);
		}
		else 
		{
			pContext->FillRectangle(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom), pImgBrush);
		}
	}
}

void NLGUI_CvsDrawBitmap(ID2D1DeviceContext* pContext, HGUIIMG hImg, INT scaleMode, FLOAT radius, FLOAT alpha, D2D1_RECT_F rect)
{
	D2D1_SIZE_F imgSize = ((ID2D1Bitmap*)hImg)->GetSize();
	NLGUI_CvsDrawImage(pContext, (ID2D1Bitmap*)hImg, imgSize, scaleMode, radius, alpha, rect);
}

void NLGUI_CvsDrawIconBitmap(ID2D1DeviceContext* pContext, HGUIIMG hImg, FLOAT alpha, D2D1_RECT_F rect)
{
	pContext->DrawBitmap((ID2D1Bitmap*)hImg, rect, alpha);
}

HGUIIMG NLGUI_ImgLoadFile(LPCWSTR filePath)
{
	HRESULT hr = NULL;
	ID2D1Bitmap* pBitmap = nullptr;
	IWICBitmapDecoder* pDecoder = nullptr;
	GUID vendor = GUID_VendorMicrosoft;
	hr = g_Ri.pWICFactory->CreateDecoderFromFilename(filePath, &vendor, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
	if (SUCCEEDED(hr))
	{
		IWICBitmapFrameDecode* pFrameDecode = nullptr;
		hr = pDecoder->GetFrame(0, &pFrameDecode);
		if (SUCCEEDED(hr))
		{
			IWICFormatConverter* FormatConverter = nullptr;
			hr = g_Ri.pWICFactory->CreateFormatConverter(&FormatConverter);
			if (SUCCEEDED(hr))
			{
				hr = FormatConverter->Initialize(pFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
				if (SUCCEEDED(hr))
				{
					g_Ri.pD2DDeviceContext->CreateBitmapFromWicBitmap(FormatConverter, &pBitmap);
				}
				FormatConverter->Release();
			}
			pFrameDecode->Release();
		}
		pDecoder->Release();
	}
	return pBitmap;
}

void NLGUI_ImgFree(HGUIIMG hImg)
{
	ID2D1Bitmap* pBitmap = (ID2D1Bitmap*)hImg;
	if (pBitmap)
	{
		pBitmap->Release();
	}
}

void NLGUIWnd::Bind(HWND hWnd) 
{
	m_baseType = NLGUI_BASETYPE_WND;
	m_objHit = -1;
	m_lpFont = StrDupW(L"Segoe UI");
	m_hWnd = hWnd;

	g_Ri.pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pD2DDeviceContext);
	m_pD2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &m_pBrush);

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = 1;
	desc.Height = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Scaling = DXGI_SCALING_STRETCH;
	g_Ri.pDXGIFactory->CreateSwapChainForHwnd(g_Ri.pDXGIDevice, hWnd, &desc, NULL, NULL, &m_pDXGISwapChain);

	Resize(0, 0);
}

void NLGUIWnd::Destroy()
{

}

void NLGUIWnd::Resize(INT width, INT height)
{
	HRESULT hr = NULL;

	IDXGISurface* pDXGISurface = nullptr;
	D2D1_BITMAP_PROPERTIES1 pro{};
	pro.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	pro.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	pro.dpiX = 96;
	pro.dpiY = 96;
	pro.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	m_pD2DDeviceContext->SetTarget(NULL);
	if (m_pTargetBitmap)
	{
		m_pTargetBitmap->Release();
	}
	hr = m_pDXGISwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	m_pDXGISwapChain->GetBuffer(0, __uuidof(IDXGISurface), (LPVOID*)&pDXGISurface);
	m_pD2DDeviceContext->CreateBitmapFromDxgiSurface(pDXGISurface, pro, &m_pTargetBitmap);
	m_pD2DDeviceContext->SetTarget(m_pTargetBitmap);
	pDXGISurface->Release();
}

void NLGUIWnd::Paint()
{
	ID2D1DeviceContext* pContext = m_pD2DDeviceContext;

	pContext->BeginDraw();
	FLOAT a = pContext->GetSize().width;
	pContext->SetTarget(m_pTargetBitmap);
	pContext->Clear();
	_ObjDraw();
	pContext->EndDraw();
	m_pDXGISwapChain->Present(0, 0);
}

void NLGUIWnd::_SetContextTarget(ID2D1Bitmap* target)
{
	ID2D1DeviceContext* pContext = m_pD2DDeviceContext;
	if (target)
	{
		pContext->SetTarget(target);
	}
	else
	{
		pContext->SetTarget(m_pTargetBitmap);
	}
}

void NLGUIWnd::MouseMove(POINT Pos)
{
	NLGUIObj* pObj = nullptr;
	INT nObj = _ObjHitTest(Pos);
	if (!m_mouseLDown) 
	{
		if (nObj != m_objHit)
		{
			if (m_objHit != -1)
			{
				pObj = (NLGUIObj*)m_pObjs[m_objHit];
				pObj->_Event(NLGUI_EVENT_MOUSELEAVE, 0, 0, 0, 0);
			}
			m_objHit = nObj;
		}
		else
		{
			if (m_objHit != -1)
			{
				pObj = (NLGUIObj*)m_pObjs[m_objHit];
				pObj->_Event(NLGUI_EVENT_MOUSEENTER, 0, 0, 0, 0);
			}
			m_objHit = nObj;
		}
	}
	if (m_objHit != -1) 
	{
		pObj = (NLGUIObj*)m_pObjs[m_objHit];
		POINT ppos = { Pos.x - pObj->m_w_left, Pos.y - pObj->m_w_top };
		pObj->_Event(NLGUI_EVENT_MOUSEMOVE, &ppos.x, &ppos.y, 0, 0);
	}
	//output(nObj, Pos.x, Pos.y);
}

void NLGUIWnd::MouseLDown(POINT Pos)
{
	NLGUIObj* pObj = nullptr;
	INT nObj = _ObjHitTest(Pos);
	m_mouseLDown = TRUE;
	if (nObj != -1)
	{
		if (nObj != -1)
		{
			pObj = (NLGUIObj*)m_pObjs[nObj];
			POINT ppos = { Pos.x - pObj->m_w_left, Pos.y - pObj->m_w_top };
			pObj->_Event(NLGUI_EVENT_MOUSELDOWN, &ppos.x, &ppos.y, 0, 0);
		}
	}
	//output(2, nObj, Pos.x, Pos.y);
}

void NLGUIWnd::MouseLUp(POINT Pos)
{
	NLGUIObj* pObj = nullptr;
	INT nObj = _ObjHitTest(Pos);
	if (nObj != -1)
	{
		pObj = (NLGUIObj*)m_pObjs[nObj];
		BOOL bHit = pObj->m_bExtDrag;
		POINT ppos = { Pos.x - pObj->m_w_left, Pos.y - pObj->m_w_top };
		pObj->_Event(NLGUI_EVENT_MOUSELUP, &ppos.x, &ppos.y, 0, 0);
	}
	m_mouseLDown = FALSE;
	MouseMove(Pos);
	//output(3, nObj, Pos.x, Pos.y);
}

void NLGUIWnd::MouseLDBClick(POINT Pos)
{
	NLGUIObj* pObj = nullptr;
	INT nObj = _ObjHitTest(Pos);
	m_mouseLDown = TRUE;
	if (nObj == m_objHit)
	{
		if (m_objHit != -1)
		{
			pObj = (NLGUIObj*)m_pObjs[m_objHit];
			POINT ppos = { Pos.x - pObj->m_w_left, Pos.y - pObj->m_w_top };
			pObj->_Event(NLGUI_EVENT_MOUSELDBLCLK, &ppos.x, &ppos.y, 0, 0);
		}
	}
	//output(2, nObj, Pos.x, Pos.y);
}

void NLGUIWnd::MouseWheel(INT Delta)
{
	NLGUIObj* pObj = nullptr;
	if (m_objHit != -1)
	{
		pObj = (NLGUIObj*)m_pObjs[m_objHit];
		pObj->_Event(NLGUI_EVENT_MOUSEWHEEL, &Delta, 0, 0, 0);
	}
	//output(5, Delta);
}

D2D1_COLOR_F NLGUIWnd::_GetThemeColor(INT nType, FLOAT alpha)
{
	D2D1_COLOR_F ret;
	switch (nType)
	{
	case NLGUI_THEMECOLOR_SYS:
		ret = D2D1::ColorF(0, 0.63, 0.9, 1);
		break;
	case NLGUI_THEMECOLOR_BKG:
		ret = D2D1::ColorF(1, 1, 1, 0.76);
		break;
	case NLGUI_THEMECOLOR_STROKE:
		ret = D2D1::ColorF(0, 0, 0, 0.25);
		break;
	case NLGUI_THEMECOLOR_TEXT1:
		ret = D2D1::ColorF(0, 0, 0, 1);
		break;
	case NLGUI_THEMECOLOR_TEXT2:
		ret = D2D1::ColorF(0.4, 0.4, 0.4, 1);
		break;
	case NLGUI_THEMECOLOR_CON1_TITLEBAR:
		ret = D2D1::ColorF(0.94, 0.94, 0.94, 1);
		break;
	case NLGUI_THEMECOLOR_BTN1_NORMAL:
		ret = D2D1::ColorF(1, 1, 1, 0.8);
		break;
	case NLGUI_THEMECOLOR_BTN1_ENTER:
		ret = D2D1::ColorF(0.9, 0.9, 0.9, 0.8);
		break;
	case NLGUI_THEMECOLOR_BTN1_DOWN:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.8);
		break;
	case NLGUI_THEMECOLOR_BTN2_SELECT:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.6);
		break;
	case NLGUI_THEMECOLOR_BTN2_ENTER:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.4);
		break;
	case NLGUI_THEMECOLOR_BTN2_DOWN:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.8);
		break;
	case NLGUI_THEMECOLOR_BTN3_SELECT:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.5);
		break;
	case NLGUI_THEMECOLOR_BTN3_ENTER:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.5);
		break;
	case NLGUI_THEMECOLOR_BTN3_DOWN:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.9);
		break;
	case NLGUI_THEMECOLOR_COVERBTN_ENTER:
		ret = D2D1::ColorF(1, 1, 1, 0.3);
		break;
	case NLGUI_THEMECOLOR_COVERBTN_DOWN:
		ret = D2D1::ColorF(0.8, 0.8, 0.8, 0.3);
		break;
	case NLGUI_THEMECOLOR_SLIDER_BKG:
		ret =  D2D1::ColorF(0.5, 0.5, 0.5, 0.5);
		break;
	case NLGUI_THEMECOLOR_SLIDER_SLIDERBKG:
		ret = D2D1::ColorF(1, 1, 1, 1);
		break;
	case NLGUI_THEMECOLOR_IMG_MASK:
		ret = D2D1::ColorF(1, 1, 1, 0.75);
		break;
	}
	ret.a = ret.a * alpha;
	return ret;
}

void NLGUIWnd::_ObjDraw()
{
	ID2D1DeviceContext* pContext = m_pD2DDeviceContext;
	NLGUIObj* pObj = nullptr;
	for (INT i = 0; i < m_pObjs.size(); i++)
	{
		pObj = (NLGUIObj*)m_pObjs[i];
		if (pObj->m_w_bVisible)
		{
			switch (pObj->m_objType)
			{
			case NLGUI_OBJTYPE_LABEL:
				((NLGUIObj_Label*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_CON1:
				((NLGUIObj_Con1*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_BTN1:
				((NLGUIObj_Btn1*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_BTN2:
				((NLGUIObj_Btn2*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_IMG:
				((NLGUIObj_Img*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_INST:
				((NLGUIObj_Inst*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_CON2:
				((NLGUIObj_Con2*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_BTN3:
				((NLGUIObj_Btn3*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_CON3:
				((NLGUIObj_Con3*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_COVERBTN:
				((NLGUIObj_CoverBtn*)pObj)->_Draw(pContext);
				break;
			case NLGUI_OBJTYPE_SLIDER:
				((NLGUIObj_Slider*)pObj)->_Draw(pContext);
				break;
			}
		}
	}
}

void NLGUIObj::Redraw()
{
	m_pWnd->Paint();
}

INT NLGUIWnd::_ObjHitTest(POINT Pos)
{
	INT ret = -1;
	for (INT i = 0; i < m_pObjs.size(); i++)
	{
		NLGUIObj* pObj = (NLGUIObj*)m_pObjs[i];
		if (pObj->m_w_bVisible)
		{
			if (pObj->m_bUse)
			{
				RECT rect = { pObj->m_w_left, pObj->m_w_top, pObj->m_w_right, pObj->m_w_bottom };
				POINT ppos = { Pos.x, Pos.y };
				if (PtInRect(&rect, ppos))
				{
					ret = i;
				}
			}
		}
	}
	return ret;
}

LPVOID NLGUIWnd::HitTest(POINT Pos)
{
	INT nObj = _ObjHitTest(Pos);
	if (nObj != -1) 
	{
		return m_pObjs[nObj];
	}
	return 0;
}

NLGUIObj* NLGUIObj::GetParent()
{
	return (NLGUIObj*)m_pParent;
}

void NLGUIObj::_Event(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (m_objType)
	{
	case NLGUI_OBJTYPE_BTN1:
		((NLGUIObj_Btn1*)this)->_ObjEvent(nEvent, c1, c2, c3, c4);
		break;
	case NLGUI_OBJTYPE_BTN2:
		((NLGUIObj_Btn2*)this)->_ObjEvent(nEvent, c1, c2, c3, c4);
		break;
	case NLGUI_OBJTYPE_BTN3:
		((NLGUIObj_Btn3*)this)->_ObjEvent(nEvent, c1, c2, c3, c4);
		break;
	case NLGUI_OBJTYPE_COVERBTN:
		((NLGUIObj_CoverBtn*)this)->_ObjEvent(nEvent, c1, c2, c3, c4);
		break;
	case NLGUI_OBJTYPE_SLIDER:
		((NLGUIObj_Slider*)this)->_ObjEvent(nEvent, c1, c2, c3, c4);
		break;
	}
}

void NLGUIObj::_RunEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	if (m_Event)
	{
		m_Event(this, nEvent, c1, c2, c3, c4);
	}
}

void NLGUIWnd::_AddObj(LPVOID pObj)
{
	std::vector<LPVOID> pObjs(m_pObjs);
	m_pObjs.~vector();
	pObjs.push_back(pObj);
	m_pObjs = pObjs;
}

void NLGUIObj::_Create(NLGUIBase* pGUI, INT nType, BOOL bUse, BOOL bExtDrag, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	NLGUIWnd* pWnd = nullptr;
	NLGUIObj_Page* pPageObj = nullptr;
	if (pGUI->m_baseType == NLGUI_BASETYPE_OBJ)
	{
		pPageObj = (NLGUIObj_Page*)pGUI;
		pWnd = pPageObj->m_pWnd;
		m_pParent = pPageObj;
	}
	else 
	{
		pWnd = (NLGUIWnd*)pGUI;
		m_pParent = nullptr;
	}
	m_baseType = NLGUI_BASETYPE_OBJ;
	m_objType = nType;
	m_bUse = bUse;
	m_bExtDrag = bExtDrag;
	m_pWnd = pWnd;
	if (pPageObj) 
	{
		pPageObj->_AddObj(this);
	}
	SetPos(left, top, width, height);
	SetVisible(TRUE);
	pWnd->_AddObj(this);
}

void NLGUIObj::SetPosA(INT left, INT top, INT right, INT bottom)
{
	m_left = left;
	m_top = top;
	m_right = right;
	m_bottom = bottom;
	m_w_left = m_left;
	m_w_top = m_top;
	m_w_right = m_right;
	m_w_bottom = m_bottom;
	if (m_pParent)
	{
		NLGUIObj_Page* pPageObj = (NLGUIObj_Page*)m_pParent;
		//while(pPageObj)
		{
			m_w_left += pPageObj->m_w_left;
			m_w_top += pPageObj->m_w_top;
			m_w_right += pPageObj->m_w_left;
			m_w_bottom += pPageObj->m_w_top;
			//pPageObj = (GUIOBJINFO_PAGE*)pPageObj->base.pParent;
		}
	}
	if (m_objType == NLGUI_OBJTYPE_PAGE)
	{
		NLGUIObj_Page* pPageObj = (NLGUIObj_Page*)this;
		for (INT i = 0; i < pPageObj->p_pObjs.size(); i++)
		{
			NLGUIObj* pPObj = (NLGUIObj*)pPageObj->p_pObjs[i];
			pPObj->m_w_left = pPObj->m_left + m_w_left;
			pPObj->m_w_top = pPObj->m_top + m_w_top;
			pPObj->m_w_right = pPObj->m_right + m_w_left;
			pPObj->m_w_bottom = pPObj->m_bottom + m_w_top;
			if (pPObj->m_objType == NLGUI_OBJTYPE_PAGE)
			{
				pPObj->SetPosA(pPObj->m_left, pPObj->m_top, pPObj->m_right, pPObj->m_bottom);
			}
		}
	}
	_Event(NLGUI_EVENT_POS, 0, 0, 0, 0);
}

void NLGUIObj::SetPos(INT left, INT top, INT width, INT height)
{
	SetPosA(left, top, left + width, top + height);
}

void NLGUIObj::SetVisible(BOOL bVisible)
{
	m_bVisible = bVisible;
	m_w_bVisible = bVisible;
	if (m_pParent)
	{
		NLGUIObj_Page* pPageObj = (NLGUIObj_Page*)m_pParent;
		m_w_bVisible = m_bVisible ? pPageObj->m_w_bVisible : FALSE;
	}
	if (m_objType == NLGUI_OBJTYPE_PAGE)
	{
		NLGUIObj_Page* pPageObj = (NLGUIObj_Page*)this;
		for (INT i = 0; i < pPageObj->p_pObjs.size(); i++)
		{
			NLGUIObj* pPObj = (NLGUIObj*)pPageObj->p_pObjs[i];
			pPObj->m_w_bVisible = pPObj->m_bVisible ? m_w_bVisible : FALSE;
			if (pPObj->m_objType == NLGUI_OBJTYPE_PAGE)
			{
				pPObj->SetVisible(pPObj->m_bVisible);
			}
		}
	}
}

BOOL NLGUIObj::GetVisible()
{
	return m_bVisible;
}

void NLGUIObj::SetEvent(OBJEVENT Event)
{
	m_Event = Event;
}

void NLGUIObj_Page::Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	_Create(pGUI, NLGUI_OBJTYPE_PAGE, FALSE, FALSE, left, top, width, height);
}

void NLGUIObj_Page::_AddObj(LPVOID pObj)
{
	std::vector<LPVOID> pObjs(p_pObjs);
	p_pObjs.~vector();
	pObjs.push_back(pObj);
	p_pObjs = pObjs;
}

void NLGUIObj_Label::Create(NLGUIBase* pGUI, LPCWSTR lpTitle, INT nSize, BOOL bBold, INT nTextAlign, INT nLineAlign, INT nColor, BOOL bWrap, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_lpTitle = StrDupW(lpTitle);
	p_nSize = nSize;
	p_bBold = bBold;
	p_nTextAlign = nTextAlign;
	p_nLineAlign = nLineAlign;
	p_nColor = nColor;
	p_bWrap = bWrap;
	_Create(pGUI, NLGUI_OBJTYPE_LABEL, FALSE, FALSE, left, top, width, height);
}

void NLGUIObj_Label::SetTitle(LPCWSTR lpTitle)
{
	NL_MemFree((LPVOID)p_lpTitle);
	p_lpTitle = StrDupW(lpTitle);
	Redraw();
}

void NLGUIObj_Label::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(p_nColor, 1));

	NLGUI_CvsDrawText(
		pContext,
		p_lpTitle,
		m_pWnd->m_lpFont,
		p_nSize,
		p_bBold,
		m_pWnd->m_pBrush,
		D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom),
		(DWRITE_PARAGRAPH_ALIGNMENT)p_nLineAlign,
		(DWRITE_TEXT_ALIGNMENT)p_nTextAlign,
		p_bWrap);
}

void NLGUIObj_Con1::Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	_Create(pGUI, NLGUI_OBJTYPE_CON1, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Con1::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE));
	pContext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 7, 7), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BKG));
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 8, 8), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_CON1_TITLEBAR));
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_top + 40), 8, 8), m_pWnd->m_pBrush);
	pContext->FillRectangle(D2D1::RectF(m_w_left, m_w_top + 10, m_w_right, m_w_top + 40), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(D2D1::ColorF(0, 0.2));
}

void NLGUIObj_Btn1::Create(NLGUIBase* pGUI, LPCWSTR lpTitle, HGUIIMG hImg, FLOAT fRadius, INT nTextSize, FLOAT fImgSize, INT nStyle, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_lpTitle = StrDupW(lpTitle);
	p_pImage = hImg;
	p_fRadius = fRadius;
	p_nTextSize = nTextSize;
	p_fImgSize = fImgSize;
	p_nStyle = nStyle;
	_Create(pGUI, NLGUI_OBJTYPE_BTN1, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Btn1::SetTitle(LPCWSTR lpTitle)
{
	NL_MemFree((LPVOID)p_lpTitle);
	p_lpTitle = StrDupW(lpTitle);
	Redraw();
}

void NLGUIObj_Btn1::_Draw(ID2D1DeviceContext* pContext)
{
	FLOAT width = m_w_right - m_w_left, height = m_w_bottom - m_w_top;
	//FLOAT imgSize = 21;
	FLOAT imgSize = p_fImgSize;
	FLOAT imgPos = height / 2 - imgSize / 2;
	D2D1_COLOR_F bkg = D2D1::ColorF(0, 0);
	if (p_nStyle == 0) 
	{
		m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE, 1));
		pContext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush, 1);
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN1_NORMAL, 1);
	}
	switch (p_nState)
	{
	case NLGUI_BTN_STATE_ENTER:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN1_ENTER, 1);
		break;
	case NLGUI_BTN_STATE_DOWN:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN1_DOWN, 1);
		break;
	}
	m_pWnd->m_pBrush->SetColor(bkg);
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_TEXT1, 1));
	NLGUI_CvsDrawText(pContext, p_lpTitle, m_pWnd->m_lpFont, p_nTextSize, FALSE, m_pWnd->m_pBrush, 
		D2D1::RectF(m_w_left + imgPos + imgSize + 10, m_w_top, m_w_right, m_w_bottom),
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER, DWRITE_TEXT_ALIGNMENT_LEADING, FALSE);
	if (p_pImage) 
	{
		NLGUI_CvsDrawIconBitmap(pContext, (ID2D1Bitmap*)p_pImage, 1, D2D1::RectF(m_w_left + imgPos, m_w_top + imgPos, m_w_left + imgPos + imgSize, m_w_top + height / 2 + imgSize / 2));
	}
}

void NLGUIObj_Btn1::_ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSEENTER:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELEAVE:
		p_nState = NLGUI_BTN_STATE_NORMAL;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELDOWN:
		p_nState = NLGUI_BTN_STATE_DOWN;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELUP:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	}
	_RunEvent(nEvent, c1, c2, c3, c4);
}

void NLGUIObj_Btn2::Create(NLGUIBase* pGUI, HGUIIMG hImg, FLOAT fRadius, FLOAT fImgSize, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_pImage = hImg;
	p_fImgSize = fImgSize;
	p_fRadius = fRadius;
	_Create(pGUI, NLGUI_OBJTYPE_BTN2, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Btn2::SetImg(HGUIIMG hImg)
{
	p_pImage = hImg;
	Redraw();
}

void NLGUIObj_Btn2::SetSelect(BOOL bSelect)
{
	p_bSelect = bSelect;
	Redraw();
}

void NLGUIObj_Btn2::_Draw(ID2D1DeviceContext* pContext)
{
	INT width = m_w_right - m_w_left, height = m_w_bottom - m_w_top;
	D2D1_COLOR_F bkg = p_bSelect ? m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN2_SELECT, 1) : D2D1::ColorF(0, 0, 0, 0);
	switch (p_nState)
	{
	case NLGUI_BTN_STATE_ENTER:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN2_ENTER, 1);
		break;
	case NLGUI_BTN_STATE_DOWN:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN2_DOWN, 1);
		break;
	}
	m_pWnd->m_pBrush->SetColor(bkg);
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
	if (p_pImage)
	{
		NLGUI_CvsDrawIconBitmap(pContext, (ID2D1Bitmap*)p_pImage, 1, 
			D2D1::RectF(m_w_left + width / 2 - p_fImgSize / 2, m_w_top + height / 2 - p_fImgSize / 2, m_w_left + width / 2 + p_fImgSize / 2, m_w_top + height / 2 + p_fImgSize / 2));
	}
}

void NLGUIObj_Btn2::_ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSEENTER:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELEAVE:
		p_nState = NLGUI_BTN_STATE_NORMAL;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELDOWN:
		p_nState = NLGUI_BTN_STATE_DOWN;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELUP:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	}
	_RunEvent(nEvent, c1, c2, c3, c4);
}

void NLGUIObj_Img::Create(NLGUIBase* pGUI, HGUIIMG hImg, INT nScaleMode, FLOAT fRadius, BOOL bShadow, BOOL bStroke, BOOL bMask, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_pImage = hImg;
	p_nScaleMode = nScaleMode;
	p_fRadius = fRadius;
	p_bShadow = bShadow;
	p_bStroke = bStroke;
	p_bMask = bMask;
	p_fBlur = 0;
	_Create(pGUI, NLGUI_OBJTYPE_IMG, FALSE, FALSE, left, top, width, height);
}

void NLGUIObj_Img::SetImg(HGUIIMG hImg)
{
	p_pImage = hImg;
	Redraw();
}

void NLGUIObj_Img::_Draw(ID2D1DeviceContext* pContext)
{
	if (p_pImage)
	{
		NLGUI_CvsDrawBitmap(pContext, p_pImage, p_nScaleMode, p_fRadius, 1, D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom));
	}
	if (p_bMask) 
	{
		m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_IMG_MASK));
		pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
	}
	if (p_bStroke)
	{
		m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE));
		pContext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
	}
}

void NLGUIObj_Inst::Create(NLGUIBase* pGUI, FLOAT fRadius, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_fRadius = fRadius;
	_Create(pGUI, NLGUI_OBJTYPE_INST, FALSE, FALSE, left, top, width, height);
}

void NLGUIObj_Inst::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_SYS));
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
}

void NLGUIObj_Con2::Create(NLGUIBase* pGUI, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	_Create(pGUI, NLGUI_OBJTYPE_CON2, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Con2::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE));
	pContext->DrawLine(D2D1::Point2F(m_w_left, m_w_top), D2D1::Point2F(m_w_right, m_w_top), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BKG));
	pContext->FillRectangle(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), m_pWnd->m_pBrush);
}

void NLGUIObj_Btn3::Create(NLGUIBase* pGUI, LPCWSTR lpTitle, FLOAT fRadius, INT nTextSize, INT nTextLeft, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_lpTitle = StrDupW(lpTitle);
	p_fRadius = fRadius;
	p_nTextSize = nTextSize;
	p_nTextLeft = nTextLeft;
	_Create(pGUI, NLGUI_OBJTYPE_BTN3, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Btn3::SetSelect(BOOL bSelect)
{
	p_bSelect = bSelect;
	Redraw();
}

void NLGUIObj_Btn3::SetTitle(LPCWSTR lpTitle)
{
	NL_MemFree((LPVOID)p_lpTitle);
	p_lpTitle = StrDupW(lpTitle);
	Redraw();
}

void NLGUIObj_Btn3::_Draw(ID2D1DeviceContext* pContext)
{
	D2D1_COLOR_F bkg = p_bSelect ? m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN3_SELECT) : D2D1::ColorF(0, 0, 0, 0);
	switch (p_nState)
	{
	case NLGUI_BTN_STATE_ENTER:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN3_ENTER);
		break;
	case NLGUI_BTN_STATE_DOWN:
		bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BTN3_DOWN);
		break;
	}
	m_pWnd->m_pBrush->SetColor(bkg);
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), p_fRadius, p_fRadius), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_TEXT1));
	NLGUI_CvsDrawText(pContext, p_lpTitle, m_pWnd->m_lpFont, p_nTextSize, FALSE, m_pWnd->m_pBrush, 
		D2D1::RectF(m_w_left + p_nTextLeft, m_w_top, m_w_right, m_w_bottom),
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER, DWRITE_TEXT_ALIGNMENT_LEADING, FALSE);
}

void NLGUIObj_Btn3::_ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSEENTER:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELEAVE:
		p_nState = NLGUI_BTN_STATE_NORMAL;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELDOWN:
		p_nState = NLGUI_BTN_STATE_DOWN;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELUP:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	}
	_RunEvent(nEvent, c1, c2, c3, c4);
}

void NLGUIObj_Con3::Create(NLGUIBase* pGUI, BOOL bShadow, BOOL bBlur, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_bBlur = bBlur;
	p_bShadow = bShadow;
	_Create(pGUI, NLGUI_OBJTYPE_CON3, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_Con3::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE, 1));
	pContext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 6, 6), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_BKG, 1));
	pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 6, 6), m_pWnd->m_pBrush);
}

void NLGUIObj_CoverBtn::Create(NLGUIBase* pGUI, HGUIIMG hImg, HGUIIMG hBtnImg, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_pImage = hImg;
	p_pBtnImage = hBtnImg;
	_Create(pGUI, NLGUI_OBJTYPE_COVERBTN, TRUE, FALSE, left, top, width, height);
}

void NLGUIObj_CoverBtn::SetImg(HGUIIMG hImg)
{
	p_pImage = hImg;
	Redraw();
}

void NLGUIObj_CoverBtn::_Draw(ID2D1DeviceContext* pContext)
{
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE));
	pContext->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 6, 6), m_pWnd->m_pBrush);
	FLOAT width = m_w_right - m_w_left, height = m_w_bottom - m_w_top;
	if (p_pImage)
	{
		NLGUI_CvsDrawBitmap(pContext, p_pImage, 0, 6, 1, D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom));
	}
	if (p_nState == NLGUI_BTN_STATE_ENTER || p_nState == NLGUI_BTN_STATE_DOWN)
	{
		D2D1_COLOR_F bkg = D2D1::ColorF(0, 0, 0, 0);
		switch (p_nState)
		{
		case NLGUI_BTN_STATE_ENTER:
			bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_COVERBTN_ENTER);
			break;
		case NLGUI_BTN_STATE_DOWN:
			bkg = m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_COVERBTN_DOWN);
			break;
		}
		m_pWnd->m_pBrush->SetColor(bkg);
		pContext->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(m_w_left, m_w_top, m_w_right, m_w_bottom), 6, 6), m_pWnd->m_pBrush);
		if (p_pBtnImage)
		{
			NLGUI_CvsDrawIconBitmap(pContext, (ID2D1Bitmap*)p_pBtnImage, 1, D2D1::RectF(m_w_left + width / 2 - 16, m_w_top + height / 2 - 16, m_w_left + width / 2 + 16, m_w_top + height / 2 + 16));
		}
	}
}

void NLGUIObj_CoverBtn::_ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	switch (nEvent)
	{
	case NLGUI_EVENT_MOUSEENTER:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELEAVE:
		p_nState = NLGUI_BTN_STATE_NORMAL;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELDOWN:
		p_nState = NLGUI_BTN_STATE_DOWN;
		Redraw();
		break;
	case NLGUI_EVENT_MOUSELUP:
		p_nState = NLGUI_BTN_STATE_ENTER;
		Redraw();
		break;
	}
	_RunEvent(nEvent, c1, c2, c3, c4);
}

void NLGUIObj_Slider::Create(NLGUIBase* pGUI, BOOL bAutoSmall, BOOL bVertical, FLOAT left, FLOAT top, FLOAT width, FLOAT height)
{
	p_fMin = 0;
	p_fMax = 1;
	p_fPos = 0;
	_Create(pGUI, NLGUI_OBJTYPE_SLIDER, TRUE, TRUE, left, top, width, height);
}

void NLGUIObj_Slider::SliderSetPos(LONG fPos)
{
	p_fPos = fPos;
	if (p_fPos > p_fMax)
	{
		p_fPos = p_fMax;
	}
	if (p_fPos < p_fMin)
	{
		p_fPos = p_fMin;
	}
}

LONG NLGUIObj_Slider::GetPos()
{
	return p_fPos;
}

void NLGUIObj_Slider::SetInfo(LONG fMin, LONG fMax, LONG fWheel)
{
	p_fMin = fMin;
	p_fMax = fMax;
	p_fWheel = fWheel;
	if (p_fPos > p_fMax)
	{
		p_fPos = p_fMax;
	}
	if (p_fPos < p_fMin)
	{
		p_fPos = p_fMin;
	}
}

void NLGUIObj_Slider::_Draw(ID2D1DeviceContext* pContext)
{
	INT width = m_w_right - m_w_left, height = m_w_bottom - m_w_top;
	INT barPosWidth =
		((DOUBLE)(p_fPos - p_fMin) / (DOUBLE)(p_fMax - p_fMin) * width);
	INT sliderSize = 16;
	INT barHeight = 4;

	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_SLIDER_BKG, 1));
	pContext->FillRoundedRectangle(D2D1::RoundedRect(
		D2D1::RectF(m_w_left, m_w_top + height / 2 - barHeight / 2, m_w_right, m_w_top + height / 2 - barHeight / 2 + barHeight),
		barHeight / 2, barHeight / 2), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_SYS, 1));
	D2D1_RECT_F prect;
	prect = D2D1::RectF(m_w_left, m_w_top + height / 2 - barHeight / 2, m_w_left + barPosWidth, m_w_top + height / 2 - barHeight / 2 + barHeight);
	pContext->FillRoundedRectangle(D2D1::RoundedRect(prect, barHeight / 2, barHeight / 2), m_pWnd->m_pBrush);
	D2D1_POINT_2F pos = D2D1::Point2F(m_w_left + barPosWidth, m_w_top + height / 2);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_STROKE, 1));
	pContext->DrawEllipse(D2D1::Ellipse(pos, sliderSize / 2, sliderSize / 2), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_SLIDER_SLIDERBKG, 1));
	pContext->FillEllipse(D2D1::Ellipse(pos, sliderSize / 2, sliderSize / 2), m_pWnd->m_pBrush);
	m_pWnd->m_pBrush->SetColor(m_pWnd->_GetThemeColor(NLGUI_THEMECOLOR_SYS, 1));
	pContext->FillEllipse(D2D1::Ellipse(pos, sliderSize / 2 - 3.5, sliderSize / 2 - 3.5), m_pWnd->m_pBrush);
}

void NLGUIObj_Slider::_ObjEvent(INT nEvent, LPVOID c1, LPVOID c2, LPVOID c3, LPVOID c4)
{
	_RunEvent(nEvent, c1, c2, c3, c4);
}
