#include "DrawRectangle.h"

DemoApp::DemoApp():	//�����ڿ��� Ŭ���� ���� NULL�� �ʱ�ȭ
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL)
{
}

DemoApp::~DemoApp() {
	DiscardDeviceResources();	//��� ��ġ ������ �ڿ��� �ݳ��ϴ� �Լ�
	SAFE_RELEASE(m_pDirect2dFactory);	//��ġ ������ �ڿ��� d2d ���丮�� �ʿ����� �����Ƿ� �ݳ�
}

void DemoApp::RunMessageLoop() {
	MSG msg;

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//������ �ʱ�ȭ
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

//RunMessageLoop�� ���������� ȣ��
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

//��ġ ������ �ڿ����� ����
HRESULT DemoApp::CreateDeviceResources() {
	HRESULT hr = S_OK;

	if (!m_pRenderTarget) {	//����Ÿ���� �̹� ��ȿ�� �� ���� x
		RECT rc; GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		//����Ÿ�� ����
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget);
		if (SUCCEEDED(hr)) {
			//ȸ�� ��
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}
		if (SUCCEEDED(hr)) {
			//�Ķ��� ��
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
	}
	return hr;
}

//��� ��ġ ������ �ڿ��� �ݳ��ϴ� �Լ�. �Ҹ��ڿ��� �ҷ���.
void DemoApp::DiscardDeviceResources() {
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
}

//������ �޽��� ó���� ���� �Լ�
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
				{ UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
				}
				result = 0; wasHandled = true; break;
				case WM_DISPLAYCHANGE :
				{ InvalidateRect(hWnd, NULL, FALSE);
				}
				result = 0; wasHandled = true; break;
				case WM_PAINT:
				{ pDemoApp->OnRender();
				ValidateRect(hWnd, NULL);
				}
				result = 0; wasHandled = true; break;
				case WM_DESTROY:
				{ PostQuitMessage(0);
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
	HRESULT hr = S_OK;
	hr = CreateDeviceResources();
	if (SUCCEEDED(hr)) { // ����Ÿ���� ��ȿ��. 
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		/* �׸��� �Լ����� ȣ��... �� �κ��� ���� �ʿ� ���� */
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
			//ȭ�� �߰��� �� �簢���� �׸� .
			D2D1_RECT_F rectangle1 = D2D1::RectF(
				rtSize.width / 2 - 50.0f, rtSize.height / 2 - 50.0f,
				rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f);
			D2D1_RECT_F rectangle2 = D2D1::RectF(
				rtSize.width / 2 - 100.0f, rtSize.height / 2 - 100.0f,
				rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f);
			m_pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush);
			m_pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush);

			hr = m_pRenderTarget->EndDraw();
		}
		if (hr == D2DERR_RECREATE_TARGET) { // ����Ÿ���� ������ؾ� �� . 
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