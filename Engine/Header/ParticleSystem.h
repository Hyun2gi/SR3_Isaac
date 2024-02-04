#pragma once
#include "Base.h"

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem : public CBase
{
protected:
	explicit CParticleSystem();
	virtual ~CParticleSystem();

public:
	virtual bool Ready_Particle(IDirect3DDevice9* pDevice, char* texFileName);
	virtual void Reset();
	virtual void Reset_Partice(Attribute* attribute) = 0;
	virtual void Add_Particle();
	virtual void Update_Particle(_float fTimeDelat) = 0;

	virtual void Pre_Render();
	virtual void Render();
	virtual void Post_Render();

	bool Is_Empty();
	bool Is_Dead();

	DWORD FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

protected:
	virtual void Remove_Dead_Particles();

protected:
	IDirect3DDevice9*			m_pGraphicDev;
	
	_vec3						m_vOrigin;
	
	BoundingBox					m_tBoundingBox;
	
	_float						m_fEmitRate;
	_float						m_fSize;

	IDirect3DTexture9*			m_pTex;
	IDirect3DVertexBuffer9*		m_pVb;
	
	list<Attribute>				m_ParticlesList;
	
	_int						m_iMaxParticles;

	DWORD						m_VbSize;
	DWORD						m_VbOffset;
	DWORD						m_VbBatchSize;

public:
	static CParticleSystem*		Create(const _float& fCallLimit);

private:
	virtual void	Free();
};

END