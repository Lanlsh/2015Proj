#include "stdafx.h"
#include "LanCriticalLock.h"

LanCriticalLock::LanCriticalLock(CRITICAL_SECTION& cs, const std::string& strFunc)
{
	m_strFunc = strFunc;
	m_pcs = &cs;
	Lock();
}


LanCriticalLock::~LanCriticalLock()
{
	Unlock();
}

void LanCriticalLock::Lock()
{
	EnterCriticalSection(m_pcs);
}

void LanCriticalLock::Unlock()
{
	LeaveCriticalSection(m_pcs);
}