#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleScatter : public CParticleSystem
{
public:
	explicit CParticleScatter(BoundingBox* boundingBox, int numParticles, _float fSize);
	virtual ~CParticleScatter();

public:
	bool Ready_Particle(IDirect3DDevice9* pDevice) override;
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;

public:
	static CParticleScatter* Create(IDirect3DDevice9* pDevice, _vec3 vMin, _vec3 vMax, _float fSize = 0.25f, _int iCount = 30);

private:
	virtual void	Free() override;
};
END