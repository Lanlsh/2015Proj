#include "stdafx.h"
#include "LanWorkerThread.h"
#include <process.h>	//for _beginthreadex

LanWorkerThread::LanWorkerThread()
{
	m_bIsStop = false;
	m_bRunState = false;
	m_task = NULL;
	m_coreIOCP = NULL;

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

void LanWorkerThread::SetCoreIOCP(CCoreByIOCP* coreIOCP)
{
	if (coreIOCP != NULL)
	{
		m_coreIOCP = coreIOCP;
	}
}

void LanWorkerThread::Run()
{
	if (m_coreIOCP == NULL)
	{
		cout << "IOCP对象为NULL！！！！！" << endl;
		return;
	}

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

unsigned __stdcall  LanWorkerThread::ThreadExec(void* m_coreIOCP)
{
	LanWorkerThread* pWorker = (LanWorkerThread*)m_coreIOCP;
	pWorker->DoExec();
	
	return 1;
}

void LanWorkerThread::DoExec()
{
	if (m_coreIOCP == NULL)
	{
		cout << "IOCP对象为NULL！！！！！" << endl;
		return;
	}

	OVERLAPPED*	pOverlapped = NULL;
	DWORD dwIoSize = 0;
	BOOL bRet = FALSE;
	DWORD dwErr = 0;
	IOCP_PARAM* pIocpParam = NULL;
	PER_IO_CONTEXT* pIoContext = NULL;
	HANDLE hdIOCP = m_coreIOCP->GetIOCPPort();

	while (1)
	{
		//m_eventCondition.Wait();
		//m_bRunState = false;
		bRet = GetQueuedCompletionStatus(//  从完成端口中获取消息
			hdIOCP,
			&dwIoSize,
			(PULONG_PTR)&pIocpParam,
			&pOverlapped,
			INFINITE);

		if (EXIT_CODE == pIocpParam)
			break;

		pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_overLapped);

		if (!bRet)// 处理错误信息
		{
			dwErr = GetLastError();
			if (WAIT_TIMEOUT == dwErr)// 超时
			{
				// 超时后，通过发送一个消息，判断是否断线，否则在socket上投递WSARecv会出错
				// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的
				if (-1 == send(pIocpParam->m_rourceSock, "", 0, 0))
				{
					m_coreIOCP->MoveToFreeParamPool(pIocpParam);
					m_coreIOCP->RemoveStaleClient(pIoContext, FALSE);
				}
				continue;
			}
			if (ERROR_NETNAME_DELETED == dwErr)// 客户端异常退出
			{
				m_coreIOCP->MoveToFreeParamPool(pIocpParam);
				m_coreIOCP->RemoveStaleClient(pIoContext, FALSE);
				continue;
			}

			break;// 完成端口出现错误
		}

		// 正式处理接收到的数据 读取接收到的数据
		// CONTAINING_RECORD宏返回给定结构类型的结构实例的 基地址 和包含结构中字段的地址。
		if (bRet && 0 == dwIoSize)
		{
			// 客户端断开连接，释放资源
			m_coreIOCP->MoveToFreeParamPool(pIocpParam);
			m_coreIOCP->RemoveStaleClient(pIoContext, FALSE);
			continue;
		}

		if (bRet && NULL != pIoContext && NULL != pIocpParam)
		{
			try
			{
				m_coreIOCP->ProcessIOMessage(pIoContext->m_IOType, pIoContext, dwIoSize);
			}
			catch (...) {}
		}
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
