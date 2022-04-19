#include "DrawRectangle.h"
#include <tchar.h>

//전역변수 선언
D2D_POINT_2F currentPosition; // 현재 마우스 좌표 

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


DemoApp::DemoApp():	//생성자에서 클래스 변수 NULL로 초기화
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pBlackBrush(NULL),
	m_pTextFormat(NULL),
	m_pDWriteFactory(NULL)
{
}

DemoApp::~DemoApp() {
	DiscardDeviceResources();	//모든 장치 의존적 자원을 반납하는 함수
	SAFE_RELEASE(m_pDirect2dFactory);	//장치 독립적 자원인 d2d 팩토리도 필요하지 않으므로 반납
}

void DemoApp::RunMessageLoop() {
	MSG msg;

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//윈도우 초기화
HRESULT DemoApp::Initialize(HINSTANCE hInstance) {
	HRESULT hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr)) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL,IDI_APPLICATION);
		wcex.lpszClassName = "D2DDemoApp";
		RegisterClassEx(&wcex);
		m_hwnd = CreateWindow(
			"D2DDemoApp", "Direct2D Demo Application", 
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480, NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}
	return hr;
}

//RunMessageLoop를 순차적으로 호출
int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (SUCCEEDED(CoInitialize(NULL))) {
		{
			DemoApp app;
			if (SUCCEEDED(app.Initialize(hlnstance))) {
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}

HRESULT DemoApp::CreateDeviceIndependentResources() {
	HRESULT hr = S_OK;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	return hr;
}

//장치 의존적 자원들을 생성
HRESULT DemoApp::CreateDeviceResources() {
	HRESULT hr = S_OK;

	if (!m_pRenderTarget) {	//렌더타겟이 이미 유효할 땐 실행 x
		RECT rc; GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		//랜더타켓 생성
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget);
		if (SUCCEEDED(hr)) {
			//회색 붓
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}
		if (SUCCEEDED(hr)) {
			//파란색 붓
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
		if (SUCCEEDED(hr)) {
			//검은색 붓
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}
		//텍스트 포맷 관련
		if (SUCCEEDED(hr))
		{
			hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
			);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pDWriteFactory->CreateTextFormat(
				L"Verdana",
				NULL,
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				13.0f,
				L"en-us",
				&m_pTextFormat
			);
		}
	}
	return hr;
}

//모든 장치 의존적 자원을 반납하는 함수. 소멸자에서 불러옴.
void DemoApp::DiscardDeviceResources() {
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
}

//윈도우 메시지 처리를 위한 함수
LRESULT CALLBACK DemoApp::WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, PtrToUlong(pDemoApp));
		result = 1;
	}
	else {
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(
			GetWindowLongPtrW(hWnd, GWLP_USERDATA)));
		bool wasHandled = false;
		if (pDemoApp) {
			switch (message) {
				case WM_SIZE:
				{ 
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pDemoApp->OnResize(width, height);
				}
				result = 0; wasHandled = true; break;
				case WM_DISPLAYCHANGE :
				{ 
					InvalidateRect(hWnd, NULL, FALSE);
				}
				result = 0; wasHandled = true; break;
				case WM_PAINT:
				{
					pDemoApp->OnRender();
					ValidateRect(hWnd, NULL);
					return 0;
				}
				result = 0; wasHandled = true; break;
				case WM_MOUSEMOVE:
				{
					currentPosition.x = LOWORD(lParam);
					currentPosition.y = HIWORD(lParam);
					TRACE_WIN32("X = %.2f, Y = %.2f\n",currentPosition.x,currentPosition.y);
					InvalidateRect(hWnd, NULL, false);
					return 0;
				}
				case WM_DESTROY:
				{ 
					PostQuitMessage(0);
				}
				result = 1; wasHandled = true; break;
			}
		}
		if (!wasHandled)
			result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}

HRESULT DemoApp::OnRender()
{
	/*
	✔️회색 사각형 그려주기(높이:너비 = 1:4)
	마우스좌표, 회전각, 벡터사이즈 등 표시하는 캡션 텍스트 그리기
	마우스 왼쪽버튼 눌리면
	회색박스 안에서 눌리면 학생추가
	스택 꼭대기에서 눌리면 학생삭제
	(일단은 상자안에 글씨말고 상자만 그려보자)
	*/
	HRESULT hr = S_OK;
	hr = CreateDeviceResources();

	//참고한부분
	// 렌더타겟 변환을 항등 변환으로 리셋함.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	if (SUCCEEDED(hr)) { // 렌더타겟이 유효함. 
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);
		for (int x = 0; x < width; x += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
				D2D1::Point2F(static_cast<float>(x), rtSize.height),
				m_pLightSlateGrayBrush, 0.5f);
		}
		for (int y = 0; y < height; y += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
				D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
				m_pLightSlateGrayBrush, 0.5f);
		}
		InitRender();	//화면 위쪽에 사각형 그리고 글자표시

		hr = m_pRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET) { // 랜더타겟을 재생성해야 함. 
			hr = S_OK;
			DiscardDeviceResources();
		}
	}
	return hr;
}
void DemoApp::OnResize(UINT width, UINT height) {
	if (m_pRenderTarget) {
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

void DemoApp::InitRender() {
	D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
	//화면 위쪽에 사각형을 그림 .
	D2D1_RECT_F rectangle1 = D2D1::RectF(
		rtSize.width / 2 - 25.0f, 10.0f,
		rtSize.width / 2 + 25.0f, 35.0f);
	D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rectangle1, 10.0f, 10.0f);
	m_pRenderTarget->DrawRoundedRectangle(&roundedRect, m_pLightSlateGrayBrush);
	m_pRenderTarget->FillRoundedRectangle(&roundedRect, m_pLightSlateGrayBrush);
	TRACE_WIN32("사각형 그림");

	//마우스 위치 정보 등 표시
	// 캡션 텍스트를 표시함.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	_swprintf(szText, L"마우스 X:%.2f\n마우스 Y:%.2f\n회전각도:%.2f\n크기조정 인자 :%.2f\n박스개수 : %d\n",
		currentPosition.x, currentPosition.y, 0.0,0.0,0);
	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat,
		D2D1::RectF(10.0f, 10.5f, 236.0f, 190.5f), m_pBlackBrush);
	TRACE_WIN32("캡션표시");
}