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

	//경로기하
	ID2D1PathGeometry* m_pPathGeometry;
	ID2D1PathGeometry* m_pObjectGeometry;
	ID2D1PathGeometry* m_pCharGeometry;


	//애니메이션
	AnimationLinear<float> m_Animation;
	AnimationLinear<float> m_JumpAnimation;


	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	//붓
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	ID2D1SolidColorBrush* m_pTextBrush;


	//비트맵
	ID2D1Bitmap* m_pBitmap;	//배경 비트맵
	ID2D1Bitmap* m_pCharactorBitmap;	//캐릭터 비트맵

	//비트맵 브러쉬
	ID2D1BitmapBrush* m_pBackgroundBitmapBrush;
	ID2D1BitmapBrush* m_pGridPatternBitmapBrush;
	ID2D1BitmapBrush* m_pCharactorBitmapBrush;

	//텍스트
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextFormat* m_score_TextFormat;

public:
	HRESULT CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, __deref_out ID2D1BitmapBrush** ppBitmapBrush);
	void WriteActionInfo();

};