#pragma once

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <ctime>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#include "Animation.h"

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

	//��α���
	ID2D1PathGeometry* m_pPathGeometry;
	ID2D1PathGeometry* m_pObjectGeometry;
	ID2D1PathGeometry* m_pRedBoxGeometry;

	//�ִϸ��̼�
	AnimationLinear<float> m_Animation;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	//��
	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pYellowBrush;
	ID2D1SolidColorBrush* m_pTextBrush;

	//��Ʈ��
	ID2D1Bitmap* m_pBitmap;	//��� ��Ʈ��
	ID2D1Bitmap* m_pCharactorBitmap;	//ĳ���� ��Ʈ��
	ID2D1Bitmap* m_pGameoverBitmap;	//���ӿ��� ��Ʈ��

	//��Ʈ�� �귯��
	ID2D1BitmapBrush* m_pBackgroundBitmapBrush;
	ID2D1BitmapBrush* m_pGameoverBitmapBrush;
	ID2D1BitmapBrush* m_pCharactorBitmapBrush;

	//�ؽ�Ʈ
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextFormat* m_score_TextFormat;

	//�������� ���� ����
	int score = 0;		//����
	bool isStart = false;
	int startTime = 0;		//�����̽� ������ �� ���� ������ �ð��� �����ϴ� ����
	int backgroundPosition = 1;

	//��ֹ� ��ġ
	int redBox_x = 1100;


public:
	void WriteActionInfo();
	void ScoreCountStart();

	//���ӹ���
	float Acceleration();
	bool isCrash();
};