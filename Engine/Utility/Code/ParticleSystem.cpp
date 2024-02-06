#include "..\..\Header\ParticleSystem.h"

#include "Export_Utility.h"

CParticleSystem::CParticleSystem()
{
	m_pGraphicDev = 0;
	m_pVb = 0;
	m_pTex = 0;
}

CParticleSystem::~CParticleSystem()
{

}

void CParticleSystem::Reset()
{
	list<Attribute>::iterator i;
	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		Reset_Partice(&(*i));
	}
}

void CParticleSystem::Add_Particle()
{
	Attribute attribute;

	Reset_Partice(&attribute);

	m_ParticlesList.push_back(attribute);
}

void CParticleSystem::Render_GameObject()
{
	if (!m_ParticlesList.empty())
	{
		m_pGraphicDev->SetTexture(0, m_pTex);
		m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_pGraphicDev->SetStreamSource(0, m_pVb, 0, sizeof(Particle));

		// start at beginning if we're at the end of the vb
		if (m_VbOffset >= m_VbSize)
			m_VbOffset = 0;

		Particle* v = 0;

		m_pVb->Lock(
			m_VbOffset * sizeof(Particle),
			m_VbBatchSize * sizeof(Particle),
			(void**)&v,
			m_VbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		list<Attribute>::iterator i;
		for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
		{
			if (i->_bIsAlive)
			{
				v->_position = i->_vPosition;
				v->_color = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				if (numParticlesInBatch == m_VbBatchSize)
				{
					m_pVb->Unlock();

					m_pGraphicDev->DrawPrimitive(
						D3DPT_POINTLIST,
						m_VbOffset,
						m_VbBatchSize);

					m_VbOffset += m_VbBatchSize;

					if (m_VbOffset >= m_VbSize)
						m_VbOffset = 0;

					m_pVb->Lock(
						m_VbOffset * sizeof(Particle),
						m_VbBatchSize * sizeof(Particle),
						(void**)&v,
						m_VbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}

		m_pVb->Unlock();

		if (numParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(
				D3DPT_POINTLIST,
				m_VbOffset,
				numParticlesInBatch);
		}

		m_VbOffset += m_VbBatchSize;

	}
}

bool CParticleSystem::Is_Empty()
{
	return m_ParticlesList.empty();
}

bool CParticleSystem::Is_Dead()
{
	list<Attribute>::iterator i;
	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		if (i->_bIsAlive)
			return false;
	}
	return true;
}

void CParticleSystem::Remove_Dead_Particles()
{
	list<Attribute>::iterator i;

	i = m_ParticlesList.begin();

	while (i != m_ParticlesList.end())
	{
		if (i->_bIsAlive == false)
		{
			i = m_ParticlesList.erase(i);
		}
		else
		{
			i++; // next in list
		}
	}
}

void CParticleSystem::Free()
{
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}