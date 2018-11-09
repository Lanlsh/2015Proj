#include "stdafx.h"
#include "LanWorkerThread.h"
#include <process.h>	//for _beginthreadex

LanWorkerThread::LanWorkerThread()
{
	m_bIsStop = false;
	m_bRunState = false;
	m_task = NULL;

	m_hdlThread = (HANDLE)_beginthreadex(NULL, 0, &LanWorkerThread::ThreadExec, this, CREATE_SUSPENDED, NULL);	//第5个参数 0： 立即运行  CREATE_SUSPENDED: 悬挂（挂起） //也可以使用SuspendThread函数来挂起线程
	if (m_hdlThread == 0)
	{
		cout << "创建线程失败！！！" << endl;
	}
}


LanWorkerThread::~LanWorkerThread()
{
	if (m_hdlThread != 0)
	{
		//WaitForSingleObject函数用来检测hHandle事件的信号状态，在某一线程中调用该函数时，线程暂时挂起，如果在挂起的dwMilliseconds毫秒内，
		//线程所等待的对象变为有信号状态，则该函数立即返回；如果超时时间已经到达dwMilliseconds毫秒，但hHandle所指向的对象还没有变成有信号状态，函数照样返回。
		//WaitForSingleObject(m_hdlThread, INFINITE);
		WaitForSingleObject(m_hdlThread, INFINITE);
		cout << "线程ID：" << GetThreadId(m_hdlThread) << "退出!!!" << endl;
		CloseHandle(m_hdlThread);
	}

}

void LanWorkerThread::Run()
{
	if (m_hdlThread)
	{
		//唤醒线程
		ResumeThread(m_hdlThread);
	}
	else
	{
		cout << "线程HANDLE错误！！！";
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
			//cout <<endl << "线程ID：" << GetThreadId(m_hdlThread) << "  task ------start" << endl;
			m_task();
			//cout <<endl << "线程ID：" << GetThreadId(m_hdlThread) << "  task ------end" << endl;
			m_task = NULL;
		}

		m_bRunState = false;
	}

	cout << "线程ID：" << GetThreadId(m_hdlThread) << "执行完毕！！" << endl;
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
