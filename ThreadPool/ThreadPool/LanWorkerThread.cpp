#include "stdafx.h"
#include "LanWorkerThread.h"
#include <process.h>	//for _beginthreadex

LanWorkerThread::LanWorkerThread()
{
	m_bIsStop = false;
	m_bRunState = false;
	m_task = NULL;

	m_hdlThread = (HANDLE)_beginthreadex(NULL, 0, &LanWorkerThread::ThreadExec, this, CREATE_SUSPENDED, NULL);	//��5������ 0�� ��������  CREATE_SUSPENDED: ���ң����� //Ҳ����ʹ��SuspendThread�����������߳�
	if (m_hdlThread == 0)
	{
		cout << "�����߳�ʧ�ܣ�����" << endl;
	}
}


LanWorkerThread::~LanWorkerThread()
{
	if (m_hdlThread != 0)
	{
		//WaitForSingleObject�����������hHandle�¼����ź�״̬����ĳһ�߳��е��øú���ʱ���߳���ʱ��������ڹ����dwMilliseconds�����ڣ�
		//�߳����ȴ��Ķ����Ϊ���ź�״̬����ú����������أ������ʱʱ���Ѿ�����dwMilliseconds���룬��hHandle��ָ��Ķ���û�б�����ź�״̬�������������ء�
		//WaitForSingleObject(m_hdlThread, INFINITE);
		WaitForSingleObject(m_hdlThread, INFINITE);
		cout << "�߳�ID��" << GetThreadId(m_hdlThread) << "�˳�!!!" << endl;
		CloseHandle(m_hdlThread);
	}

}

void LanWorkerThread::Run()
{
	if (m_hdlThread)
	{
		//�����߳�
		ResumeThread(m_hdlThread);
	}
	else
	{
		cout << "�߳�HANDLE���󣡣���";
	}
}

void LanWorkerThread::Stop()
{
	m_bIsStop = true;
	m_eventCondition.Signal();
}

unsigned __stdcall  LanWorkerThread::ThreadExec(void* pThis)
{
	LanWorkerThread* pWorker = (LanWorkerThread*)pThis;
	pWorker->DoExec();
	
	return 1;
}

void LanWorkerThread::DoExec()
{
	while (!m_bIsStop)
	{
		m_eventCondition.Wait();

		if (m_task)
		{
			//m_bRunState = true;
			//cout <<endl << "�߳�ID��" << GetThreadId(m_hdlThread) << "  task ------start" << endl;
			m_task();
			//cout <<endl << "�߳�ID��" << GetThreadId(m_hdlThread) << "  task ------end" << endl;
			m_task = NULL;
		}

		m_bRunState = false;
	}

	cout << "�߳�ID��" << GetThreadId(m_hdlThread) << "ִ����ϣ���" << endl;
}

void LanWorkerThread::SetTask(Task task)
{
	if (task)
	{
		m_task = task;
		m_bRunState = true;
		m_eventCondition.Signal();
	}
	
}
