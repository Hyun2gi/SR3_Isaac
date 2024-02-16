#include "..\..\Header\ParticleSystem.h"
#include "ParticleExplosion.h"

#include "Export_Utility.h"

CParticleExplosion::CParticleExplosion(int numParticles, _float fSize)
{
	m_fSize = fSize;
	m_VbSize = 2048;
	m_VbOffset = 0;
	m_VbBatchSize = 512;

	m_vMinVelocity = _vec3(-0.5f, 0.0f, -1.0f);
	m_vMaxVelocity = _vec3(0.5f, 1.0f, 1.0f);

	for (int i = 0; i < numParticles; i++)
		Add_Particle();
}

CParticleExplosion::~CParticleExplosion()
{
	Free();
}

void CParticleExplosion::Free()
{
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}

bool CParticleExplosion::Ready_Particle(IDirect3DDevice9* pDevice)
{
	m_pGraphicDev = pDevice;

	HRESULT hr = 0;

	hr = pDevice->CreateVertexBuffer(
		m_VbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&m_pVb,
		0);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = D3DXCreateTextureFromFile(
		pDevice,
		m_wstrTexturePath.c_str(),
		&m_pTex);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
}

void CParticleExplosion::Reset_Partice(Attribute* attribute)
{
	attribute->_bIsAlive = true;
	attribute->_vPosition = { m_matWorld._41, m_matWorld._42, m_matWorld._43 };

	GetRandomVector(
		&attribute->_vVelocity,
		&m_vMinVelocity,
		&m_vMaxVelocity);

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->_vVelocity,
		&attribute->_vVelocity);

	attribute->_vVelocity *= 2.f;

	attribute->_color = D3DXCOLOR(
		1.f,
		1.f,
		1.f,
		0.5f);

	//attribute->_color = D3DXCOLOR(
	//	GetRandomFloat(0.0f, 1.0f),
	//	GetRandomFloat(0.0f, 1.0f),
	//	GetRandomFloat(0.0f, 1.0f),
	//	1.0f);

	attribute->_fAge = 0.0f;
	attribute->_fLifeTime = 1.0f; // lives for 2 seconds
}

void CParticleExplosion::Update_Particle(_float fTimeDelat)
{
	list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		// only update living particles
		if (i->_bIsAlive)
		{
			i->_vPosition += i->_vVelocity * fTimeDelat;

			i->_fAge += fTimeDelat;

			if (i->_fAge > i->_fLifeTime) // kill 
				i->_bIsAlive = false;
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);

	__super::Update_Particle(fTimeDelat);
}

CParticleExplosion* CParticleExplosion::Create(IDirect3DDevice9* pDevice, 
	_matrix matWorld, 
	_float fSize, _int iCount,
	const _tchar* pTextruePath)
{
	CParticleExplosion* pInstance = new CParticleExplosion(iCount, fSize);
	pInstance->Set_World_Matrix(matWorld);
	pInstance->m_wstrTexturePath = pTextruePath;

	if (FAILED(pInstance->Ready_Particle(pDevice)))
	{
		Safe_Release(pInstance);
		MSG_BOX("ParticleExplosion Create Failed");
		return nullptr;
	}

	return pInstance;
}