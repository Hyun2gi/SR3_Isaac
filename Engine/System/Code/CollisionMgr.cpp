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
//	// pSrc ����
//	// pDst ���
//
//	CTransform* pSrcTrans = dynamic_cast<CTransform*>(pSrc->Get_Component(ID_DYNAMIC, L"Transform"));
//	CTransform* pDstTrans = dynamic_cast<CTransform*>(pDst->Get_Component(ID_DYNAMIC, L"Transform"));
//	
//	// z�� ũ��� �������� �ʱ� ������ x�� ������ ������ ���� ������ �� ����.
//	//pSrcTrans->m_vScale.x
//
//	//if()
//
//
//	return CGameObject();
//}