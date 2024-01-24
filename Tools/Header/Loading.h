#pragma once

#include "Base.h"
#include "Engine_Define.h"

class CLoading : public CBase
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_BOSS, LOADING_END };

public:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	LOADINGID			Get_LoadingID() const { return m_eID; }
	CRITICAL_SECTION*	Get_Crt() { return &m_Crt; }
	_bool				Get_Finish() const { return m_bFinish; }

public:
	HRESULT		Ready_Loading(LOADINGID eID);
	_uint		Loading_ForStage();
	const _tchar*		Get_String() const { return m_szLoading; }

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);

private:
	HANDLE				m_hThread;

	CRITICAL_SECTION	m_Crt;

	LOADINGID			m_eID;

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	bool				m_bFinish;
	_tchar				m_szLoading[128];

public:
	static CLoading*		Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void		Free();

};

