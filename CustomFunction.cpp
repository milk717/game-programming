#include "DemoApp.h"

//���� ī��Ʈ�ϴ� �Լ�
void DemoApp::ScoreCountStart()
{
	if(this->isStart)
	{
		this->score = (clock()-startTime);
	}
}

//��ӵ� ��������� ���� ����ϴ� �Լ�
float DemoApp::Acceleration()
{
	return 0.5+score*0.000005;		//������ ���������� ���ӵ� ����

}