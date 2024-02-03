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
	m_bDead(false), m_bItemDrop(false)
{
}

CMapObj::CMapObj(const CMapObj& rhs)
	: Engine::CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom), m_pTransformCom(rhs.m_pTransformCom), m_pTargetTransCom(rhs.m_pTargetTransCom),
	m_pTextureCom(rhs.m_pTextureCom), m_pCalculator(rhs.m_pCalculator),
	m_iHitCount(rhs.m_iHitCount), m_iLimitHit(rhs.m_iLimitHit), m_iPicNum(rhs.m_iPicNum), m_fFrameSpeed(rhs.m_fFrameSpeed),
	m_fCallLimit(rhs.m_fCallLimit), m_fAccTimeDelta(rhs.m_fAccTimeDelta),
	m_bDead(rhs.m_bDead), m_bItemDrop(rhs.m_bItemDrop)
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
	if(!m_bItemDrop)
		CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CMapObj::LateUpdate_GameObject()
{
	if(!m_bItemDrop)
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

CItem* CMapObj::Create_Item(ITEM_TYPE eItemType, _int iSpawnPos, CLayer* pLayer)
{
	_vec3 vPos, vLook;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

	switch (eItemType)
	{
	case Engine::BRIM:
		//CItem* pItem = CBrimStone::Create(m_pGraphicDev, iSpawnPos, vPos);
		//pItem->Set_MyLayer(L"GameItem");

		break;
	case Engine::EPIC:
		break;
	case Engine::SAD_ONION:
		break;
	case Engine::TRINKET:
		break;

	case Engine::COIN:
	{
		CItem* pItem = CCoin::Create(m_pGraphicDev, iSpawnPos, vPos, vLook);
		pItem->Set_MyLayer(L"GameItem");
		return pItem;
		break;
	}

	case Engine::HEART:
		break;
	case Engine::HEART_HALF:
		break;
	case Engine::PILL_0:
		break;
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
