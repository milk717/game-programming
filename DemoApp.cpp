#include "DemoApp.h"

#include <tchar.h>
#include <time.h>
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

bool isStart = false;
bool isJumpClick = false;
D2D1_POINT_2F charPoint = {30,400};
int score = 0;
double spaceTime;
double temp = 0;	//��ǥ �ӽ�����
double chx = 0;
double chy = 0;
double jump;

/* ���� ���콺 ��ġ ��ǥ */
D2D_POINT_2F currentMousePosition;

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pWICFactory(NULL),
	m_pPathGeometry(NULL),
	m_pObjectGeometry(NULL),
	m_pRedBrush(NULL),
	m_pYellowBrush(NULL),

	//��Ʈ��
	m_pBitmap(NULL),
	m_pCharactorBitmap(NULL),
	m_pGameoverBitmap(NULL),

	//��Ʈ�� �귯��
	m_pBackgroundBitmapBrush(NULL),
	m_pGameoverBitmapBrush(NULL),
	m_pCharactorBitmapBrush(NULL),

	// Text
	m_pDWriteFactory(NULL),
	m_pTextBrush(NULL),
	m_pTextFormat(NULL),
	m_score_TextFormat(NULL),

	//�ִϸ��̼�
	m_Animation()
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pPathGeometry);
	SAFE_RELEASE(m_pObjectGeometry);
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pYellowBrush);

	SAFE_RELEASE(m_pBitmap);

	//��Ʈ�� �귯��
	SAFE_RELEASE(m_pGameoverBitmapBrush);
	SAFE_RELEASE(m_pCharactorBitmapBrush);
	SAFE_RELEASE(m_pBackgroundBitmapBrush);

	// Text
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_score_TextFormat);

	//��Ʈ��
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pCharactorBitmap);
	SAFE_RELEASE(m_pGameoverBitmap);
}

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	//register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DemoApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D2DDemoApp";
	RegisterClassEx(&wcex);

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Direct2D Demo Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			960, 720, NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pPathGeometry->ComputeLength(NULL, &length);

			if (SUCCEEDED(hr))
			{
				m_Animation.SetStart(0); //start at beginning of path
				m_Animation.SetEnd(length); //length at end of path
				m_Animation.SetDuration(3.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}
	}

	QueryPerformanceFrequency(&m_nFrequency);
	QueryPerformanceCounter(&m_nPrevTime);

	return hr;
}

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
		m_pTextBrush
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

	// ������ ����� ��� ���ϸ� ������.
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

	// ������ �簢�� ����� ��� ���ϸ� ������.
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

	return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU((rc.right - rc.left), (rc.bottom - rc.top));

		// D2D ����Ÿ���� ������.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		// ������ ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
		}
		// ����� ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
		}
		// ������ ���� ������.
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTextBrush);
		}
		/*
		* ��Ʈ�� ��ü ����
		*/
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"background", L"Image", size.width, size.height, &m_pBitmap);
		}
		if (SUCCEEDED(hr)) {
			hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"charactor", L"Image", 100, 100, &m_pCharactorBitmap);
		}
		if (SUCCEEDED(hr)) {
			hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"gameover", L"Image", size.width, size.height, &m_pGameoverBitmap);
		}

		/*
		* ��Ʈ�� �귯�� ����
		*/
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pGameoverBitmap, &m_pGameoverBitmapBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pBitmap, &m_pBackgroundBitmapBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateBitmapBrush(m_pCharactorBitmap, &m_pCharactorBitmapBrush);
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pYellowBrush);
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pCharactorBitmap);
}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	//ĳ���� �̵� �ִϸ��̼�
	static float charAnimationTime = 0.0F;
	static float anim_time = 0.0f;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		D2D1_MATRIX_3X2_F triangleMatrix;
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 960;

		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(rtSize.width / 2, rtSize.height / 2);

		// �׸��⸦ �غ���.
		m_pRenderTarget->BeginDraw();

		// ��ȯ�� �׵���ķ� ������.
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// ����Ÿ���� Ŭ������.
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		//��� �׸���
		m_pRenderTarget->FillRectangle(&D2D1::RectF(0, 0, rtSize.width, rtSize.height), m_pBackgroundBitmapBrush);

		WriteActionInfo();	//���콺 ��ǥ ���� ���̵���

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
				isJumpClick = false;
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
			anim_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			anim_time += elapsedTime;
			charAnimationTime += elapsedTime;
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
				score++;
				if (!isStart) {	//���� ������ ��� ��������
					isStart = true;
					isJumpClick = true;
				}
				else if (isStart && !isJumpClick) {	//�����߰� ���� ���� ���¿��� Ű ������ ��
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

// Creates a Direct2D bitmap from a resource in the application resource file.
HRESULT DemoApp::LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
					);
				}

				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
					);
				}
			}
		}
		else
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			);
		}
	}

	if (SUCCEEDED(hr))
	{
		//create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}