#include "DemoApp.h"

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// D2D ����Ÿ���� ������.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		// ���� ���α׷� ���ҽ��κ��� ��Ʈ�� ��ü m_pBitmap�� ������.
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"background", L"Image", 200, 0, &m_pBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
		}
	}

	return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(ID2D1RenderTarget* pRenderTarget, ID2D1BitmapBrush** ppBitmapBrush)
{
	HRESULT hr = S_OK;

	// ȣȯ ����Ÿ���� ������.
	ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
	hr = pRenderTarget->CreateCompatibleRenderTarget(D2D1::SizeF(10.0f, 10.0f), &pCompatibleRenderTarget);

	if (SUCCEEDED(hr))
	{
		// ���� ������ �׸�.
		ID2D1SolidColorBrush* pGridBrush = NULL;
		hr = pCompatibleRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f), &pGridBrush);

		if (SUCCEEDED(hr))
		{
			pCompatibleRenderTarget->BeginDraw();
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
			pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.1f, 1.0f, 10.0f), pGridBrush);
			pCompatibleRenderTarget->EndDraw();

			// ����Ÿ�����κ��� ��Ʈ���� ����.
			ID2D1Bitmap* pGridBitmap = NULL;
			hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

			if (SUCCEEDED(hr))
			{
				// ��Ʈ�� ���� ������.
				hr = pRenderTarget->CreateBitmapBrush(pGridBitmap, D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), ppBitmapBrush);

				pGridBitmap->Release();
			}

			pGridBrush->Release();
		}

		pCompatibleRenderTarget->Release();
	}

	return hr;
}   //��� ����