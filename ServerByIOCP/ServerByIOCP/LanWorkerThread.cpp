#include "stdafx.h"
#include "LanWorkerThread.h"
#include <process.h>	//for _beginthreadex

LanWorkerThread::LanWorkerThread()
{
	m_bIsStop = false;
	m_bRunState = false;
	m_task = NULL;
	m_coreIOCP = NULL;

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
		cout << "IOCP����ΪNULL����������" << endl;
		return;
	}

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
		cout << "IOCP����ΪNULL����������" << endl;
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
		bRet = GetQueuedCompletionStatus(//  ����ɶ˿��л�ȡ��Ϣ
			hdIOCP,
			&dwIoSize,
			(PULONG_PTR)&pIocpParam,
			&pOverlapped,
			INFINITE);

		if (EXIT_CODE == pIocpParam)
			break;

		pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_overLapped);

		if (!bRet)// ���������Ϣ
		{
			dwErr = GetLastError();
			if (WAIT_TIMEOUT == dwErr)// ��ʱ
			{
				// ��ʱ��ͨ������һ����Ϣ���ж��Ƿ���ߣ�������socket��Ͷ��WSARecv�����
				// ��Ϊ����ͻ��������쳣�Ͽ�(����ͻ��˱������߰ε����ߵ�)��ʱ�򣬷����������޷��յ��ͻ��˶Ͽ���֪ͨ��
				if (-1 == send(pIocpParam->m_rourceSock, "", 0, 0))
				{
					m_coreIOCP->MoveToFreeParamPool(pIocpParam);
					m_coreIOCP->RemoveStaleClient(pIoContext, FALSE);
				}
				continue;
			}
			if (ERROR_NETNAME_DELETED == dwErr)// �ͻ����쳣�˳�
			{
				m_coreIOCP->MoveToFreeParamPool(pIocpParam);
				m_coreIOCP->RemoveStaleClient(pIoContext, FALSE);
				continue;
			}

			break;// ��ɶ˿ڳ��ִ���
		}

		// ��ʽ������յ������� ��ȡ���յ�������
		// CONTAINING_RECORD�귵�ظ����ṹ���͵Ľṹʵ���� ����ַ �Ͱ����ṹ���ֶεĵ�ַ��
		if (bRet && 0 == dwIoSize)
		{
			// �ͻ��˶Ͽ����ӣ��ͷ���Դ
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
