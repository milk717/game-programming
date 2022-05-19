#include "DemoApp.h"

void DemoApp::ScoreCountStart()
{
	if(this->isStart)
	{
		this->score = clock()-startTime;
	}
}
