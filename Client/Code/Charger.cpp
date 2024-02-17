#include "stdafx.h"
#include "Charger.h"

#include "Export_System.h"
#include "Export_Utility.h"

CCharger::CCharger(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CCharger::CCharger(const CCharger& rhs)
	: CMonster(rhs)
{
}

CCharger::~CCharger()
{
}

HRESULT CCharger::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 0.5f, 10.f);

	m_iHp = 3;

	m_fCallLimit = 0.1f;
	m_fSpeed = 2.f;

	m_ePreState = CHARGER_END;

	m_eMstType = CHARGER;

	return S_OK;
}

_int CCharger::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bCreate)
	{
		if (Check_Time(fTimeDelta))
			Create_Start_Particle(0.f);
	}

	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	else
		m_fSlowDelta = 0.f;

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	Check_Range();

	_vec3 vTargetPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vTargetPos);

	m_pTransformCom->Chase_Target(&vTargetPos, m_fSpeed, m_fSlowDelta);

	Face_Camera();

	CGameObject::Update_GameObject(m_fSlowDelta);

	Check_Outof_Map();

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.1f, 20);

		int soundrand = rand() % 4;
		if (soundrand == 0)
		{
			Engine::PlayEffect(L"blood fire 3.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 1)
		{
			Engine::PlayEffect(L"blood fire 2.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 2)
		{
			Engine::PlayEffect(L"blood fire 1.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 3)
		{
			Engine::PlayEffect(L"blood fire.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		return 1;
	}

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CCharger::LateUpdate_GameObject()
{
	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);

		m_bHit = false;

		if (0 >= m_iHp)
			m_bDead = true;
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CCharger::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CCharger::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ChargerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ChargerTexture", pComponent });

	// ATTACK
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ChargerAttackTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ChargerAttackTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CCharger::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CCharger::CHARGER_IDLE:
			m_iPicNum = 4;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_ChargerTexture"));
			break;

		case CCharger::CHARGER_ATTACK:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_ChargerAttackTexture"));
			Engine::PlayEffect(L"charger.wav", SOUND_EFFECT_MON_STOPSUDDEN, 0.8f);
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CCharger::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CCharger::Check_Range()
{
	_vec3 vTargetPos, vPos;

	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &vTargetPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fDistance =
		(fabs(vTargetPos.x - vPos.x) * fabs(vTargetPos.x - vPos.x)) +
		(fabs(vTargetPos.y - vPos.y) * fabs(vTargetPos.y - vPos.y));

	if (10 < fDistance) // 범위에 속하지 않을 때
	{
		m_eCurState = CHARGER_IDLE;
		m_fSpeed = 2.f;
	}
	else // 범위에 속할 때
	{
		m_eCurState = CHARGER_ATTACK;
		m_fSpeed = 6.f;
	}
}

CCharger* CCharger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCharger* pInstance = new CCharger(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Charger Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCharger::Free()
{
	__super::Free();
}
