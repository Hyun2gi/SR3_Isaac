#include "stdafx.h"
#include "MapObj.h"

#include "Item.h"

#include "BrimStone.h"
#include "Epic.h"

#include "SadOnion.h"
#include "WhipWorm.h"

#include "Coin.h"
#include "Heart.h"
#include "HeartHalf.h"
#include "Pill.h"

#include "Export_Utility.h"

CMapObj::CMapObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),
	m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTargetTransCom(nullptr), m_pTextureCom(nullptr), m_pCalculator(nullptr),
	m_iHitCount(0), m_iLimitHit(0), m_iPicNum(1), m_fFrameSpeed(0.f), m_fCallLimit(0.f), m_fAccTimeDelta(0.f),
	m_bDead(false), m_bItemDrop(false), m_bHit(false), m_eDropItem(COIN), m_eObjType(OBJECT_TYPE_END), m_eObjID(MOBJID_NONE)
{
}

CMapObj::CMapObj(const CMapObj& rhs)
	: Engine::CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom), m_pTransformCom(rhs.m_pTransformCom), m_pTargetTransCom(rhs.m_pTargetTransCom),
	m_pTextureCom(rhs.m_pTextureCom), m_pCalculator(rhs.m_pCalculator),
	m_iHitCount(rhs.m_iHitCount), m_iLimitHit(rhs.m_iLimitHit), m_iPicNum(rhs.m_iPicNum), m_fFrameSpeed(rhs.m_fFrameSpeed),
	m_fCallLimit(rhs.m_fCallLimit), m_fAccTimeDelta(rhs.m_fAccTimeDelta),
	m_bDead(rhs.m_bDead), m_bItemDrop(rhs.m_bItemDrop), m_bHit(rhs.m_bHit), m_eDropItem(rhs.m_eDropItem), m_eObjType(rhs.m_eObjType), m_eObjID(rhs.m_eObjID)
{
}

CMapObj::~CMapObj()
{
}

HRESULT CMapObj::Ready_GameObject()
{
	return S_OK;
}

_int CMapObj::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CMapObj::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMapObj::Render_GameObject()
{
}

HRESULT CMapObj::Add_Component()
{
	return S_OK;
}

bool CMapObj::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

void CMapObj::Setting_ItemTag()
{
	switch (m_eDropItem)
	{
	case Engine::BRIM:
		m_wstrDropItemTag = L"BrimStone";
		break;
	case Engine::EPIC:
		m_wstrDropItemTag = L"Epic";
		break;
	case Engine::SAD_ONION:
		m_wstrDropItemTag = L"SadOnion";
		break;
	case Engine::TRINKET:
		m_wstrDropItemTag = L"WhipWorm";
		break;
	case Engine::COIN:
		m_wstrDropItemTag = L"Coin";
		break;
	case Engine::HEART:
		m_wstrDropItemTag = L"Heart";
		break;
	case Engine::HEART_HALF:
		m_wstrDropItemTag = L"HeartHalf";
		break;
	case Engine::PILL:
		m_wstrDropItemTag = L"Pill";
		break;
	default:
		break;
	}
}

CItem* CMapObj::Create_Item(ITEM_TYPE eItemType, _int iSpawnPos, CLayer* pLayer)
{
	_vec3 vPos, vLook;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

	switch (eItemType)
	{
	case Engine::BRIM:
	{
		CItem* pItem = CBrimStone::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::EPIC:
	{
		CItem* pItem = CEpic::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::SAD_ONION:
	{
		CItem* pItem = CSadOnion::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::TRINKET:
	{
		CItem* pItem = CWhipWorm::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::COIN:
	{
		CItem* pItem = CCoin::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::HEART:
	{
		CItem* pItem = CHeart::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::HEART_HALF:
	{
		CItem* pItem = CHeartHalf::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	case Engine::PILL:
	{
		CItem* pItem = CPill::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}
	}
}

CMapObj* CMapObj::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMapObj* pInstance = new CMapObj(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MapObj Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMapObj::Free()
{
	__super::Free();
}
