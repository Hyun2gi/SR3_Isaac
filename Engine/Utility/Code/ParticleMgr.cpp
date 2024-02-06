#include "..\..\Header\ParticleMgr.h"

#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
	:m_pGraphicDev(nullptr)
{
}

CParticleMgr::CParticleMgr(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
}

CParticleMgr::~CParticleMgr()
{
	Free();
}

void CParticleMgr::Create_Scatter(_vec3 vMin, _vec3 vMax, _float fSize, _int iCount)
{
	CParticleSystem* pParticle = CParticleScatter::Create(m_pGraphicDev, vMin, vMax, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Explosion(_vec3 vPos, _float fSize, _int iCount)
{
	CParticleExplosion* pParticle = CParticleExplosion::Create(m_pGraphicDev, vPos, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash(_vec3 vPos, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplash* pParticle = CParticleSplash::Create(m_pGraphicDev, vPos, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Burst(_vec3 vPos, _float fSize, _int iCount)
{
}

void CParticleMgr::Update_Particles(_float fDeltaTime)
{
}

void CParticleMgr::Render_Particles()
{
}


void CParticleMgr::Free(void)
{

}
