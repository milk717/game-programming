#pragma once
#include <string>
#include <ctime>
#include <random>
#include <cstdlib>
using namespace std;
class Student {
private:
	string firstName[16] = { "김","이","박","최","송","장","안","임","연","지","서","권","함","정","하" };
	string lastName[16] = { "수","민","준","영","윤","은","지","유","연","희","서","권","진","정","하" };
public:
	string getName();
	int getScore();
};