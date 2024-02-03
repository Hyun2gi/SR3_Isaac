#include "stdafx.h"
#include "..\Header\Item.h"

#include "Export_Utility.h"


CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:Engine::CGameObject(pGraphicDev), 
	m_pCalculCom(nullptr),m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f), m_bDead(false), m_iTimer(0)
{
}

CItem::CItem(const CItem& rhs)
	: Engine::CGameObject(rhs),
	m_pCalculCom(nullptr), m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f), m_bDead(false), m_iTimer(0)
{
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_GameObject()
{
	return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CItem::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CItem::Render_GameObject()
{
}

void CItem::Run_Item_Effect()
{
}

void CItem::Set_Item_SpawnSpot(int _spawnspot)
{
	switch (_spawnspot)
	{
	case 0:
		m_eCurItemPlace = SP_IDLE;
		break;
	case 1:
		m_eCurItemPlace = SP_SLOT;
		break;
	case 2 :
		m_eCurItemPlace = SP_OBJECT;
		m_fMoveSpeed = 0.9;
		break;
	case 3:
		m_eCurItemPlace = SP_SHOP;
		break;
	}
}

void CItem::Item_Spawn_Action()
{
}

HRESULT CItem::Add_Component()
{
	return S_OK;
}

bool CItem::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CItem* pInstance = new CItem(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Item Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();
}
