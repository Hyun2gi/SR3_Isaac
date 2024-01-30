#include "..\..\Header\CollisionMgr.h"
#include "GameObject.h"
#include "Transform.h"

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

//CGameObject* CCollisionMgr::Collision_GameObject(CGameObject* pSrc, CGameObject* pDst)
//{
//	// pSrc 본인
//	// pDst 상대
//
//	CTransform* pSrcTrans = dynamic_cast<CTransform*>(pSrc->Get_Component(ID_DYNAMIC, L"Transform"));
//	CTransform* pDstTrans = dynamic_cast<CTransform*>(pDst->Get_Component(ID_DYNAMIC, L"Transform"));
//	
//	// z의 크기는 존재하지 않기 때문에 x와 동일한 임의의 값을 가지게 할 예정.
//	//pSrcTrans->m_vScale.x
//
//	//if()
//
//
//	return CGameObject();
//}