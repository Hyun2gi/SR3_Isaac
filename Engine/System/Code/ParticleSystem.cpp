#include "..\..\Header\ParticleSystem.h"

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CParticleSystem::CParticleSystem()
{
	m_pGraphicDev = 0;
	m_pVb = 0;
	m_pTex = 0;
}

CParticleSystem::~CParticleSystem()
{
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}

bool CParticleSystem::Ready_Particle(IDirect3DDevice9* pDevice, char* texFileName)
{
	m_pGraphicDev = pDevice;

	HRESULT hr = 0;

	hr = pDevice->CreateVertexBuffer(
		m_VbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&m_pVb,
		0);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	hr = D3DXCreateTextureFromFile(
		pDevice,
		texFileName,
		&m_pTex);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
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

void CParticleSystem::Pre_Render()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// control the size of the particle relative to distance
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// use alpha from texture
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CParticleSystem::Render()
{
	if (!m_ParticlesList.empty())
	{
		//
		// set render states
		//

		Pre_Render();

		m_pGraphicDev->SetTexture(0, m_pTex);
		m_pGraphicDev->SetFVF(Particle::FVF);
		m_pGraphicDev->SetStreamSource(0, m_pVb, 0, sizeof(Particle));

		//
		// render batches one by one
		//

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

		//
		// Until all particles have been rendered.
		//
		std::list<Attribute>::iterator i;
		for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
		{
			if (i->_bIsAlive)
			{
				//
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				//
				v->_position = i->_vPosition;
				v->_color = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				// if this batch full?
				if (numParticlesInBatch == m_VbBatchSize)
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					//
					m_pVb->Unlock();

					m_pGraphicDev->DrawPrimitive(
						D3DPT_POINTLIST,
						m_VbOffset,
						m_VbBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					m_VbOffset += m_VbBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
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

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.

		if (numParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(
				D3DPT_POINTLIST,
				m_VbOffset,
				numParticlesInBatch);
		}

		// next block
		m_VbOffset += m_VbBatchSize;

		//
		// reset render states
		//

		Post_Render();
	}
}

void CParticleSystem::Post_Render()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
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
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if (i->_bIsAlive)
			return false;
	}
	// no living particles found, the system must be dead.
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
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			i = m_ParticlesList.erase(i);
		}
		else
		{
			i++; // next in list
		}
	}
}



CParticleSystem * CParticleSystem::Create(const _float & fCallLimit)
{
	CParticleSystem *		pInstance = new CParticleSystem;

	if (FAILED(pInstance->Ready_Frame(fCallLimit)))
	{
		MSG_BOX("Frame Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CParticleSystem::Free()
{
}
