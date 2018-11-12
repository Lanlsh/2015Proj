#pragma once
#include "stdafx.h"
#include "LanEvent.h"
#include <atomic>

/*
	ʵ��ִ��ĳһ������
*/

//��������ص�����ָ��
typedef void(*Task)(VOID);

class LanWorkerThread
{
public:
	LanWorkerThread();
	~LanWorkerThread();
	
	//��ʼ����
	void Run();

	//����coreIOCP����
	void SetCoreIOCP(CCoreByIOCP* coreIOCP);

	//ֹͣ����
	void Stop();

	//�߳�ִ�к���
	static unsigned __stdcall  ThreadExec(void* pThis);

	//�߳�������ִ�к���
	void DoExec();

	//��������Ļص�����
	void SetTask(Task task);

	//��ȡʹ��״̬
	bool GetRunState() 
	{ 
		return m_bRunState; 
	}

	//��ȡ�����̵߳ľ��
	HANDLE GetHandle() { return m_hdlThread; }

private:
	//�Ƿ�����
	bool m_bIsStop;

	//ʹ��״̬
	std::atomic<bool> m_bRunState;

	//�߳̾��
	HANDLE m_hdlThread;

	//���̵߳�ִ�е�����
	Task m_task;

	//�¼�ͬ������
	LanEvent m_eventCondition;

	//coreIOCP
	CCoreByIOCP* m_coreIOCP;

};

