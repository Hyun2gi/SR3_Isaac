#pragma once
#include "Base.h"

#include "Engine_Define.h"

BEGIN(Engine)

//파티클 시스템 관련 구조체들
struct BoundingBox
{
	BoundingBox()
	{
		_min.x = (float)(1e+300 * 1e+300);
		_min.y = (float)(1e+300 * 1e+300);
		_min.z = (float)(1e+300 * 1e+300);

		_max.x = -(float)(1e+300 * 1e+300);
		_max.y = -(float)(1e+300 * 1e+300);
		_max.z = -(float)(1e+300 * 1e+300);
	};

	bool isPointInside(_vec3& p)
	{
		if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
			p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	_vec3 _min;
	_vec3 _max;
};

struct Particle
{
	_vec3 _position;
	D3DCOLOR    _color;
	const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct Attribute
{
	_vec3 _vPosition;		// 월드 내 파티클 위치
	_vec3 _vVelocity;		// 파티클의 속도 (초당 이동 단위)
	_vec3 _vAcceleration;	// 파티클의 가속 (초당 이동 단위)
	_float	_fLifeTime;		// 소멸까지의 시간
	_float	_fAge;			// 파티클의 현재 나이
	D3DXCOLOR _color;		// 컬러
	D3DXCOLOR _colorFade;	// 컬러가 시간의 흐름에 따라 퇴색하는 방법
	bool	_bIsAlive;		// 생존한 경유 true, 소멸 시 false
};

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

	void	Set_Velocity_Speed(_float fSpeed) { m_fVelocitySpeed = fSpeed; }

	DWORD FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound) // bad input
			return lowBound;

		// get random float in [0, 1] interval
		float f = (rand() % 10000) * 0.0001f;

		// return float in [lowBound, highBound] interval. 
		return (f * (highBound - lowBound)) + lowBound;
	}


	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max)
	{
		out->x = GetRandomFloat(min->x, max->x);
		out->y = GetRandomFloat(min->y, max->y);
		out->z = GetRandomFloat(min->z, max->z);
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

	_float						m_fVelocitySpeed;

private:
	virtual void	Free();
};

//class ENGINE_DLL Snow : public CParticleSystem
//{
//public:
//	Snow(BoundingBox* boundingBox, int numParticles);
//	void resetParticle(Attribute* attribute);
//	void update(float timeDelta);
//};

END