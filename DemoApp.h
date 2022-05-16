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
#include <dwmapi.h>

#include "Animation.h"

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif
#endif

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize();
	void RunMessageLoop();

	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);


private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hwnd;
	ID2D1Factory* m_pD2DFactory;
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;


	AnimationEaseInOut<float> m_Animation;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	//경로기하
	ID2D1PathGeometry* m_pPathGeometry;
	ID2D1PathGeometry* m_pObjectGeometry;
	//ID2D1PathGeometry* m_pCharactorPathGeometry;

	//붓
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	

	//비트맵
	ID2D1Bitmap* m_pBitmap;	//배경 비트맵
	ID2D1Bitmap* m_pCharactorBitmap;	//캐릭터 비트맵

	//비트맵 브러쉬
	ID2D1BitmapBrush* m_pBackgroundBitmapBrush;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;
	ID2D1BitmapBrush* m_pCharactorBitmapBrush;

public:
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, __deref_out ID2D1BitmapBrush** ppBitmapBrush);

};
