#include "..\..\Header\ParticleSystem.h"
#include "ParticleScatter.h"

CParticleScatter::CParticleScatter(BoundingBox* boundingBox, int numParticles)
{
	m_tBoundingBox = *boundingBox;
	m_fSize = 0.25f;
	m_VbSize = 2048;
	m_VbOffset = 0;
	m_VbBatchSize = 512;

	for (int i = 0; i < numParticles; i++)
		Add_Particle();
}

CParticleScatter::~CParticleScatter()
{

}

void CParticleScatter::Free()
{
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}

void CParticleScatter::Reset_Partice(Attribute* attribute)
{
	attribute->_bIsAlive = true;

	GetRandomVector(
		&attribute->_vPosition,
		&m_tBoundingBox._min,
		&m_tBoundingBox._max);

	attribute->_vPosition.y = m_tBoundingBox._max.y;

	attribute->_vVelocity.x = GetRandomFloat(0.0f, 1.0f) * -3.0f;
	attribute->_vVelocity.y = GetRandomFloat(0.0f, 1.0f) * -10.0f;
	attribute->_vVelocity.z = 0.0f;

	attribute->_color = D3DCOLOR_XRGB(255, 255, 255);
}

void CParticleScatter::Update_Particle(_float fTimeDelat)
{
	std::list<Attribute>::iterator i;
	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		i->_vPosition += i->_vVelocity * fTimeDelat;

		if (m_tBoundingBox.isPointInside(i->_vPosition) == false)
		{
			Reset_Partice(&(*i));
		}
	}
}