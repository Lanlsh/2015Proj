#pragma once
#include "stdafx.h"
#include "LanEvent.h"
#include <atomic>

/*
	实现执行某一个函数
*/

//定义任务回调函数指针
typedef void(*Task)(VOID);

class LanWorkerThread
{
public:
	LanWorkerThread();
	~LanWorkerThread();
	
	//开始运行
	void Run();

	//设置coreIOCP对象
	void SetCoreIOCP(CCoreByIOCP* coreIOCP);

	//停止运行
	void Stop();

	//线程执行函数
	static unsigned __stdcall  ThreadExec(void* pThis);

	//线程真正的执行函数
	void DoExec();

	//设置任务的回调函数
	void SetTask(Task task);

	//获取使用状态
	bool GetRunState() 
	{ 
		return m_bRunState; 
	}

	//获取工作线程的句柄
	HANDLE GetHandle() { return m_hdlThread; }

private:
	//是否运行
	bool m_bIsStop;

	//使用状态
	std::atomic<bool> m_bRunState;

	//线程句柄
	HANDLE m_hdlThread;

	//本线程的执行的任务
	Task m_task;

	//事件同步对象
	LanEvent m_eventCondition;

	//coreIOCP
	CCoreByIOCP* m_coreIOCP;

};

