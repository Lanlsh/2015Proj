#pragma once
#include<windows.h>
class LanEvent
{
public:
	LanEvent();
	~LanEvent();

	//�̵߳ȴ�ʱ�䴥���ź�
	void Wait();

	//�����߳��¼��ź�
	void Signal();

private:
	HANDLE	m_hEvent;
};

