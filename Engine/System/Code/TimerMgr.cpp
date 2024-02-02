#include "..\..\Header\TimerMgr.h"

IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}

CTimerMgr::~CTimerMgr()
{
	Free();
}

_float CTimerMgr::Get_TimeDelta(const _tchar * pTimerTag) 
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	return pTimer->Get_TimeDelta();
}

bool CTimerMgr::Set_TimeDeltaScale(const _tchar* pTimerTag, _float fDeltaScale)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK_RETURN(pTimer, 0.f);

	pTimer->Set_DeltaScale(fDeltaScale);
}

void CTimerMgr::Update_TimeDelta(const _tchar * pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	NULL_CHECK(pTimer);

	pTimer->Update_TimeDelta();
}

HRESULT CTimerMgr::Ready_Timer(const TCHAR * pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	m_mapTimers.insert({pTimerTag, pTimer});

	return S_OK;
}

CTimer * CTimerMgr::Find_Timer(const _tchar * pTimerTag)
{
	auto		iter = find_if(m_mapTimers.begin(), 
		m_mapTimers.end(), 
		CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;
	
	return iter->second;
}

void CTimerMgr::Free(void)
{
	for_each(m_mapTimers.begin(), m_mapTimers.end(), CDeleteMap());
	m_mapTimers.clear();
}
