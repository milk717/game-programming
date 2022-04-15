#pragma once
#include "stdafx.h"
#include "Student.h"
#include <vector>
using namespace std;
class StudentVector {
private:
	vector<Student> studentList;
	vector<Student>::iterator iter;
	int size;
public:
	StudentVector();
	void sortStudentList();
	bool deleteTopStudent();
	bool addStudent();
	LRESULT drawStudentList(HWND hwnd);
	bool isBoxClicked(HWND hWnd, int x, int y);
};