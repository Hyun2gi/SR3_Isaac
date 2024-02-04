#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleScatter : public CParticleSystem
{
protected:
	explicit CParticleScatter();
	virtual ~CParticleScatter();

public:
	virtual void Reset_Partice(Attribute* attribute) override;
	virtual void Update_Particle(_float fTimeDelat) override;
//public:
//	Snow(BoundingBox* boundingBox, int numParticles);
//	void resetParticle(Attribute* attribute);
//	void update(float timeDelta);

private:
	virtual void	Free();
};
END