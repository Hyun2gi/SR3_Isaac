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

void CSlotMC::Set_Machine_ToStage(CLayer* pLayer)
{
	if(m_bCreate)
		pLayer->Add_GameObject(L"Machine", m_pMachine);
}

HRESULT CSlotMC::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(25.f, 1.f, 20.f);

	m_iLimitHit = 4;

	m_bCreate = false;
	m_bGame = false;

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

	if (!m_pCardList.empty())
	{
		for (auto& iter : m_pCardList)
		{
			iter->Update_GameObject(fTimeDelta);
		}
	}

	if (m_pMachine->Get_Dead())
		m_bGame = true;
	else
		m_bGame = false;

	if (m_bGame)
	{
		m_pMachine->Set_Game();

		for (auto& iter : m_pCardList)
		{
			iter->Set_Random();
		}
	}

	//if (m_pMachine != nullptr && !(m_pMachine->Get_Dead()))
	//{
	//	if (Engine::Get_DIKeyState(DIK_G) & 0x80)// 이후 플레이어와의 충돌로 변경
	//	{
	//		
	//	}
	//}

	return 0;
}

void CSlotMC::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pMachine != nullptr)
		m_pMachine->LateUpdate_GameObject();

	if (!m_pCardList.empty())
	{
		for (auto& iter : m_pCardList)
		{
			iter->LateUpdate_GameObject();
		}
	}
}

void CSlotMC::Render_GameObject()
{
	if (m_pMachine != nullptr)
		m_pMachine->Render_GameObject();

	if (!m_pCardList.empty())
	{
		for (auto& iter : m_pCardList)
		{
			iter->Render_GameObject();
		}
	}
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
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fScalar = SCALAR_X;

	for (int i = 0; i < 3; ++i)
	{
		CSlotCard* pCard = CSlotCard::Create(m_pGraphicDev);
		pCard->Set_MyLayer(m_vecMyLayer[0]);
		pCard->Get_TransformCom()->Set_Pos(vPos.x + fScalar, vPos.y - SCALAR_Y, vPos.z - 0.1f);
		pCard->Set_Index(i);
		m_pCardList.push_back(pCard);
		fScalar += 0.73f;
	}
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

	for_each(m_pCardList.begin(), m_pCardList.end(), CDeleteObj());

	__super::Free();
}
