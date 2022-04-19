#include "Student.h"

string Student::getName() {
	srand((unsigned int)time(NULL));
	int randomNum = rand() % 15;
	string name = firstName[randomNum]; // 이름을 랜덤하게 만들어냄 
	for (int i = 0; i < 2; i++) { // 이름 3글자까지만
		int randomNum = rand() % 15;
		name = name + lastName[randomNum];
	}
	return name;
}
int Student::getScore() {
	int randomNum = rand() % (91 - 30) + 30;	//성적은 30~90
	return randomNum;
}