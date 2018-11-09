#pragma once
#include "stdafx.h"
#include <mutex>
#include "LanWorkerThread.h"

/*
	1. 基于windows api实现
	2. 基于时间对象同步
	3. 线程池设计成单例模式
	4. 功能描述：
		a. 先初始化一定的线程数量
		b. 当有任务时，使用初始化的线程去执行
		c. 如果任务太多线程不够用时，创建线程
		d. 总的线程数不能超过CPU核心数*2
		e. 使用“事件”和原子操作进行线程同步
*/

//定义任务回调函数指针
typedef void(*Task)(VOID);

class LanThreadPool
{
public:
	~LanThreadPool();

	void SetThreadPool(int nMinThreadCount, int nMaxThreadCount);

	void Initialize(int nMinThreadCount, int nMaxThreadCount);

	//获取本机的CPU核心数
	int GetNumberOfProcessors();

	//创建工作线程
	bool CreateWorkerThread(int nCount, Task task);

	//执行任务
	void ExecTask(Task task);

	//停止处理任务
	void StopTask();

	//获取内存池单例实例对象
	static LanThreadPool* GetLanThreadPoolInstance();

	//释放内存
	void FreeMommery();

private:
	LanThreadPool();
	//把复制构造函数和=操作符也设为私有,防止被复制
	LanThreadPool(const LanThreadPool&);
	LanThreadPool& operator=(const LanThreadPool& threadPool);
	//单例对象
	static LanThreadPool* m_lanThreadPool;

	//给单例实例的锁
	static std::mutex* m_mtxForInstance;

	//创建自动释放内存的类（利用进程结束时，系统会自动析构此静态成员的原理）--如果想在进程结束时自动释放内存，可使用此方法
	//class CThreadPoolGC
	//{
	//public:
	//	CThreadPoolGC() {}
	//	~CThreadPoolGC()
	//	{
	//		if (m_lanThreadPool)
	//		{
	//			delete m_lanThreadPool;
	//			m_lanThreadPool = NULL;
	//		}
	//	}

	//};

	//static CThreadPoolGC m_gc;	//	垃圾回收类的静态成员函数

private:
	//保存内存池
	std::vector<LanWorkerThread* > m_vWorkerThread;

	//初始化线程数量
	int m_minThreadCount;

	//最大线程数
	int m_maxThreadCount;
};

