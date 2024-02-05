#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class CTexture;

class ENGINE_DLL CParticleSplash : public CParticleSystem
{
public:
	explicit CParticleSplash(_vec3* vOrigin, int numParticles);
	virtual ~CParticleSplash();

public:
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;
	virtual void Render() override;

	void	Create_Texture(const _tchar* pTexturePath, _int iMaxFrame);

private:
	virtual void	Free() override;

private:
	bool m_bIsLeft;
	_float m_fGravity;

	CTexture* m_pTexture;

	_int					m_iPicNum;
	_float					m_fFrame = 0.f;
	_float					m_fFrameSpeed = 2.f;
};
END