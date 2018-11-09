#pragma once
#include "stdafx.h"
#include <queue>
#include <mutex>

/*
	任务分发管理类
*/

//定义任务回调函数指针
typedef void(*Task)(VOID);

class TaskDeliveryManager
{
public:
	TaskDeliveryManager();
	~TaskDeliveryManager();

	//增加任务
	void AddTask(Task task);

	//启动任务
	void RunTask();

	//停止处理任务
	void StopTask();

private:
	//保存任务队列(Task其实就是回调函数)
	std::queue<Task> m_queTask;

	//任务锁
	std::mutex m_mtxTask;

	//是否执行任务
	bool bIsRunningTask;
};

