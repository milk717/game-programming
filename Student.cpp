#include "Student.h"

string Student::getName() {
	srand((unsigned int)time(NULL));
	int randomNum = rand() % 15;
	string name = firstName[randomNum]; // �̸��� �����ϰ� ���� 
	for (int i = 0; i < 2; i++) { // �̸� 3���ڱ�����
		int randomNum = rand() % 15;
		name = name + lastName[randomNum];
	}
	return name;
}
int Student::getScore() {
	int randomNum = rand() % (91 - 30) + 30;	//������ 30~90
	return randomNum;
}