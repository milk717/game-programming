#include "Student.h"

string Student::getName() {
	srand((unsigned int)time(NULL));
	int randomNum = rand() % 15;
	string name = firstName[randomNum]; // �̸��� �����ϰ� ���� 
	for (int i = 0; i < 3; i++) { // �̸� 3���ڱ�����
		name = name + lastName[randomNum];
	}
	return name;
}
int Student::getScore() {
	int randomNum = rand() % (91 - 30) + 30;	//������ 30~90
	return randomNum;
}