#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleExplosion : public CParticleSystem
{
public:
	explicit CParticleExplosion(_vec3* vOrigin, int numParticles);
	virtual ~CParticleExplosion();

public:
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;

private:
	virtual void	Free() override;
};
END