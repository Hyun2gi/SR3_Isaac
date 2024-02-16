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

void CParticleMgr::Kill_Scatter()
{
	dynamic_cast<CParticleScatter*>(m_pParticleList.front())->Kill_All();
}

void CParticleMgr::Create_Explosion(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize, _int iCount, _float fSpeed, const _tchar* pTexturePath)
{
	CParticleExplosion* pParticle = CParticleExplosion::Create(pGraphicDev, matWorld, fSize, iCount, fSpeed, pTexturePath);
	pParticle->Set_World_Matrix(matWorld);
	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Dust(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize, _int iCount)
{
	CParticleDust* pParticle = CParticleDust::Create(pGraphicDev, matWorld, fSize, iCount);
	pParticle->Set_World_Matrix(matWorld);
	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplash* pParticle = CParticleSplash::Create(pGraphicDev, matWorld, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash_Left(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplashLeft* pParticle = CParticleSplashLeft::Create(pGraphicDev, matWorld, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash_Right(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplashRight* pParticle = CParticleSplashRight::Create(pGraphicDev, matWorld, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Splash_Forward(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplashForward* pParticle = CParticleSplashForward::Create(pGraphicDev, matWorld, pTexturePath, iMaxFrame, fSize, iCount);

	m_pParticleList.emplace_back(pParticle);
}

void CParticleMgr::Create_Burst(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize, _int iCount)
{
	CParticleBurst* pParticle = CParticleBurst::Create(pGraphicDev, matWorld, fSize, iCount);

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
