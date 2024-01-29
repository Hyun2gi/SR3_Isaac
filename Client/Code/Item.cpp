#include "stdafx.h"
#include "..\Header\Item.h"

#include "Export_Utility.h"


CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:Engine::CGameObject(pGraphicDev), 
	m_pCalculCom(nullptr),m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f)
{
}

CItem::CItem(const CItem& rhs)
	: Engine::CGameObject(rhs),
	m_pCalculCom(nullptr), m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f)
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
