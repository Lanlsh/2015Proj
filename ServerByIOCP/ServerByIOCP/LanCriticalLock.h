#pragma once
#include <Windows.h>
#include <string>

/*
	������ ����ͬһ�����еĶ��߳�ͬ��
*/

class LanCriticalLock
{
public:
	LanCriticalLock(CRITICAL_SECTION& cs, const std::string& strFunc);
	~LanCriticalLock();

protected:
	void Lock();
	void Unlock();

private:
	CRITICAL_SECTION*	m_pcs;
	std::string			m_strFunc;
};

