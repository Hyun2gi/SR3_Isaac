#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CParticleExplosion : public CParticleSystem
{
public:
	explicit CParticleExplosion(int numParticles, _float fSize, _float fSpeed);
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
	static CParticleExplosion* Create(IDirect3DDevice9* pDevice, 
		_matrix matWorld, 
		_float fSize = 0.25f, 
		_int iCount = 10, _float iSpeed = 2.f,
		const _tchar* pTextruePath = L"../Bin/Resource/Texture/Particle/explosion.png");

private:
	virtual void	Free() override;

private:
	_float m_fSpeed;

	_vec3 m_vMinVelocity, m_vMaxVelocity;
	_matrix m_matWorld;

	wstring m_wstrTexturePath;
};
END