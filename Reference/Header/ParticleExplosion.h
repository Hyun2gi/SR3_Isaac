#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleExplosion : public CParticleSystem
{
public:
	explicit CParticleExplosion(_vec3* vOrigin, int numParticles, _float fSize);
	virtual ~CParticleExplosion();

public:
	bool Ready_Particle(IDirect3DDevice9* pDevice) override;
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;

	void	Set_Velocity_Range(_vec3 vMin, _vec3 vMax)
	{
		m_vMinVelocity = vMin;
		m_vMaxVelocity = vMax;
	}

public:
	static CParticleExplosion* Create(IDirect3DDevice9* pDevice, _vec3 vPos, _float fSize = 0.25f, _int iCount = 10);

private:
	virtual void	Free() override;

private:
	_vec3 m_vMinVelocity, m_vMaxVelocity;
};
END