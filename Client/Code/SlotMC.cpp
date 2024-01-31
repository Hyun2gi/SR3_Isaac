#include "stdafx.h"
#include "SlotMC.h"

#include "Export_Utility.h"

CSlotMC::CSlotMC(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CSlotMC::CSlotMC(const CSlotMC& rhs)
	: CMapObj(rhs)
{
}

CSlotMC::~CSlotMC()
{
}

HRESULT CSlotMC::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(5.f, 1.f, 10.f);

	m_iLimitHit = 4;

	m_bCreate = false;

	return S_OK;
}

_int CSlotMC::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bCreate)
	{
		Create_Machine();
		Create_Card();
		m_bCreate = true;
	}



	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSlotMC::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();


}

void CSlotMC::Render_GameObject()
{
}

HRESULT CSlotMC::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CSlotMC::Create_Machine()
{
}

void CSlotMC::Create_Card()
{
}

CSlotMC* CSlotMC::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSlotMC* pInstance = new CSlotMC(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SlotMC Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSlotMC::Free()
{
	__super::Free();
}
