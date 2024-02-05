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
	Free();
}

void CParticleScatter::Free()
{
	Engine::Safe_Release(m_pVb);
	Engine::Safe_Release(m_pTex);
}

bool CParticleScatter::Ready_Particle(IDirect3DDevice9* pDevice)
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

	wstring wstr = L"../Bin/Resource/Texture/Particle/fingerbone.png";

	hr = D3DXCreateTextureFromFile(
		pDevice,
		wstr.c_str(),
		&m_pTex);

	FAILED_CHECK_RETURN(hr, E_FAIL);

	return S_OK;
}

void CParticleScatter::Reset_Partice(Attribute* attribute)
{
	attribute->_bIsAlive = true;

	GetRandomVector(
		&attribute->_vPosition,
		&m_tBoundingBox._min,
		&m_tBoundingBox._max);

	attribute->_vPosition.y = m_tBoundingBox._max.y;

	attribute->_vVelocity.x = GetRandomFloat(0.0f, 1.f) * -5.0f;
	attribute->_vVelocity.y = GetRandomFloat(0.0f, 1.f) * -5.0f;
	attribute->_vVelocity.z = GetRandomFloat(-0.5f, 0.7f) * -5.0f;

	attribute->_color = D3DXCOLOR(
		0.5f,
		0.5f,
		0.5f,
		0.7f);
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