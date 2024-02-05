#include "..\..\Header\ParticleSystem.h"
#include "ParticleExplosion.h"

CParticleExplosion::CParticleExplosion(_vec3* vOrigin, int numParticles)
{
	m_vOrigin = *vOrigin;
	m_fSize = 0.7f;
	m_VbSize = 2048;
	m_VbOffset = 0;
	m_VbBatchSize = 512;

	m_vMinVelocity = _vec3(-0.5f, 0.0f, -1.0f);
	m_vMinVelocity = _vec3(0.5f, 1.0f, 1.0f);
	m_fVelocitySpeed = 2.f;

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

void CParticleExplosion::Reset_Partice(Attribute* attribute)
{
	attribute->_bIsAlive = true;
	attribute->_vPosition = m_vOrigin;

	GetRandomVector(
		&attribute->_vVelocity,
		&m_vMinVelocity,
		&m_vMinVelocity);

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->_vVelocity,
		&attribute->_vVelocity);

	attribute->_vVelocity *= m_fVelocitySpeed;

	attribute->_color = D3DXCOLOR(
		1.f,
		1.f,
		1.f,
		1.0f);

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
}