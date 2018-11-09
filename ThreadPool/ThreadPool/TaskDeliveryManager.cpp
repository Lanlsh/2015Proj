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
	//cout << "主线程 线程ID：" << GetCurrentThreadId() << "总任务数量：" <<m_queTask.size()<< endl;
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
		cout << "线程池对象为NULL！！！！" << endl;
		return;
	}

	Task task = NULL;
	while (bIsRunningTask)
	{
		m_mtxTask.lock();
		if (!m_queTask.empty())
		{
			/*
			std::move函数可以以非常简单的方式将左值引用转换为右值引用。（左值、左值引用、右值、右值引用 参见：http://www.cnblogs.com/SZxiaochun/p/8017475.html）
			通过std::move，可以避免不必要的拷贝操作。
			std::move是为性能而生。
			std::move是将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存的搬迁或者内存拷贝。
			*/
			task = std::move(m_queTask.front());	//取出一个任务！！！！
			m_queTask.pop();
		}
		m_mtxTask.unlock();

		//安排线程去执行任务
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
			cout << "线程池对象为NULL！！！！" << endl;
			break;
		}

		//Sleep(50);
	}

	cout << "----------------主线程执行完毕 ------------------" << endl;
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
