#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CParticleSystem;

class ENGINE_DLL CParticleMgr : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr();

public:
	void	Create_Scatter(_vec3 vMin, _vec3 vMax, _float fSize = 0.25f, _int iCount = 30);
	void	Create_Explosion(_vec3 vPos, _vec3 vMin, _vec3 vMax, _float fSize = 0.5f, _int iCount = 10);
	void	Create_Splash(_vec3 vPos, const _tchar* pTexturePath, _int iMaxFrame);
	void	Create_Burst();

	void	Update_Particles(_float fDeltaTime);
	void	Render_Particles();
	

private:
	list<CParticleSystem*> m_pParticles;

private:
	virtual void		Free(void);
};

END