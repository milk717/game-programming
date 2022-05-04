#pragma once
//Windows 관련 헤더
#include <Windows.h>

//C 런타임 관련 헤더
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include<tchar.h>

//DX 관련 헤더
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

//인터페이스 객체의 안전한 반납을 위함
#define SAFE_RELEASE(p){if(p) {(p)->Release(); (p)=NULL;}}

//현재 실행 모듈의 HINSTACE를 불러오기 위함
#ifndef  HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif // ! HINST_THISCOMPONENT


class DemoApp {
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize(HINSTANCE hInstance);	//HRESULT == 32bit signed 정수형
	void RunMessageLoop();


private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);	//UINT == unsigned int의 약자

	//LPARAM은 포인터 값을 전달할 때 사용.
	//WPARAM은 핸들 또는 정수를 받아들일 때 주로 사용
	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);	// LRESULT == long타입, 

	//클래스 변수들 선언
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	//브러쉬
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;	//디폴트 회색상자 그릴 때
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;	//추가되고있는 상자
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_RectangleBrush;

	//DWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};