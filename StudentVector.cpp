#pragma once
#include "stdafx.h"
#include "StudentVector.h"
#include "Student.h"
#include <vector>
#include <random>
#include <algorithm>
#include <tchar.h>
using namespace std;

StudentVector::StudentVector() {
	this->size = 0;
}
void StudentVector::sortStudentList() {
	//sort(this->studentList.begin(), this->studentList.end());
}
bool StudentVector::deleteTopStudent() {
	if (this->size == 0) {
		return false;
	}
	this->studentList.pop_back();
	return true;
}
bool StudentVector::addStudent() {
	if (this->size > 6) {
		return false;
	}
	random_device rd;	//�õ尪 ��� ���� random_device ����
	mt19937 gen(rd());	//���� ���� ���� �ʱ�ȭ
	uniform_int_distribution<int> dis_name(0, 16);	//�յ���� ����
	uniform_int_distribution<int> dis_score(30, 90);	//����	

	string firstNames[] = { TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),
								TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��") };
	string lastNames[] = { TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),
								TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��") };

	//���� �̸� ����
	string name = firstNames[dis_name(gen)];
	name += lastNames[dis_name(gen)];
	name += lastNames[dis_name(gen)];

	//���� ���� ����
	int score = dis_name(gen);

	Student newStudent = Student(name, score);
	this->studentList.push_back(newStudent);
	sortStudentList();
	return true;
}
LRESULT StudentVector::drawStudentList(HWND hWnd) {
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	RECT rc; GetClientRect(hWnd, &rc);  // �׸��� ������ ũ�⸦ ����

	HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0)); // ���� ������
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyRedPen); // ���� ������

	Rectangle(ps.hdc, rc.left + 100, rc.bottom - 100*(this->size+1), rc.right - 100, rc.bottom - 100* (this->size + 2)); // �簢���� �׸�

	SelectObject(ps.hdc, hOldPen); // ���� ��ü�� ������
	DeleteObject(hMyRedPen);


	const TCHAR str[] = "�ȳ��ϼ���!";
	TextOut(ps.hdc, (rc.right - rc.left) / 2 - 40, (rc.bottom - rc.top) / 2 - 10, str, lstrlen(str));

	EndPaint(hWnd, &ps);

	return 0;
}
bool StudentVector::isBoxClicked(HWND hWnd,int x, int y) {
	RECT rc; GetClientRect(hWnd, &rc);  // �׸��� ������ ũ�⸦ ����
	TRACE("%d %d\n", (rc.left + 100, rc.right - 100));
	TRACE("%d %d\n", (rc.bottom - 100 * (this->size + 1), rc.bottom - 100 * (this->size + 2)));
	if (rc.left + 100 <= x && x <= rc.right - 100) {
		if (rc.bottom - 100 * (this->size + 1) <= y && rc.bottom - 100 * (this->size + 2)) {
			return true;
		}
	}
	return false;
}
#if defined(_DEBUG)&&defined(WIN32)&&!defined(_AFX)&&!defined(_AFXDLL)
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
