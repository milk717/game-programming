// GdiSimpleRectangle.cpp : 단순한 윈도우 응용 프로그램 프레임워크.
//

#include "stdafx.h"
#include <tchar.h>
#define MAX_STUDENT 7

HWND hWndMain = 0;  // 메인 윈도우 핸들.

bool initWin(HINSTANCE instanceHandle, int show);
int runWin();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// 응용 프로그램 진입 함수.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	
	if (!initWin(hInstance, nCmdShow)) {
		MessageBox(0, "initWin - Failed", "Error", MB_OK);
		return FALSE;
	}

	return runWin();
}



// 초기화가 성공적으로 수행되면 true를 되돌림.
bool initWin(HINSTANCE hInstance, int nCmdShow)
{
	// WNDCLASS 구조체에 값을 채워서 생성하고자 할 윈도우의 특성을 지정함.
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "Hello";

	// 윈도우 클래스 description을 등록함.
	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass - Failed", 0, 0);
		return false;
	}

	// 등록된 윈도우 클래스 description에 기반하여 윈도우를 생성함.
	// 생성된 윈도우의 HWND를 되돌림.
	hWndMain = CreateWindow(
		"Hello", "Hello", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (!hWndMain) {
		MessageBox(0, "CreateWindow - Failed", 0, 0);
		return false;
	}

	// 생성된 윈도우를 show하고 update함.
	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	return true;
}


// 메시지 루프를 포함하는 함수.
int runWin()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 윈도우가 생성된 후 메시지 루프에 진입하고 WM_QUIT 메시지가 올 때까지 메시지 루프를 반복함.

	// GetMessage 함수는 WM_QUIT 메시지를 받을 때만 0을 되돌림. 이때 루프를 벗어나도록 함.
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);  //윈도우가 생성한 가상키 코드를 문자메시지로 해석함
		DispatchMessage(&msg);  //메시지를 window procedure로 보냄
	}
	return (int)msg.wParam;
}

// 윈도우가 받는 이벤트들을 처리하는 윈도우 프로시져.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		//왼쪽 마우스 눌렸을 때
	case WM_KEYDOWN:
		// ESC 키가 눌려지면, 윈도우를 destroy함.
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWndMain);
		return 0;
	case WM_DESTROY:
		// destroy 메시지를 받으면 quit 메시지를 보내서 메시지 루프를 벗어나도록 함.
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		break;

	default:
		// 그 외의 메시지들은 디폴트 윈도우 프로시져로 forward함.
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
