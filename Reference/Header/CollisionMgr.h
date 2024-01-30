#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollisionMgr
{
public:
	CCollisionMgr();
	~CCollisionMgr();

public:
	//static CGameObject* Collision_GameObject(CGameObject* pSrc, CGameObject* pDst);

};

END