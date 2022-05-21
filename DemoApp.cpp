#include "DemoApp.h"

#include <tchar.h>
#include <ctime>
#include <cmath>

//������ �Լ�
#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
#define TRACE TRACE_WIN32
#pragma warning(disable: 4996)
void TRACE_WIN32(LPCTSTR lpszFormat, ...) {
	TCHAR lpszBuffer[0x160]; //buffer size
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vstprintf(lpszBuffer, lpszFormat, fmtList);
	va_end(fmtList);
	OutputDebugString(lpszBuffer);
}
#endif

bool isJumpClick = false;
D2D1_POINT_2F charPoint = {30,400};
double spaceTime;
double temp = 0;	//��ǥ �ӽ�����
double chx = 0;
double chy = 0;
double jump;

/* ���� ���콺 ��ġ ��ǥ */
D2D_POINT_2F currentMousePosition;

void DemoApp::WriteActionInfo()
{
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	// debug
	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	WCHAR szText[250];
	swprintf_s(szText, L"���콺x : %1.f\n���콺y : %1.f\n��ֹ� ��ġ = %f\nĳ���� ��ǥ = (%f, %f)",
		currentMousePosition.x, currentMousePosition.y, temp, chx, chy);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 30.0f, 500.0f, 240.0f),
		m_pRedBrush
	);

	swprintf_s(szText, L"���� : %d", score);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_score_TextFormat,
		D2D1::RectF(800.f, 30.0f, 960.0f, 240.0f),
		m_pTextBrush
	);

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink* pSink = NULL;


	// D2D ���丮�� ������.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	/*
	* TextFormat
	*/
	if (SUCCEEDED(hr))
	{
		// DirectWrite ���丮�� ������.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr))
	{
		//DirectWrite �ؽ�Ʈ ���� ��ü�� ������.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"en-us",
			&m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			25.0f,
			L"en-us",
			&m_score_TextFormat
		);
	}

	// ���� ����� ��� ���ϸ� ������.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pPathGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { 960, 170 };

		pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddLine({ 960, 170.0F });

		pSink->AddLine({ -960,170.0F });

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	SAFE_RELEASE(pSink);

	// ��ֹ� ��� ���ϸ� ������.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pObjectGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pObjectGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(D2D1::Point2F(-25.0f, -50.0f), D2D1_FIGURE_BEGIN_FILLED);

		const D2D1_POINT_2F ptTriangle[] = { {-50.0f, -50.0f}, {-50.0f, 50.0f}, {50.0f, 50.0f},  {50.0f, -50.0f} };
		pSink->AddLines(ptTriangle, 4);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	// ��� ��� ���ϸ� ������.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pBackgroundGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pBackgroundGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { 1350.0f, 0.0f };

		pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(-1350, 0));

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	return hr;
}





HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	//ĳ���� �̵� �ִϸ��̼�
	static float char_animation_time = 0.0F;
	static float anim_time = 0.0f;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		D2D1_MATRIX_3X2_F triangleMatrix;
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 720;
		TRACE(L"%f\n", minWidthHeightScale);

		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(rtSize.width /2, rtSize.height/2 );

		// �׸��⸦ �غ���.
		m_pRenderTarget->BeginDraw();

		// ��ȯ�� �׵���ķ� ������.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// ����Ÿ���� Ŭ������.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		ScoreCountStart();	//����ī��Ʈ�ϱ�
		WriteActionInfo();	//info ���

		//��� �׸���

		////��� �����̰�
		float backLength = m_Animation.GetValue(char_animation_time);
		D2D1::Matrix3x2F backgroundScale = D2D1::Matrix3x2F::Scale(1, 1,D2D1::Point2F(0,0));


		D2D1_POINT_2F backpoint = D2D1::Point2F(0, 0);
		m_pBackgroundGeometry->ComputePointAtLength(backLength, NULL, &backpoint, &tangent);
		triangleMatrix = D2D1::Matrix3x2F(
			1, 0,
			0, 1,
			backpoint.x, backpoint.y);
		m_pRenderTarget->SetTransform(triangleMatrix * backgroundScale);
		//����� �׸�
		D2D1_SIZE_F backGroundBitmapSize = m_pBitmap->GetSize();
		m_pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
				0, 0,
				backpoint.x, rtSize.height
			)
		);

		backpoint = D2D1::Point2F(rtSize.width, 0);
		triangleMatrix = D2D1::Matrix3x2F(
			1, 0,
			0, 1,
			backpoint.x, backpoint.y);
		m_pRenderTarget->SetTransform(triangleMatrix * backgroundScale);
		//����� �׸�
		m_pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
				0, 0,
				backpoint.x, rtSize.height
			)
		);

		//��ֹ� �׸���
		float length = m_Animation.GetValue(anim_time);
		// ���� �ð��� �ش��ϴ� ���� ���̿� ��ġ�ϴ� �̵� ���� ���� ������ ����.
		m_pPathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);
		// �簢���� ������ �����Ͽ� �̵� ������ ���󰡴� ������ �ǵ��� ��.
		triangleMatrix = D2D1::Matrix3x2F(
			tangent.x, tangent.y,
			-tangent.y, tangent.x,
			point.x, point.y);
		//TRACE(L"(x,y) = (%f, %f)\n", point.x, point.y);
		m_pRenderTarget->SetTransform(triangleMatrix * scale * translation);
		// �簢���� ���������� �׸�.
		m_pRenderTarget->FillGeometry(m_pObjectGeometry, m_pRedBrush);


		
		//�̴Ͼ� �׸��� & �̴Ͼ� ����
		D2D1_SIZE_F size = m_pCharactorBitmap->GetSize();	//��Ʈ�� ������ ���
		D2D1_POINT_2F leftGround = D2D1::Point2F(0.f, 400);
		if (isJumpClick) {
			jump = 100 + std::abs(300 - (clock() - spaceTime) * 0.5);
			if (jump > 400) {
				jump = 400;
			}
			leftGround = D2D1::Point2F(0.f, jump);
			//TRACE(L"%f\n", jump);
		}
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(leftGround.x, leftGround.y));
		m_pRenderTarget->FillRectangle(&D2D1::RectF(0, 0, size.width, size.height), m_pCharactorBitmapBrush);

		//��ֹ� �浹���� ��
		temp = point.x;
		chx = 0;
		chy = jump;
		if (isCrash()&&isStart) {
			score = 0;
			isStart = false;
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0, 0));
			D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 1000, 10000);
			m_pRenderTarget->FillRectangle(rcBrushRect, m_pGameoverBitmapBrush);
			hr = m_pRenderTarget->EndDraw();
			Sleep(3000);
			TRACE(L"true");
		}

		// �׸��� ������� ������.
		hr = m_pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		// �ִϸ��̼��� ���� �����ϸ� �ٽ� ó������ �ǵ����� �ݺ��ǵ��� ��.
		if (anim_time >= m_Animation.GetDuration())
		{
			TRACE(L"%f", m_Animation.GetDuration());
			anim_time = 0.0f;
			//charAnimationTime = 0.0f;
		}
		else if (char_animation_time >= 2.4) {
			char_animation_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			anim_time += elapsedTime;
			char_animation_time += elapsedTime;
		}
	}

	return hr;
}

bool DemoApp::isCrash() {
	//TRACE(L"object = %d\tcharactor = (%f, %f)\n",temp, chx, chy);
	if (temp>=-670 && temp <= -590) { 
		//TRACE(L"temp\n");
		if (330 <= chy && chy <= 400) {
			//TRACE(L"chy\n");
			return true;
		}
	}
	return false;
}

void DemoApp::OnResize(UINT width, UINT height)
{ 
	if (m_pRenderTarget)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		m_pRenderTarget->Resize(size);
	}
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			{
				pDemoApp->OnRender();

				// ���⿡�� ValidateRect�� ȣ������ ���ƾ� OnRender �Լ��� ��� �ݺ� ȣ���.
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYDOWN:
			{
				spaceTime = clock();
				if (!(pDemoApp->isStart)) {	//���� ������ ��� ��������
					pDemoApp->startTime = clock();
					pDemoApp->isStart = true;
					isJumpClick = true;
				}
				else if (pDemoApp->isStart && !isJumpClick) {	//�����߰� ���� ���� ���¿��� Ű ������ ��
					isJumpClick = true;		//������� ǥ��
				}
			}

			case WM_MOUSEMOVE:
			{
				currentMousePosition.x = LOWORD(lParam);
				currentMousePosition.y = HIWORD(lParam);
				InvalidateRect(hwnd, NULL, FALSE);
				break;
			}

			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}
