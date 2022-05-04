#pragma once
//Windows ���� ���
#include <Windows.h>

//C ��Ÿ�� ���� ���
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include<tchar.h>

//DX ���� ���
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

//�������̽� ��ü�� ������ �ݳ��� ����
#define SAFE_RELEASE(p){if(p) {(p)->Release(); (p)=NULL;}}

//���� ���� ����� HINSTACE�� �ҷ����� ����
#ifndef  HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif // ! HINST_THISCOMPONENT


class DemoApp {
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize(HINSTANCE hInstance);	//HRESULT == 32bit signed ������
	void RunMessageLoop();


private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);	//UINT == unsigned int�� ����

	//LPARAM�� ������ ���� ������ �� ���.
	//WPARAM�� �ڵ� �Ǵ� ������ �޾Ƶ��� �� �ַ� ���
	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam);	// LRESULT == longŸ��, 

	//Ŭ���� ������ ����
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	//�귯��
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;	//����Ʈ ȸ������ �׸� ��
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;	//�߰��ǰ��ִ� ����
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_RectangleBrush;

	//DWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};