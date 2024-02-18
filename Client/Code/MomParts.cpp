#include "stdafx.h"
#include "MomParts.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Squirt.h"
#include "Leaper.h"
#include "Pacer.h"

CMomParts::CMomParts(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
	: CMonster(pGraphicDev),
	m_pMom(nullptr), m_pLayer(nullptr)
{
	m_iIndex = iIndex;

	DWORD dwSeed = (m_iIndex << 16) | (time(NULL) % 1000);
	srand(dwSeed);
}

CMomParts::CMomParts(const CMomParts& rhs)
	: CMonster(rhs),
	m_pMom(rhs.m_pMom), m_pLayer(rhs.m_pLayer)
{
}

CMomParts::~CMomParts()
{
}

HRESULT CMomParts::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 7.f, 12.f, 1.f };

	m_iRandNum = 5;
	m_iRandNumMstCreate = 0;

	m_fCallLimit = 0.1f;
	m_fSpeed = 10.f;

	m_ePreState = MOM_END;

	m_bScaleReduce = false;
	m_bScaleChange = false;
	m_bMstCreate = false;
	m_bCheckCreate = false;
	m_iScaleCount = 0;

	m_bBoss = true;
	m_eBossType = MOM_PARTS;

	return S_OK;
}

_int CMomParts::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsLoadCreatEnd)
		return 0;

	if (!m_pMom) return 0;

	if (m_bDead)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);

		Engine::Create_Splash_Left(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), L"../Bin/Resource/Texture/Particle/BloodExp_Left/BloodExp_%d.png", 2, 2.f, 20);
		Engine::Create_Splash_Right(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), L"../Bin/Resource/Texture/Particle/BloodExp_Right/BloodExp_%d.png", 2, 2.f, 20);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 10.f, 30);

		return 1;
	}

	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	else
		m_fSlowDelta = 0.f;

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
	{
		if (MOM_HAND == m_eCurState)
		{
			//m_eCurState = MOM_DOOR;
			m_fFrame = 2.9f;
		}
		else
			m_fFrame = 0.f;
	}

	if (!m_bCreate)
	{
		if (Check_Time(m_fSlowDelta))
		{
			Create_Start_Particle(3.f);
			m_eCurState = MOM_DOOR;
		}
	}

	Set_RandNum();

	if (Check_Time(m_fSlowDelta))
	{
		m_fCallLimit = (_float)m_iRandNum;

		Change_State();
		
		Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 10.f, 20);

		m_bCheckCreate = true;

		//Check_CreateMst(); // 잡몹 생성 여부 판단
	}

	if (m_bCheckCreate) // 잡몹 생성 여부 판단 가능할 때
	{
		Check_CreateMst(); // 잡몹 생성 여부 판단
		m_bCheckCreate = false;
	}

	if (m_bMstCreate) // 잡몹 생성 패턴
	{
		Create_Mst(m_vecCreatePos); // 이 함수가 엄청 많이 실행되는 경우가 있음

		Engine::PlayEffect(L"summonsound.wav", SOUND_EFFECT_BOSS_STOPSUDDEN_SUB, 1.f);

		m_bMstCreate = false;
	}

	if (m_bScaleChange) // 상태 변환 시 애니메이션
		Animation_Change();

	if (m_pMom->Get_Dead()) // Mom 사망 시 Parts도 Dead 처리
		m_bDead = true;

	CGameObject::Update_GameObject(m_fSlowDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
	}

void CMomParts::LateUpdate_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	if (!m_pMom) return;

	if (m_bHit) // 피격 시 Mom 의 HP 를 깎아야 함
	{
		if (m_eCurState == MOM_EYE || m_eCurState == MOM_HAND || m_eCurState == MOM_SKIN)
		{
			m_bHit = false;
			m_bHitColor = true;
			//m_pMom->Set_Hp_Minus();
			m_pMom->Hit();
		}
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMomParts::Render_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMomParts::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// DOOR
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomDoorTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomDoorTexture", pComponent });

	// EYE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomEyeTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomEyeTexture", pComponent });

	// SKIN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomSkinTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomSkinTexture", pComponent });

	// HAND
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomHandTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomHandTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMomParts::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CMomParts::MOM_DOOR:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MomDoorTexture"));
			break;

		case CMomParts::MOM_EYE:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MomEyeTexture"));
			break;

		case CMomParts::MOM_SKIN:
			m_iPicNum = 2;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MomSkinTexture"));
			break;

		case CMomParts::MOM_HAND:
			m_iPicNum = 3;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MomHandTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CMomParts::Set_RandNum()
{
	m_iRandNum = rand() % 6;
	m_iRandNumMstCreate = rand() % 5; // 3
}

void CMomParts::Change_State()
{
	if (0 == m_iRandNum)
	{
		m_eCurState = MOM_EYE;
		m_bScaleChange = true;
	}
	else if (1 == m_iRandNum)
	{
		m_eCurState = MOM_SKIN;
		m_bScaleChange = true;
	}
	else if (2 == m_iRandNum)
	{
		m_eCurState = MOM_HAND;
		m_bScaleChange = true;
	}
	else if(3 == m_iRandNum)
		m_eCurState = MOM_DOOR;
}

void CMomParts::Animation_Change()
{
	// 총 두 번 말랑말랑
	if (2 == m_iScaleCount)
	{
		m_bScaleChange = false;
		m_pTransformCom->m_vScale = { ORIGIN_SCALE_X, 12.f, 1.f };
		m_iScaleCount = 0;
	}

	_vec3 vScale;
	vScale = m_pTransformCom->m_vScale;

	if (m_bScaleReduce)
	{
		vScale.x -= 0.2f;

		if (vScale.x <= ORIGIN_SCALE_X - 1.f)
		{
			m_bScaleReduce = false;
			vScale.x = ORIGIN_SCALE_X - 1.f;
		}
	}
	else
	{
		vScale.x += 0.2f;

		if (vScale.x >= ORIGIN_SCALE_X + 1.f)
		{
			m_bScaleReduce = true;
			vScale.x = ORIGIN_SCALE_X + 1.f;
			++m_iScaleCount;
		}
	}
	m_pTransformCom->m_vScale = vScale;
}

void CMomParts::Check_CreateMst()
{
	if (MOM_DOOR != m_eCurState && MOM_END != m_eCurState) // Parts 상태가 hand, skin, eye중 하나 일 때
	{
		if (0 == m_iRandNumMstCreate) // 3분의 1 확률로 몬스터 생성
		{
			m_pTransformCom->Get_Info(INFO_POS, &m_vecCreatePos);
			m_bMstCreate = true; // 몬스터 생성 가능할 때만 true

			// 동서남북에 따라 생성 위치 바꿔주기
			switch (m_iIndex)
			{
			case 0: // 상
				m_vecCreatePos.z -= 5.f;
				m_bCheckCreate = false;
				break;
			case 1: // 우
				m_vecCreatePos.x -= 5.f;			
				m_bCheckCreate = false;
				break;
			case 2: // 하
				m_vecCreatePos.z += 5.f;
				m_bCheckCreate = false;		
				break;
			case 3: // 좌
				m_vecCreatePos.x += 5.f;
				m_bCheckCreate = false;
				break;
			}
		}
		else
			m_bMstCreate = false;
	}
	m_bCheckCreate = false;
}	

void CMomParts::Create_Mst(_vec3 vPos)
{
	int iCreateMst = rand() % 3;

	switch (iCreateMst)
	{
	case 0: // Squirt
	{
		CSquirt* pSquirt = CSquirt::Create(m_pGraphicDev, 0);
		pSquirt->Get_Transform()->Set_Pos(vPos.x, 1.2f, vPos.z);
		pSquirt->Set_MyLayer(m_vecMyLayer[0]);
		m_pLayer->Add_GameObject(L"Squirt", pSquirt);
		m_bMstCreate = false;
		break;
	}
	case 1: // Leaper
	{
		CLeaper* pLeaper = CLeaper::Create(m_pGraphicDev, 0);
		pLeaper->Get_Transform()->Set_Pos(vPos.x, 0.4f, vPos.z);
		pLeaper->Set_MyLayer(m_vecMyLayer[0]);
		m_pLayer->Add_GameObject(L"Leaper", pLeaper);
		m_bMstCreate = false;
		break;
	}
	case 2: // Pacer
	{
		CPacer* pPacer = CPacer::Create(m_pGraphicDev, 0);
		pPacer->Get_Transform()->Set_Pos(vPos.x, 0.4f, vPos.z);
		pPacer->Set_MyLayer(m_vecMyLayer[0]);
		m_pLayer->Add_GameObject(L"Pacer", pPacer);
		m_bMstCreate = false;
		break;
	}default:
		m_bMstCreate = false;
		break;
	}
}

void CMomParts::Setting_Value()
{
	switch (m_iIndex)
	{
	case 0: // 상
		m_pTransformCom->Set_Pos(VTXCNTX * 0.5f, INTERVAL_Y, VTXCNTZ - INTERVAL);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(0.f));
		break;
	case 1: // 우
		m_pTransformCom->Set_Pos(VTXCNTX - INTERVAL, INTERVAL_Y, VTXCNTZ * 0.5f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f));
		break;
	case 2: // 하
		m_pTransformCom->Set_Pos(VTXCNTX * 0.5f, INTERVAL_Y, INTERVAL);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f));
		break;
	case 3: // 좌
		m_pTransformCom->Set_Pos(0.f + INTERVAL, INTERVAL_Y, VTXCNTZ * 0.5f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f));
		break;
	}
}

CMomParts* CMomParts::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
	CMomParts* pInstance = new CMomParts(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Mom's Parts Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMomParts::Free()
{
	__super::Free();
}
