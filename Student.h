#pragma once
#include <string>
#include <ctime>
#include <random>
#include <cstdlib>
using namespace std;
class Student {
private:
	string firstName[16] = { "��","��","��","��","��","��","��","��","��","��","��","��","��","��","��" };
	string lastName[16] = { "��","��","��","��","��","��","��","��","��","��","��","��","��","��","��" };
public:
	string getName();
	int getScore();
};