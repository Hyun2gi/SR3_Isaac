#include "stdafx.h"
#include "SlotMC.h"

#include "Export_System.h"
#include "Export_Utility.h"

CSlotMC::CSlotMC(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pMachine(nullptr)
{
}

CSlotMC::CSlotMC(const CSlotMC& rhs)
	: CMapObj(rhs),
	m_pMachine(rhs.m_pMachine)
{
}

CSlotMC::~CSlotMC()
{
}

HRESULT CSlotMC::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(15.f, 1.f, 5.f);

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

	if (m_pMachine != nullptr)
		m_pMachine->Update_GameObject(fTimeDelta);

	if (Engine::Get_DIKeyState(DIK_Z) & 0x80)
	{
		m_pMachine->Set_Game();
	}

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSlotMC::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pMachine != nullptr)
		m_pMachine->LateUpdate_GameObject();
}

void CSlotMC::Render_GameObject()
{
	if (m_pMachine != nullptr)
		m_pMachine->Render_GameObject();
}

HRESULT CSlotMC::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region SlotMC Texture

	// IDLE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlotMCTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SlotMCTexture", pComponent });

	// BROKEN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BrokenSlotMCTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_BrokenSlotMCTexture", pComponent });

#pragma endregion SlotMC Texture

#pragma region Card Texture

	// Card
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlotCardTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SlotCardTexture", pComponent });

	// Random Card
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlotCardRandTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SlotCardRandTexture", pComponent });

#pragma endregion Card Texture

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
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pMachine = CMachine::Create(m_pGraphicDev);
	m_pMachine->Set_MyLayer(m_vecMyLayer[0]);
	m_pMachine->Get_TransformCom()->Set_Pos(vPos);

	if (m_pMachine == nullptr)
		return;
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
	Safe_Release<CMachine*>(m_pMachine);
	m_pMachine = nullptr;

	__super::Free();
}
