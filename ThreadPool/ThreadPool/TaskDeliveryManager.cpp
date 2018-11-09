#include "stdafx.h"
#include "TaskDeliveryManager.h"
#include "LanThreadPool.h"


TaskDeliveryManager::TaskDeliveryManager()
{
	bIsRunningTask = true;
}


TaskDeliveryManager::~TaskDeliveryManager()
{
	LanThreadPool* threadPool = LanThreadPool::GetLanThreadPoolInstance();
	if (threadPool)
	{
		threadPool->FreeMommery();
	}
}

void TaskDeliveryManager::AddTask(Task task)
{
	m_mtxTask.lock();
	m_queTask.push(task);
	//cout << "���߳� �߳�ID��" << GetCurrentThreadId() << "������������" <<m_queTask.size()<< endl;
	m_mtxTask.unlock();
}

void TaskDeliveryManager::RunTask()
{
	LanThreadPool* threadPool = LanThreadPool::GetLanThreadPoolInstance();
	if (threadPool)
	{
		threadPool->SetThreadPool(2, 6);
	}
	else
	{
		cout << "�̳߳ض���ΪNULL��������" << endl;
		return;
	}

	Task task = NULL;
	while (bIsRunningTask)
	{
		m_mtxTask.lock();
		if (!m_queTask.empty())
		{
			/*
			std::move���������Էǳ��򵥵ķ�ʽ����ֵ����ת��Ϊ��ֵ���á�����ֵ����ֵ���á���ֵ����ֵ���� �μ���http://www.cnblogs.com/SZxiaochun/p/8017475.html��
			ͨ��std::move�����Ա��ⲻ��Ҫ�Ŀ���������
			std::move��Ϊ���ܶ�����
			std::move�ǽ������״̬��������Ȩ��һ������ת�Ƶ���һ������ֻ��ת�ƣ�û���ڴ�İ�Ǩ�����ڴ濽����
			*/
			task = std::move(m_queTask.front());	//ȡ��һ�����񣡣�����
			m_queTask.pop();
		}
		m_mtxTask.unlock();

		//�����߳�ȥִ������
		if (threadPool)
		{
			if (task)
			{
				threadPool->ExecTask(task);
				task = NULL;
			}

		}
		else
		{
			cout << "�̳߳ض���ΪNULL��������" << endl;
			break;
		}

		//Sleep(50);
	}

	cout << "----------------���߳�ִ����� ------------------" << endl;
}

void TaskDeliveryManager::StopTask()
{
	bIsRunningTask = false;
	LanThreadPool* threadPool = LanThreadPool::GetLanThreadPoolInstance();
	if (threadPool)
	{
		threadPool->StopTask();
	}
}
