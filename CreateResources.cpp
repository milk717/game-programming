#include "DemoApp.h"

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU((rc.right - rc.left), (rc.bottom - rc.top));

		// D2D ·»´õÅ¸°ÙÀ» »ý¼ºÇÔ.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		// ºÓÀº»ö º×À» »ý¼ºÇÔ.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
		}
		// ³ë¶õ»ö º×À» »ý¼ºÇÔ.
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
		}
		// Èò»ö º×À» »ý¼ºÇÔ.
		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTextBrush);
		}
		/*
		* ºñÆ®¸Ê °´Ã¼ »ý¼º
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
		* ºñÆ®¸Ê ºê·¯½¬ »ý¼º
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