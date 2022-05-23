#include "DemoApp.h"

//점수 카운트하는 함수
void DemoApp::ScoreCountStart()
{
	if(this->isStart)
	{
		this->score = (clock()-startTime);
	}
}

//등가속도 직선운동에서 변위 계산하는 함수
float DemoApp::Acceleration()
{
	return 0.5+score*0.000005;		//점수가 높아질수록 가속도 증가

}