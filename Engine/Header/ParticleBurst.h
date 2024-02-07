#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class CTexture;

class ENGINE_DLL CParticleBurst : public CParticleSystem
{
public:
	explicit CParticleBurst(int numParticles, _float fSize);
	virtual ~CParticleBurst();

public:
	bool Ready_Particle(IDirect3DDevice9* pDevice) override;
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;
	virtual void Render_GameObject() override;

	void	Create_Texture();

	static CParticleBurst* Create(IDirect3DDevice9* pDevice, _matrix matWorld, _float fSize = 0.5f, _int iCount = 10);


private:
	virtual void	Free() override;

private:
	_float m_fGravity;

	CTexture* m_pTexture;

	_int					m_iPicNum;
	_float					m_fFrame = 0.f;
	_float					m_fFrameSpeed = 2.f;

	_int					m_iRandNum;
};
END