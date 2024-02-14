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

		//if (Check_Time(fTimeDelta)) // 랜덤인 동안 일정 시간이 될 때까지 프레임 돌고 일정 시간 되면 랜덤 = false
		//{
		//	m_bRandom = false;
		//	m_eCurState = CARD_IDLE;
		//	Check_Result(); // 보상 체크
		//	// 랜덤 체크를 카드가 아니라 슬롯머신에서 할 수 있도록
		//}
	}
	else if(m_bStart && !m_bRandom)// Start, Result 상태인 경우에는 한 카드만 보여주기
	{
		m_eCurState = CARD_IDLE; // 기본 카드 텍스쳐 사용
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SlotCardTexture")); // 머얌
		Setting_Result();
	}

	// 카드에서 m_bResult의 존재 의의 : 내부적으로 영향을 끼치지는 X
	// 카드의 m_bResult가 true인 경우 충돌처리 시 아이템 드랍->false로 바꿔줌으로써 아이템을 여러번 드랍하지 않도록 막아주는?

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
			m_iPicNum = 1; // 7  ---> 카드 하나만 사용
			//m_fFrameSpeed = 1.f;
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

void CSlotCard::Check_Result()
{
	int iResult = rand() % 10;

	switch (iResult)
	{
	case 0: // 꽝
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		m_fFrame = rand() % 4 + m_iIndex;
		m_iReward = 0;
		m_bReward = false;
		break;
	case 6: // 코인
	case 7:
		m_fFrame = 2.8f;
		m_iReward = 1;
		m_bReward = true; // 당첨된 경우 m_bReward = true
		break;
	case 8: // 하트
	case 9:
		m_fFrame = 5.8f;
		m_iReward = 2;
		m_bReward = true;
		break;
	default:
		break;
	}

	//m_bReward = true;
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
