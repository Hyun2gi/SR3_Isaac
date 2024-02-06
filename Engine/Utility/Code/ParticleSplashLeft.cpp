#include "..\..\Header\ParticleSystem.h"
#include "ParticleSplashLeft.h"

#include "Export_Utility.h"

CParticleSplashLeft::CParticleSplashLeft(_vec3* vOrigin, int numParticles, _float fSize)
{
	m_vOrigin = *vOrigin;
	m_fSize = fSize;
	m_VbSize = 2048;
	m_VbOffset = 0;
	m_VbBatchSize = 512;
	m_fGravity = -8.f;

	for (int i = 0; i < numParticles; i++)
		Add_Particle();

	//CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomLeg.png")
}

CParticleSplashLeft::~CParticleSplashLeft()
{
	Free();
}

void CParticleSplashLeft::Free()
{
	Engine::Safe_Release(m_pTexture);
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}

bool CParticleSplashLeft::Ready_Particle(IDirect3DDevice9* pDevice)
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

	return S_OK;
}

void CParticleSplashLeft::Reset_Partice(Attribute* attribute)
{
	attribute->_bIsAlive = true;
	attribute->_vPosition = m_vOrigin;

	_vec3 vMin = _vec3(-1.f, 1.0f, -0.8f);
	_vec3 vMax = _vec3(-0.5f, 1.0f, 0.8f);

	GetRandomVector(
		&attribute->_vVelocity,
		&vMin,
		&vMax);

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->_vVelocity,
		&attribute->_vVelocity);

	attribute->_vVelocity *= 3.f;

	attribute->_color = D3DXCOLOR(
		0.5f,
		0.5f,
		0.5f,
		1.0f);

	//attribute->_color = D3DXCOLOR(
	//	GetRandomFloat(0.0f, 1.0f),
	//	GetRandomFloat(0.0f, 1.0f),
	//	GetRandomFloat(0.0f, 1.0f),
	//	1.0f);

	attribute->_fAge = 0.0f;
	attribute->_fLifeTime = 0.8f;
}

void CParticleSplashLeft::Update_Particle(_float fTimeDelat)
{
	m_fFrame += m_iPicNum * fTimeDelat * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	list<Attribute>::iterator i;

	for (i = m_ParticlesList.begin(); i != m_ParticlesList.end(); i++)
	{
		// only update living particles
		if (i->_bIsAlive)
		{
			i->_vVelocity.y += m_fGravity * fTimeDelat;
			i->_vPosition += i->_vVelocity * fTimeDelat;

			i->_fAge += fTimeDelat;


			if (i->_fAge > i->_fLifeTime) // kill 
				i->_bIsAlive = false;
		}
	}

	__super::Update_Particle(fTimeDelat);
}

void CParticleSplashLeft::Render_GameObject()
{
	if (!m_ParticlesList.empty())
	{
		//
		// set render states
		//

		m_pTexture->Set_Texture((_uint)m_fFrame);
		m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
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

void CParticleSplashLeft::Create_Texture(const _tchar* pTexturePath, _int iMaxFrame)
{
	m_iPicNum = iMaxFrame;
	m_pTexture = CTexture::Create(m_pGraphicDev, TEX_NORMAL, pTexturePath, iMaxFrame);
}

CParticleSplashLeft* CParticleSplashLeft::Create(IDirect3DDevice9* pDevice, _vec3 vPos, const _tchar* pTextruePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleSplashLeft* pInstance = new CParticleSplashLeft(&vPos, iCount, fSize);

	if (FAILED(pInstance->Ready_Particle(pDevice)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CParticleSplash Create Failed");
		return nullptr;
	}

	pInstance->Create_Texture(pTextruePath, iMaxFrame);

	return pInstance;
}
