#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleScatter : public CParticleSystem
{
public:
	explicit CParticleScatter(BoundingBox* boundingBox, int numParticles);
	virtual ~CParticleScatter();

public:
	bool Ready_Particle(IDirect3DDevice9* pDevice) override;
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;

private:
	virtual void	Free() override;
};
END