#include "DemoApp.h"

/*
* 배경그리기, 캐릭터그리기 등 Object들을 그리기 위한 함수들
*/

void DemoApp::DrawBackground(D2D1_SIZE_F renderTargetSize) {

	// 격자 패턴으로 배경을 칠함.
	m_pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, renderTargetSize.width, renderTargetSize.height), m_pGridPatternBitmapBrush);
	// 비트맵 m_pBitmap의 크기를 얻음.
	D2D1_SIZE_F size = m_pBitmap->GetSize();

	//배경 그리기
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);
	// 비트맵 m_pBitmap을 그림.
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