#include "DemoApp.h"

#include <tchar.h>
#include <ctime>
#include <cmath>

#define GROUND_Y_POSITION 400	//바닥의 Y좌표
#define OBJECT_START_X_POSITION 1000
#define OBJECT_END_X_POSITION -100
#define OBJECT_PATH_LENGTH -1100

//디버깅용 함수
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
double temp = 0;	//좌표 임시저장
double chy = 0;
double jump;
int backgroundPosition = 1;


/* 현재 마우스 위치 좌표 */
D2D_POINT_2F currentMousePosition;

void DemoApp::WriteActionInfo()
{
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	// debug
	RECT rc;
	GetClientRect(m_hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	WCHAR szText[250];
	swprintf_s(szText, L"마우스x : %1.f\n마우스y : %1.f\n장애물 위치 = %f\n캐릭터 좌표 = (0, %f)",
		currentMousePosition.x, currentMousePosition.y, temp, chy);

	m_pRenderTarget->DrawText(
		szText,
		wcslen(szText),
		m_pTextFormat,
		D2D1::RectF(10.0f, 30.0f, 500.0f, 240.0f),
		m_pRedBrush
	);

	swprintf_s(szText, L"점수 : %05d", score/100);

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


	// D2D 팩토리를 생성함.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	/*
	* TextFormat
	*/
	if (SUCCEEDED(hr))
	{
		// DirectWrite 팩토리를 생성함.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr))
	{
		//DirectWrite 텍스트 포맷 객체를 생성함.
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

	// 장애물 경로 기하를 생성함.
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

	// 배경 경로 기하를 생성함.
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
		pSink->AddLine(D2D1::Point2F(OBJECT_PATH_LENGTH, 0));

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}
	SAFE_RELEASE(pSink);

	return hr;
}


HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	//캐릭터 이동 애니메이션
	static float char_animation_time = 0.0F;
	static float anim_time = 0.0f;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();


		// 그리기를 준비함.
		m_pRenderTarget->BeginDraw();

		// 변환을 항등행렬로 리셋함.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// 렌더타겟을 클리어함.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));


		//배경 그리기
		{
			float acceleration = Acceleration();
			TRACE(L"가속도 = %f\n", acceleration);

			backgroundPosition = -(int)(this->score*acceleration)%944;

			TRACE(L"배경 위치 = %f\n", backgroundPosition);

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

		ScoreCountStart();	//점수카운트하기
		WriteActionInfo();	//info 출력

		//장애물 그리기
		{
			D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(OBJECT_START_X_POSITION, GROUND_Y_POSITION);

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			float length = m_Animation.GetValue(anim_time);
			// 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
			m_pRedBoxGeometry->ComputePointAtLength(length, NULL, &point, NULL);
			// 사각형의 방향을 조절하여 이동 동선을 따라가는 방향이 되도록 함.
			D2D1_MATRIX_3X2_F objectMatrix = D2D1::Matrix3x2F(
				1, 0,
				0, 1,
				point.x, point.y);
			//TRACE(L"(x,y) = (%f, %f)\n", point.x, point.y);
			m_pRenderTarget->SetTransform(objectMatrix * translation);
			// 사각형을 빨간색으로 그림.
			m_pRenderTarget->FillGeometry(m_pObjectGeometry, m_pRedBrush);
		}


		//미니언 그리기 & 미니언 점프
		D2D1_SIZE_F size = m_pCharactorBitmap->GetSize();	//비트맵 사이즈 얻기
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

		//장애물 충돌했을 때
		temp = 944+point.x;
		chy = jump;
		if (isCrash()&&isStart) {
			this->score = 0;
			isStart = false;
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(0, 0));
			D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 944, 1000);
			m_pRenderTarget->FillRectangle(rcBrushRect, m_pGameoverBitmapBrush);
			hr = m_pRenderTarget->EndDraw();
			Sleep(2000);
			TRACE(L"true");
		}

		// 그리기 연산들을 제출함.
		hr = m_pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		// 애니메이션의 끝에 도달하면 다시 처음으로 되돌려서 반복되도록 함.
		if (anim_time >= m_Animation.GetDuration())
		{
			anim_time = 0.0f;
			//charAnimationTime = 0.0f;
		}
		else if (char_animation_time >= 1.47) {
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
	if (temp>=0 && temp <= 80) { 
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

				// 여기에서 ValidateRect를 호출하지 말아야 OnRender 함수가 계속 반복 호출됨.
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYDOWN:
			{
				spaceTime = clock();
				if (!(pDemoApp->isStart)) {	//시작 안했을 경우 시작해줌
					pDemoApp->startTime = clock();
					pDemoApp->isStart = true;
					isJumpClick = true;
				}
				else if (pDemoApp->isStart && !isJumpClick) {	//시작했고 점프 안한 상태에서 키 눌렸을 때
					isJumpClick = true;		//점프라고 표시
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
