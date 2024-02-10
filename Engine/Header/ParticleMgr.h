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
	void	Create_Scatter(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vMin, _vec3 vMax, _float fSize = 0.25f, _int iCount = 30);
	void	Kill_Scatter();
	void	Create_Explosion(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 0.7f, _int iCount = 10);
	void	Create_Dust(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 0.7f, _int iCount = 10);
	void	Create_Splash(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize = 0.5f, _int iCount = 10);
	void	Create_Splash_Left(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize = 0.5f, _int iCount = 10);
	void	Create_Splash_Right(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize = 0.5f, _int iCount = 10);
	void	Create_Burst(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 0.5f, _int iCount = 10);

	void	Update_Particles(_float fDeltaTime);
	void	Render_Particles();
	

private:
	list<CParticleSystem*> m_pParticleList;

private:
	virtual void		Free(void);
};

END