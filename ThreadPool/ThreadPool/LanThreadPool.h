#pragma once
#include "stdafx.h"
#include <mutex>
#include "LanWorkerThread.h"

/*
	1. ����windows apiʵ��
	2. ����ʱ�����ͬ��
	3. �̳߳���Ƴɵ���ģʽ
	4. ����������
		a. �ȳ�ʼ��һ�����߳�����
		b. ��������ʱ��ʹ�ó�ʼ�����߳�ȥִ��
		c. �������̫���̲߳�����ʱ�������߳�
		d. �ܵ��߳������ܳ���CPU������*2
		e. ʹ�á��¼�����ԭ�Ӳ��������߳�ͬ��
*/

//��������ص�����ָ��
typedef void(*Task)(VOID);

class LanThreadPool
{
public:
	~LanThreadPool();

	void SetThreadPool(int nMinThreadCount, int nMaxThreadCount);

	void Initialize(int nMinThreadCount, int nMaxThreadCount);

	//��ȡ������CPU������
	int GetNumberOfProcessors();

	//���������߳�
	bool CreateWorkerThread(int nCount, Task task);

	//ִ������
	void ExecTask(Task task);

	//ֹͣ��������
	void StopTask();

	//��ȡ�ڴ�ص���ʵ������
	static LanThreadPool* GetLanThreadPoolInstance();

	//�ͷ��ڴ�
	void FreeMommery();

private:
	LanThreadPool();
	//�Ѹ��ƹ��캯����=������Ҳ��Ϊ˽��,��ֹ������
	LanThreadPool(const LanThreadPool&);
	LanThreadPool& operator=(const LanThreadPool& threadPool);
	//��������
	static LanThreadPool* m_lanThreadPool;

	//������ʵ������
	static std::mutex* m_mtxForInstance;

	//�����Զ��ͷ��ڴ���ࣨ���ý��̽���ʱ��ϵͳ���Զ������˾�̬��Ա��ԭ��--������ڽ��̽���ʱ�Զ��ͷ��ڴ棬��ʹ�ô˷���
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

	//static CThreadPoolGC m_gc;	//	����������ľ�̬��Ա����

private:
	//�����ڴ��
	std::vector<LanWorkerThread* > m_vWorkerThread;

	//��ʼ���߳�����
	int m_minThreadCount;

	//����߳���
	int m_maxThreadCount;
};

