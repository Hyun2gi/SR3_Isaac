#pragma once
#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

//class CParticleSystem;

//template<typename T>
class ENGINE_DLL CAbstractFactory : public CBase
{
public:
	static DWORD FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

	//static CParticleSystem* Create

public:
	CAbstractFactory() {}
	~CAbstractFactory() {}
};

END