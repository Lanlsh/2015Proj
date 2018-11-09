#pragma once
#include<windows.h>
class LanEvent
{
public:
	LanEvent();
	~LanEvent();

	//线程等待时间触发信号
	void Wait();

	//触发线程事件信号
	void Signal();

private:
	HANDLE	m_hEvent;
};

