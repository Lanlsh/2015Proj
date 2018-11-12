#include "stdafx.h"
#include "LanEvent.h"


LanEvent::LanEvent()
{
	/*
	//�ڶ��������������true�� ���ʱ��ʹWaitForSingleObject�������أ�����Ҳ�����Ϊnon-signaled״̬��ֻ��ͨ��һ�����������ı�״̬
	BOOL ResetEvent(HANDLE hEvent);	//to the non-signaled
	BOOL SetEvent(HANDLE hEvent);	//to the signaled
	*/

	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hEvent)
	{
		cout << "�����ź���ʧ�ܣ�����" << endl;
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
