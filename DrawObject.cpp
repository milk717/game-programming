#include "DemoApp.h"

/*
* ���׸���, ĳ���ͱ׸��� �� Object���� �׸��� ���� �Լ���
*/

void DemoApp::DrawBackground(D2D1_SIZE_F renderTargetSize) {

	// ���� �������� ����� ĥ��.
	m_pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, renderTargetSize.width, renderTargetSize.height), m_pGridPatternBitmapBrush);
	// ��Ʈ�� m_pBitmap�� ũ�⸦ ����.
	D2D1_SIZE_F size = m_pBitmap->GetSize();

	//��� �׸���
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);
	// ��Ʈ�� m_pBitmap�� �׸�.
	m_pRenderTarget->DrawBitmap(
		m_pBitmap,
		D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + renderTargetSize.width,
			upperLeftCorner.y + renderTargetSize.height),
		0.5,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
	);
}