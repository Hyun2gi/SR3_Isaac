#include "stdafx.h"
#include "SlotCard.h"

#include "Export_Utility.h"

CSlotCard::CSlotCard(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMapObj(pGraphicDev)
{
}

CSlotCard::CSlotCard(const CSlotCard& rhs)
    : CMapObj(rhs)
{
}

CSlotCard::~CSlotCard()
{
}

HRESULT CSlotCard::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 0.7f, 0.7f, 0.7f };

	m_fCallLimit = 0.7f;

	m_iReward = 0;

	m_bStart = false;
	m_bRandom = false;
	m_bReward = false;

	m_ePreState = CARD_END;

	return S_OK;
}

_int CSlotCard::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return 1;

	if (!m_bStart)
	{
		m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotCardTexture"));

		Setting_FirstCard();
		m_bStart = true;
	}

	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);

	CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bRandom)
	{
		m_eCurState = CARD_IDLE;
	}
	else
	{
		m_eCurState = CARD_RANDOM;
	}

	if (CARD_RANDOM == m_eCurState)
	{
		m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotCardRandTexture"));
		m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

		if (m_iPicNum < m_fFrame)
			m_fFrame = 0.f;

		if (Check_Time(fTimeDelta))
		{
			m_bRandom = false;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotCardTexture"));

			Check_Result();
		}
	}
	//m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSlotCard::LateUpdate_GameObject()
{
	if(CARD_RANDOM == m_eCurState)
		Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CSlotCard::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CSlotCard::Add_Component()
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

void CSlotCard::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CSlotCard::CARD_IDLE:
			m_iPicNum = 7;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotCardTexture"));
			break;

		case CSlotCard::CARD_RANDOM:
			m_iPicNum = 6;
			m_fFrameSpeed = 3.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotCardRandTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CSlotCard::Change_Scale()
{
}

void CSlotCard::Setting_FirstCard()
{
	switch (m_iIndex)
	{
	case 0:
		m_fFrame = 0.f;
		break;
	case 1:
		m_fFrame = 2.f;
		break;
	case 2:
		m_fFrame = 4.f;
		break;
	}
}

void CSlotCard::Check_Result()
{
	int iResult = rand() % 10;

	switch (iResult)
	{
	case 0: // ²Î
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		m_fFrame = rand() % 5 + m_iIndex;
		m_iReward = 0;
		m_bReward = false;
		break;
	case 6: // ÄÚÀÎ
	case 7:
		m_fFrame = 3.f;
		m_iReward = 1;
		m_bReward = true;
		m_eCurState = CARD_IDLE;
		break;
	case 8: // ÇÏÆ®
	case 9:
		m_fFrame = 6.f;
		m_iReward = 2;
		m_bReward = true;
		m_eCurState = CARD_IDLE;
		break;
	default:
		break;
	}

	//m_bReward = true;
}

CSlotCard* CSlotCard::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSlotCard* pInstance = new CSlotCard(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("FirstCard Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSlotCard::Free()
{
	__super::Free();
}
