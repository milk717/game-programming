#include "DemoApp.h"

#include <tchar.h>
#include <ctime>
#include <cmath>
#include <cstdio>

#define GROUND_Y_POSITION 400	//�ٴ��� Y��ǥ
#define OBJECT_NUMBER 5

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
double jump = 400;
bool isDoubleJump = false;
int backgroundPosition = 1;
int objectPosition=1;
int objectRandom[OBJECT_NUMBER] = { 3000,9000,5000,7000,2000 };
int objectRandom2[OBJECT_NUMBER] = { 6000,9000,5000,7000,13000 };

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
	swprintf_s(szText, L"���콺x : %1.f\n���콺y : %1.f\n��ֹ� ��ġ = %f\nĳ���� ��ǥ = (0, %f)",
		currentMousePosition.x, currentMousePosition.y, temp, jump);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 30.0f, 500.0f, 240.0f),
		m_pTextBrush
	);

	swprintf_s(szText, L"���� : %05d", score/10);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_score_TextFormat,
		D2D1::RectF(750.f, 30.0f, 944.0f, 240.0f),
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
		pSink->BeginFigure(D2D1::Point2F(0.0f, 0.0f), D2D1_FIGURE_BEGIN_FILLED);

		const D2D1_POINT_2F ptObject[] = { {100.0f, 0.0f}, {100.0f, 100.0f}, {0.0f, 100.0f},  {0.0f, 0.0f} };
		pSink->AddLines(ptObject, 4);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	// ��� ��� ���ϸ� ������.
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pRedBoxGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pRedBoxGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { 0.0f, 0.0f };

		pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(0, 100));
		pSink->AddLine(D2D1::Point2F(0, 0));

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	return hr;
}


HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	static float anim_time = 0.0f;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point;

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();


		// �׸��⸦ �غ���.
		m_pRenderTarget->BeginDraw();

		// ��ȯ�� �׵���ķ� ������.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// ����Ÿ���� Ŭ������.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));


		//��� �׸���
		{
			float acceleration = Acceleration();
			//TRACE(L"���ӵ� = %f\n", acceleration);

			backgroundPosition = -(int)(this->score*acceleration)%944;

			//TRACE(L"��� ��ġ = %f\n", backgroundPosition);

			m_pRenderTarget->DrawBitmap(
				m_pBitmap,
				D2D1::RectF(
					backgroundPosition, 0,
					backgroundPosition+rtSize.width, rtSize.height
				)
			);
			m_pRenderTarget->DrawBitmap(
				m_pBitmap,
				D2D1::RectF(
					backgroundPosition+rtSize.width, 0,
					backgroundPosition + rtSize.width*2, rtSize.height
				)
			);
		}

		ScoreCountStart();	//����ī��Ʈ�ϱ�
		WriteActionInfo();	//info ���

		//�� �׸���
		{
			D2D1_SIZE_F size = m_pBirdBitmap->GetSize();	//��Ʈ�� ������ ���

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(rtSize.width/2, -170);
			D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(0.5f, 0.5f));

			float length = m_Animation.GetValue(anim_time);
			// ���� �ð��� �ش��ϴ� ���� ���̿� ��ġ�ϴ� �̵� ���� ���� ������ ����.
			m_pRedBoxGeometry->ComputePointAtLength(length, NULL, &point, NULL);
			D2D1_MATRIX_3X2_F objectMatrix = D2D1::Matrix3x2F(
				1, 0,
				0, 1,
				point.x, point.y);
			//TRACE(L"(x,y) = (%f, %f)\n", point.x, point.y);
			m_pRenderTarget->SetTransform(objectMatrix * translation * scale);
			m_pRenderTarget->FillRectangle(&D2D1::RectF(0, 0, size.width, size.height), m_pBirdBitmapBrush);
		}

		//��ֹ� �׸���
		{
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			float acceleration = Acceleration();

			for(int i = 0; i< OBJECT_NUMBER; i++)
			{
				if(score<50000)
				{
					objectPosition = -(int)(this->score * acceleration) % objectRandom[i];

				}else
				{
					objectPosition = -(int)(this->score * acceleration) % objectRandom2[i];
				}
				D2D1_RECT_F rectangle = D2D1::RectF(objectPosition + rtSize.width, 500, objectPosition + rtSize.width + 100, 400);
				m_pRenderTarget->FillRectangle(rectangle, m_pRedBrush);
				//��ֹ� ĳ���� �浹����
				temp = objectPosition + rtSize.width;
				if (isCrash() && isStart) {
					this->score = 0;
					isStart = false;
					m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0, 0));
					D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 944, 1000);
					m_pRenderTarget->FillRectangle(rcBrushRect, m_pGameoverBitmapBrush);
					hr = m_pRenderTarget->EndDraw();
					Sleep(2000);
					TRACE("true");
				}
			}
			TRACE("��ֹ� ��ġ = %f\n", objectPosition+rtSize.width);
		}

		//�̴Ͼ� �׸��� & �̴Ͼ� ����
		D2D1_SIZE_F size = m_pCharactorBitmap->GetSize();	//��Ʈ�� ������ ���
		D2D1_POINT_2F leftGround = D2D1::Point2F(0.f, 400);
		if (isJumpClick) {
			if(score > 50000)
			{
				jump = 100 + std::pow(300 - (clock() - spaceTime), 2) * 0.0009;
			}else
			{
				jump = 100 + std::pow(300 - (clock() - spaceTime), 2) * 0.0005;
			}
			if (jump > 400) {
				jump = 400;
				isJumpClick = false;
				isDoubleJump = false;
			}
			leftGround = D2D1::Point2F(0.f, jump);
			//TRACE(L"%f\n", jump);
		}
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(leftGround.x, leftGround.y));
		m_pRenderTarget->FillRectangle(&D2D1::RectF(0, 0, size.width, size.height), m_pCharactorBitmapBrush);


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
			anim_time = 0.0f;
			//charAnimationTime = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			anim_time += elapsedTime;
		}
	}

	return hr;
}

bool DemoApp::isCrash() {
	//TRACE(L"object = %d\tcharactor = (%f, %f)\n",temp, chx, chy);
	if (temp>=0 && temp <= 80) { 
		//TRACE(L"temp\n");
		if (330 <= jump && jump <= 400) {
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
				if (!(pDemoApp->isStart)) {	//���� ������ ��� ��������
					spaceTime = clock();
					pDemoApp->startTime = clock();
					pDemoApp->isStart = true;
					isJumpClick = true;
				}
				else if (pDemoApp->isStart && !isJumpClick) {	//�����߰� ���� ���� ���¿��� Ű ������ ��
					spaceTime = clock();
					isJumpClick = true;		//������� ǥ��
				}else if(pDemoApp->isStart && pDemoApp->score>50000 && isJumpClick && !isDoubleJump)	//�����߰�, �̴����� �����ϰ�, �ѹ� ������ ���¿���
				{
					isDoubleJump = true;
					spaceTime = clock();
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
