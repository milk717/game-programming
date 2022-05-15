#pragma once

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

// �ڿ� ���� ��ȯ ��ũ��.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// ���� ����� �����ּ� ���.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize();
	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, __deref_out ID2D1BitmapBrush** ppBitmapBrush);

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);

	//��� �и� �Լ���
	void DrawBackground(D2D1_SIZE_F renderTargetSize);	//��� �׸���

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;
	ID2D1Bitmap* m_pBitmap;
};
