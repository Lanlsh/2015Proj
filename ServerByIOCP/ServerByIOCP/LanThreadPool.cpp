#include "stdafx.h"
#include "LanThreadPool.h"

//单例实体的初始化
LanThreadPool* LanThreadPool::m_lanThreadPool = new LanThreadPool();
std::mutex* LanThreadPool::m_mtxForInstance = new std::mutex();
//LanThreadPool::CThreadPoolGC LanThreadPool::m_gc;

LanThreadPool::LanThreadPool()
{
	m_vWorkerThread.clear();
	m_minThreadCount = 0;
	m_maxThreadCount = GetNumberOfProcessors() * 2;
}

LanThreadPool::LanThreadPool(const LanThreadPool&)
{

}

LanThreadPool& LanThreadPool::operator=(const LanThreadPool& threadPool)
{
	if (this != & threadPool)
	{
		
	}

	return *this;
}

LanThreadPool::~LanThreadPool()
{	
	if (m_mtxForInstance)
	{
		delete m_mtxForInstance;
		m_mtxForInstance = NULL;
	}
	
	for (std::vector<LanWorkerThread* >::iterator iter=m_vWorkerThread.begin(); iter!=m_vWorkerThread.end(); )
	{
		delete *iter;
		iter = m_vWorkerThread.erase(iter);
	}

	m_vWorkerThread.clear();
}

void LanThreadPool::SetThreadPool(int nMinThreadCount, int nMaxThreadCount)
{
	if ((nMinThreadCount < 0) || (nMaxThreadCount < 0) || (nMinThreadCount > nMaxThreadCount))
	{
		cout << "线程数设置的最大或最小值错误！！" << endl;
		return;
	}

	Initialize(nMinThreadCount, nMaxThreadCount);
}

void LanThreadPool::Initialize(int nMinThreadCount, int nMaxThreadCount)
{
	//如果设置的最小或最大线程数超过系统CPU核心数，则将线程数设置为CPU核心数*2
	int nNumberOfProcessors = GetNumberOfProcessors();
	nNumberOfProcessors = nNumberOfProcessors * 2;
	if (nMinThreadCount > nNumberOfProcessors)
	{
		nMinThreadCount = nNumberOfProcessors;
	}

	if (nMaxThreadCount > nNumberOfProcessors)
	{
		nMaxThreadCount = nNumberOfProcessors;
	}

	m_minThreadCount = nMinThreadCount;
	m_maxThreadCount = nMaxThreadCount;
	
	if (m_vWorkerThread.size() > 0)
	{
		for (std::vector<LanWorkerThread* >::iterator iter = m_vWorkerThread.begin(); iter != m_vWorkerThread.end(); )
		{
			//停止执行的任务
			(*iter)->Stop();

			//先唤醒所有线程
			ResumeThread((*iter)->GetHandle());

			delete *iter;
			iter = m_vWorkerThread.erase(iter);
		}
	}
	m_vWorkerThread.clear();

	CreateWorkerThread(nMinThreadCount, NULL);
}

int LanThreadPool::GetNumberOfProcessors()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	return sysInfo.dwNumberOfProcessors;
}

bool LanThreadPool::CreateWorkerThread(int nCount, Task task)
{
	for (int i=0; i<nCount; i++)
	{
		if (m_vWorkerThread.size() + 1 <= m_maxThreadCount)
		{
			LanWorkerThread* newLanWorkerThread = new LanWorkerThread();
			if (newLanWorkerThread != NULL)
			{
				m_vWorkerThread.push_back(newLanWorkerThread);
				newLanWorkerThread->SetTask(task);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

void LanThreadPool::ExecTask(Task task)
{
	//如果没有合适的线程执行这个任务，则一直循环，直到有线程接收了此任务
	if (task)
	{
		Task tempTask = task;
		//安排线程去执行任务
		bool bIsOk = false;
		do 
		{
			bool bIsRunTask = false;
			for (int i = 0; i < m_vWorkerThread.size(); i++)
			{
				if (!m_vWorkerThread[i]->GetRunState())
				{
					m_vWorkerThread[i]->SetTask(tempTask);
					bIsRunTask = true;
					bIsOk = true;
					break;
				}
			}

			if (!bIsRunTask)
			{
				bIsOk = CreateWorkerThread(1, tempTask);
				if (bIsOk)
				{
					m_vWorkerThread[m_vWorkerThread.size()-1]->Run();
				}
				
			}
		} while (!bIsOk && tempTask);

	}

}

void LanThreadPool::StopTask()
{
	cout << "当前运行的线程数量： " << m_vWorkerThread.size() << endl;
	for (int i = 0; i < m_vWorkerThread.size(); i++)
	{
		m_vWorkerThread[i]->Stop();

		//先唤醒所有线程
		ResumeThread(m_vWorkerThread[i]->GetHandle());
	}
}

LanThreadPool* LanThreadPool::GetLanThreadPoolInstance()
{
	//如果多线程时，同时获取这个单例，但此时单例还没有创建，则有可能创建多个实例，所以加个锁
	if (m_lanThreadPool == NULL)
	{
		m_mtxForInstance->lock();
		if (m_lanThreadPool == NULL)
		{
			m_lanThreadPool = new LanThreadPool();
		}
		m_mtxForInstance->unlock();
	}
	
	return m_lanThreadPool;
}

void LanThreadPool::FreeMommery()
{
	if (m_lanThreadPool)
	{
		delete m_lanThreadPool;
		m_lanThreadPool = NULL;
	}
}

void LanThreadPool::SetIOCP(CCoreByIOCP * coreByIOCP)
{
	if (coreByIOCP == NULL)
	{
		cout << "IOCP对象为NULL！！！" << endl;
		return;
	}

	for (int i =0; i<m_vWorkerThread.size(); i++)
	{
		m_vWorkerThread[i]->SetCoreIOCP(coreByIOCP);
	}
}

