#pragma once
#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

//template<typename T>
class ENGINE_DLL CAbstractFactory : public CBase
{
public:
	static DWORD FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

public:
	CAbstractFactory() {}
	~CAbstractFactory() {}
};

END