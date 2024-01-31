#include "stdafx.h"
#include "MapObj.h"

#include "Export_Utility.h"

CMapObj::CMapObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),
	m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTargetTransCom(nullptr), m_pTextureCom(nullptr), m_pCalculator(nullptr),
	m_iHitCount(0), m_iLimitHit(0), m_iPicNum(1), m_fFrameSpeed(0.f)
{
}

CMapObj::CMapObj(const CMapObj& rhs)
	: Engine::CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom), m_pTransformCom(rhs.m_pTransformCom), m_pTargetTransCom(rhs.m_pTargetTransCom),
	m_pTextureCom(rhs.m_pTextureCom), m_pCalculator(rhs.m_pCalculator),
	m_iHitCount(rhs.m_iHitCount), m_iLimitHit(rhs.m_iLimitHit), m_iPicNum(rhs.m_iPicNum), m_fFrameSpeed(rhs.m_fFrameSpeed)
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
