#pragma once
#include <Windows.h>
#include <string>

class CLock
{
public:
	CLock(CRITICAL_SECTION& cs, const std::string& strFunc)
	{
		m_strFunc = strFunc;
		m_pcs = &cs;
		Lock();
	}
	~CLock()
	{
		Unlock();
	}

protected:
	CRITICAL_SECTION*	m_pcs;
	std::string			m_strFunc;
	
	void Unlock()
	{
		LeaveCriticalSection(m_pcs);
	}

	void Lock()
	{
		EnterCriticalSection(m_pcs);
	}
};