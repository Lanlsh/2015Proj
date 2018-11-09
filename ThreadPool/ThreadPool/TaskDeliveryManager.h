#pragma once
#include "stdafx.h"
#include <queue>
#include <mutex>

/*
	����ַ�������
*/

//��������ص�����ָ��
typedef void(*Task)(VOID);

class TaskDeliveryManager
{
public:
	TaskDeliveryManager();
	~TaskDeliveryManager();

	//��������
	void AddTask(Task task);

	//��������
	void RunTask();

	//ֹͣ��������
	void StopTask();

private:
	//�����������(Task��ʵ���ǻص�����)
	std::queue<Task> m_queTask;

	//������
	std::mutex m_mtxTask;

	//�Ƿ�ִ������
	bool bIsRunningTask;
};

