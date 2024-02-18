#include "stdafx.h"
#include "SlotCard.h"

#include "Export_Utility.h"

CSlotCard::CSlotCard(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMapObj(pGraphicDev)
{
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);
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

	m_fCallLimit = 0.7f; // 0.7

	m_iReward = 0;
	m_iNoLuckNum = 0;

	m_bStart = false;
	m_bRandom = false;
	m_bReward = false;

	m_bResult = false;

	m_ePreState = CARD_END;

	return S_OK;
}

_int CSlotCard::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		return 1;

	if (!m_bStart) // 처음 시작 시 카드 고정
	{
		m_eCurState = CARD_IDLE;
		Setting_FirstCard();
		m_bStart = true;
	}

	CGameObject::Update_GameObject(fTimeDelta);

	if(m_bRandom) // 플레이어와 충돌한 슬롯머신이 카드의 m_bRandom을 true로 만들어준 상황
	{
		m_eCurState = CARD_RANDOM;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SlotCardRandTexture"));
		m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed; // 휘리릭~

		if (m_iPicNum < m_fFrame)
			m_fFrame = 0.f;
	}
	else if(m_bStart && !m_bRandom)
	{
		m_eCurState = CARD_IDLE;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SlotCardTexture"));
		Setting_Result();
	}

	return 0;
}

void CSlotCard::LateUpdate_GameObject()
{
	//Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

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

	// Card
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlotCardTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SlotCardTexture", pComponent });

	// Random Card
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlotCardRandTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SlotCardRandTexture", pComponent });

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
			m_iPicNum = 1;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SlotCardTexture"));
			break;

		case CSlotCard::CARD_RANDOM:
			m_iPicNum = 6;
			m_fFrameSpeed = 3.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SlotCardRandTexture"));
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

void CSlotCard::Setting_Result()
{
	// 슬롯머신의 결과 m_iReward 를 통해 카드 텍스쳐를 결정
	switch (m_iReward)
	{
	case 0: // 꽝
		m_fFrame = m_iNoLuckNum + m_iIndex;
		break;
	case 1: // 코인
		m_fFrame = 3.3f;
		break;
	case 2: // 하트
		m_fFrame = 6.3f;
		break;
	default:
		break;
	}
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
