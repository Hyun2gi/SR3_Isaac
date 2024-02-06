#include "..\..\Header\ParticleMgr.h"

#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}

CParticleMgr::~CParticleMgr()
{
	Free();
}

void CParticleMgr::Create_Scatter(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vMin, _vec3 vMax, _float fSize, _int iCount)
{
	CParticleSystem* pParticle = CParticleScatter::Create(pGraphicDev, vMin, vMax, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Explosion(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fSize, _int iCount)
{
	CParticleExplosion* pParticle = CParticleExplosion::Create(pGraphicDev, vPos, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplash* pParticle = CParticleSplash::Create(pGraphicDev, vPos, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Burst(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fSize, _int iCount)
{
	CParticleBurst* pParticle = CParticleBurst::Create(pGraphicDev, vPos, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Update_Particles(_float fDeltaTime)
{
	for (auto i = m_pParticleList.begin(); i != m_pParticleList.end();)
	{
		(*i)->Update_Particle(fDeltaTime);

		if ((*i)->Get_Dead())
		{
			Safe_Release((*i));
			i = m_pParticleList.erase(i);
		}
		else
		{
			i++;
		}
	}

}

void CParticleMgr::Render_Particles()
{
	for (auto& iter : m_pParticleList)
		iter->Render_GameObject();
}


void CParticleMgr::Free(void)
{
	for_each(m_pParticleList.begin(), m_pParticleList.end(), CDeleteObj());
	m_pParticleList.clear();
}
