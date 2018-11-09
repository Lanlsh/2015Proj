#include "stdafx.h"
#include "LanEvent.h"


LanEvent::LanEvent()
{
	/*
	//第二个参数：如果是true， 则此时即使WaitForSingleObject函数返回，其句柄也不会变为non-signaled状态，只能通过一下两个函数改变状态
	BOOL ResetEvent(HANDLE hEvent);	//to the non-signaled
	BOOL SetEvent(HANDLE hEvent);	//to the signaled
	*/

	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hEvent)
	{
		cout << "创建信号量失败！！！" << endl;
	}
}


LanEvent::~LanEvent()
{
	if (m_hEvent)
	{
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

void LanEvent::Wait()
{
	::WaitForSingleObject(m_hEvent, INFINITE);
	::ResetEvent(m_hEvent);
}

void LanEvent::Signal()
{
	if (m_hEvent)
	{
		::SetEvent(m_hEvent);
	}
}
