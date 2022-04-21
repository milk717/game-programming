#include "DrawRectangle.h"
#include "Student.h"
#include <tchar.h>
#include <vector>
#include <string>
#include <utility>

#define MAX_SIZE 7

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

using namespace std;

//전역변수 선언

//캡션정보
D2D_POINT_2F currentPosition; // 현재 움직이고 있는 마우스 좌표
D2D_POINT_2F clickPosition;	//마지막으로 클릭된 마우스 좌표
float Angle = 0; // 각도 조절 인자
float Size = 0; // 사이즈 조절 인자 

//좌표
int centerX, centerY;

//상자
D2D1_RECT_F topRectangle;	//맨위에 회색상자
D2D1_ROUNDED_RECT topRoundedRect;	//회색상자 라운드
RECT captionRect;

D2D1_SIZE_F rtSize; //렌더타겟 사이즈

//현재 삽입중인지 삭제중인지 판별
boolean pushingFlag = false;
boolean pushedFlag = false;
boolean popingFlag = false;
boolean popedFlag = false;

//학생 정보 저장할 벡터
typedef pair<string, int> student;
vector <student> studentList; //학생 데이터가 담길 벡터
int studentListSize = 0;	//현재 벡터에 담긴 학생 수


//함수선언
D2D1_RECT_F getTopStackPosition(int stackSize);
void dataPush();

DemoApp::DemoApp():	//생성자에서 클래스 변수 NULL로 초기화
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pBlackBrush(NULL),
	m_pTextFormat(NULL),
	m_pDWriteFactory(NULL),
	m_RectangleBrush(NULL)
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
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Direct2D Demo Application", 
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
		if (SUCCEEDED(hr)) {
			//보라색 붓
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::BlueViolet), &m_RectangleBrush);
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
	SAFE_RELEASE(m_RectangleBrush);
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
				case WM_LBUTTONUP: //드래그가 끊겼을 때
				{
					pushingFlag = false; 
					popingFlag = false;
					Angle = 0; // Angle,Size값 초기화
					Size = 0;
					break;

				}
				result = 0; wasHandled = true; break;
				case WM_LBUTTONDOWN:
				{
					if(LOWORD(lParam) >= topRectangle.left && LOWORD(lParam) <= topRectangle.right
						&& HIWORD(lParam) >= topRectangle.top && HIWORD(lParam) <= topRectangle.bottom) { // 만약 상단의 가운데 사각형이 눌려졌으면 
						pushingFlag = true; // 현재 삽입을 시작했다는 것을 나타낸다.
						clickPosition.x = LOWORD(lParam);
						clickPosition.y = HIWORD(lParam);
						TRACE_WIN32(L"X = %.2f, Y = %.2f\n", clickPosition.x, clickPosition.y);
						TRACE_WIN32(L"pushing student start\n");
					}
					else {
						/*
						* top위치에 있는 스택이 눌리면 삭제모드 시작
						* 그럼 클릭된곳이 top위치인지 알아내야함
						* top위치 좌표는 스택의 크기로 알 수 있음.
						* 그럼 스택먼저 구현하자
						*/
						if (studentListSize == 0) {
							break;
						}
						D2D1_RECT_F topRect = getTopStackPosition(studentListSize-1);	//새로 추가될 상자의 위치가 아니라 맨위 상자니까 사이즈 하나 줄여서 전달
						if (LOWORD(lParam) >= topRect.left && LOWORD(lParam) <= topRect.right
							&& HIWORD(lParam) >= topRect.top && HIWORD(lParam) <= topRect.bottom) {  // 삭제모드인채로 상단 회색 박스까지 드래그되었을 경우
							popingFlag = true; // 현재 삭제를 시작했다는 것을 나타낸다.
							clickPosition.x = LOWORD(lParam);
							clickPosition.y = HIWORD(lParam);
							TRACE_WIN32(L"X = %.2f, Y = %.2f\n", clickPosition.x, clickPosition.y);
							TRACE_WIN32(L"poping student start\n");
						}
						
					}
					break;
				}
				case WM_MOUSEMOVE:	//마우스 움직일 때: 좌표정보 갱신, 상자 추가, 삭제일 때 구현
				{
					currentPosition.x = LOWORD(lParam);
					currentPosition.y = HIWORD(lParam);
					//TRACE_WIN32("X = %.2f, Y = %.2f\n",currentPosition.x,currentPosition.y);
					InvalidateRect(hWnd, &captionRect, false);	//캡션 영역만큼 화면 갱신
				
					if (pushingFlag) {	//삽입모드일 때(맨위 회색상자에서 클릭되었고 아직 마우스 버튼이 떼지기 전)
						if (studentListSize >= MAX_SIZE) {	//최대 사이즈보다 더 크면
							pushingFlag = false; // pushingFlag를 false로 처리해 삽입모드를 벗어난다.
							::MessageBox(0, L"the number of box is 7 , it's limit", L"Fatal Error", MB_OK); //에러 메시지 띄워주기
							::MessageBeep(MB_OK);
							break;
						}
						D2D1_RECT_F topRect = getTopStackPosition(studentListSize);		//맨위 스택의 위치정보 가져옴.
						//TRACE_WIN32("get top stack position (%f, %f)\n(%f, %f)\n",topStackRect.left,topStackRect.top,topStackRect.right,topStackRect.bottom);
						if (LOWORD(lParam) >= topRect.left && LOWORD(lParam) <= topRect.right
							&& HIWORD(lParam) >= topRect.top && HIWORD(lParam) <= topRect.bottom) { // 삽입모드인 채로 맨 위쪽으로 드래그되었을 경우
							pushedFlag = true; //삽입이 완료되었다고 처리
							Angle = 0; //Angle, Size 초기화 
							Size = 0;
							pushingFlag = false;
							TRACE_WIN32(L"pushing student complete\n");
						}
						InvalidateRect(hWnd, NULL, false);
					}
					else if (popingFlag) {
						if (LOWORD(lParam) >= topRectangle.left && LOWORD(lParam) <= topRectangle.right
							&& HIWORD(lParam) >= topRectangle.top && HIWORD(lParam) <= topRectangle.bottom) {  // 삭제모드인채로 상단 회색 박스까지 드래그되었을 경우
							popedFlag = true; //삭제가 완료되었다고 처리
							Angle = 0; //Angle,Size 초기화 
							Size = 0;
							popingFlag = false;
							TRACE_WIN32(L"poping student complete\n");
						}
						InvalidateRect(hWnd, NULL, false);
					}
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
bool cmp(student a, student b) {
	return a.second > b.second;
}
HRESULT DemoApp::OnRender()
{
	/*
	✔️회색 사각형 그려주기(높이:너비 = 1:4)
	✔️마우스좌표, 
	회전각, 벡터사이즈 등 표시하는 캡션 텍스트 그리기
	마우스 왼쪽버튼 눌리면
	회색박스 안에서 눌리면 학생추가
	스택 꼭대기에서 눌리면 학생삭제
	(일단은 상자안에 글씨말고 상자만 그려보자)
	*/
	HRESULT hr = S_OK;
	hr = CreateDeviceResources();

	// 렌더타겟 변환을 항등 변환으로 리셋함.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	if (SUCCEEDED(hr)) { // 렌더타겟이 유효함. 
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		rtSize = m_pRenderTarget->GetSize();
		
		InitRender();	//화면 위쪽에 사각형 그리고 글자표시 격자
		//studentList를 그려주기 전에 성적순으로 정렬하기 (맨위가 젤 낮은성적)
		sort(studentList.begin(), studentList.end(),cmp);
		//현재 벡터 그려주기
		for (int i = 0; i < studentListSize; i++) { // studentList에 저장된 스택을 그려준다. 
			if (popingFlag == true || popedFlag == true) {	//삭제모드일때는 
				if (i + 1 == studentListSize) break;	//맨위스택 안그려줌
			}
			topRoundedRect = D2D1::RoundedRect(getTopStackPosition(i), 10.0f, 10.0f);
			m_pRenderTarget->DrawRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush);
			m_pRenderTarget->FillRoundedRectangle(topRoundedRect, m_RectangleBrush); //사각형의 예비 위치를 얻어내서 사각형을 그려줌
			const char* multiByte = studentList[i].first.c_str(); // studentList.first == name 을 const char*형으로 바꾼다. 
			TCHAR temp[15]; // const char*을 TCHAR형으로 바꿔야 하기 때문에 temp변수를 선언한다. 
			memset(temp, 0, sizeof(temp));
			MultiByteToWideChar(CP_ACP, MB_COMPOSITE, multiByte, -1, temp, 15);
			static WCHAR szText[100]; // name과 score을 한번에 넣어아 햐므로 WCHAR 배열형 변수를 선언한다. 
			swprintf_s(szText, L"%s %d\n", temp, studentList[i].second);	//상자에 텍스트 표시하기 위한 문자열
			m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat, getTopStackPosition(i), m_pBlackBrush); // Text를 그려준다.
			m_pRenderTarget->DrawRoundedRectangle(topRoundedRect, m_pBlackBrush); // 상자 테두리를 그려준다. 
		}
		//삽입되고있는 과정. 애니메이션 처리해야함
		if (pushingFlag) {
			drawPusingBoxAnimation();
			m_pCornflowerBlueBrush->SetOpacity(1.0f);	//투명도 조절
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());	//이거 안하면 가이드라인 돌아감.
			topRoundedRect = D2D1::RoundedRect(getTopStackPosition(studentListSize), 10.0f, 10.0f);	//가이드라인 상자
			m_pRenderTarget->DrawRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush);
		}
		//삭제되고 있는 과정. 애니메이션 처리해야함
		if (popingFlag) {
			drawPopingBoxAnimaion();
		}
		//맨위 회색 상자에서 드래그 시작해서 스택 맨위에서 드래그 멈췄을 때
		if (pushedFlag == true) { // 삽입 완료됬으면 
			studentListSize++; // 스택사이즈를 늘려준다. 
			dataPush(); // 학생 정보를 studentList에 넣어준다
			pushedFlag = false;	//삽입 완료모드 벗어남
		}
		//리스트 맨위 상자에서 드래그 시작해서 맨위 회색 상자에서 드래그 멈췄을 때
		if (popedFlag == true) { // 삭제되었으면
			studentListSize--; //stacksize를 줄여준다.
			studentList.pop_back(); // studentList에서 값을 빼온다
			popedFlag = false;		//삭제 완료모드 벗어남
		}
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
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);
	centerX = width / 2;
	centerY = height / 2;
	m_pLightSlateGrayBrush->SetOpacity(0.5f);
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
	//화면 위쪽에 사각형을 그림 .
	topRectangle = D2D1::RectF(
		centerX - 25.0f, 10.0f,
		centerX + 25.0f, 35.0f);
	topRoundedRect = D2D1::RoundedRect(topRectangle, 10.0f, 10.0f);
	m_pRenderTarget->DrawRoundedRectangle(&topRoundedRect, m_pLightSlateGrayBrush);
	m_pRenderTarget->FillRoundedRectangle(&topRoundedRect, m_pLightSlateGrayBrush);

	//마우스 위치 정보 등 표시
	// 캡션 텍스트를 표시함.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	WCHAR szText[100];
	_swprintf(szText, L"마우스 X:%.2f\n마우스 Y:%.2f\n회전각도:%.2f\n크기조정 인자 :%.2f\n박스개수 : %d\n",
		currentPosition.x, currentPosition.y, Angle, Size, studentListSize);
	m_pRenderTarget->DrawText(szText, wcslen(szText), m_pTextFormat,
		D2D1::RectF(10.0f, 10.5f, 236.0f, 190.5f), m_pBlackBrush);
	captionRect.left = 10.0;
	captionRect.top = 10.5;
	captionRect.right = 236.0;
	captionRect.bottom = 190.5;
}
//상자 삽입 애니메이션
void DemoApp::drawPusingBoxAnimation() {
	D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(currentPosition.x - clickPosition.x, currentPosition.y - clickPosition.y); //가장 최근에 눌린 곳의 좌표부터 현재 마우스 커서의 위치까지 이동
	Size = ((currentPosition.y - clickPosition.y) / (getTopStackPosition(studentListSize).top - 30)) + 1; // 목표 지점을 기준으로 size구함
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(topRectangle.left, topRectangle.top));
	Angle = (currentPosition.y - clickPosition.y) / (getTopStackPosition(studentListSize).top - 30) * -360; // 목표지점까지 기준으로 회전각 구함. 반시계방향이니까 -360
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F((topRectangle.right + topRectangle.left) / 2, (topRectangle.top + topRectangle.bottom) / 2));
	m_pRenderTarget->SetTransform(scale * rotation * translation); // scale->rotation->translation 순

	m_pCornflowerBlueBrush->SetOpacity(0.5f);	//투명도 조절
	topRoundedRect = D2D1::RoundedRect(topRectangle, 10.0f, 10.0f);
	m_pRenderTarget->DrawRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush);
	m_pRenderTarget->FillRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush); //topRectangle을 활용해서 모서리둥근사각형으로 바꿔서 그려줌
}
//상자 삭제 애니메이션
void DemoApp::drawPopingBoxAnimaion() {
	studentListSize--;
	D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(currentPosition.x - clickPosition.x, currentPosition.y - clickPosition.y); //가장 최근에 눌린 곳의 좌표부터 현재 마우스 커서의 위치까지 이동
	if (currentPosition.y <= clickPosition.y) //스택리스트 위쪽으로 드래그
		Size = (getTopStackPosition(studentListSize).top - 30) / ((clickPosition.y - currentPosition.y)  + (getTopStackPosition(studentListSize).top - 30)); // 목표 지점을 기준으로 size구함
	else //스택리스트쪽으로 드래그
		Size = ((currentPosition.y - clickPosition.y) / (getTopStackPosition(studentListSize).top - 30))  + 1; //pushing할때 공식 재사용
	D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(Size, 1.0f), D2D1::Point2F(clickPosition.x, clickPosition.y));
	Angle = (clickPosition.y - currentPosition.y) / (getTopStackPosition(studentListSize).top - 30) * 360 * 1; //회전각 구함
	D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(Angle, D2D1::Point2F(clickPosition.x, clickPosition.y));
	m_pRenderTarget->SetTransform(scale * rotation * translation);
	m_pCornflowerBlueBrush->SetOpacity(0.5f);	//투명도 조절
	topRoundedRect = D2D1::RoundedRect(getTopStackPosition(studentListSize), 10.0f, 10.0f);	// 가장 위쪽 스택을 기준으로 그려줌
	m_pRenderTarget->DrawRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush);
	m_pRenderTarget->FillRoundedRectangle(&topRoundedRect, m_pCornflowerBlueBrush); 
	studentListSize++;
}

D2D1_RECT_F getTopStackPosition(int stackSize) {	//맨위 추가될 상자의 위치 정보를 계산한는 함수.
	float top = rtSize.height - ((stackSize +1) * 30.0f);
	return D2D1::RectF(centerX - 50, top, centerX + 50, top + 30);
}

void dataPush() {
	Student st = Student();
	studentList.push_back(student(st.getName(), st.getScore()));
}