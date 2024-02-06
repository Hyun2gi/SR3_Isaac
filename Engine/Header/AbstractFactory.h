#pragma once
#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

template<typename T>
class ENGINE_DLL CAbstractFactory : public CBase
{
public:
	static CObj* Create()
	{
		CObj* pObj = new T;
		pObj->Initialize();

		return pObj;
	}

public:
	CAbstractFactory() {}
	~CAbstractFactory() {}
};

END