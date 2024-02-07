#include "stdafx.h"
#include "NormalFly.h"

#include "Export_System.h"
#include "Export_Utility.h"

CNormalFly::CNormalFly(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CMonster(pGraphicDev),
	m_pTargetTransCom(nullptr)
{
	m_iIndex = iIndex;
}

CNormalFly::CNormalFly(const CNormalFly& rhs)
	: CMonster(rhs),
	m_pTargetTransCom(rhs.m_pTargetTransCom)
{
}

CNormalFly::~CNormalFly()
{
}

HRESULT CNormalFly::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);

	m_iHp = 5;

	m_fSpeed = 1.f;

	m_fDistance = 4.f;
	m_fAngle = 30.f * m_iIndex;

	m_ePreState = FLY_END;

	m_iPicNum = 0;
	m_bDeadWait = false;

	m_eMstType = ATTACK_FLY;

	return S_OK;
}

_int CNormalFly::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
	{
		if (m_bDeadWait)
			m_bDead = true;
		else
			m_fFrame = 0.f;
	}

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);

		m_bHit = false;

		if (0 >= m_iHp)
		{
			m_eCurState = FLY_DEAD;
			m_bDeadWait = true;
		}
	}

	if (m_bHitColor)
		Change_Color(fTimeDelta);

	if(!m_bDeadWait)
		Revolve_Center();

	CGameObject::Update_GameObject(m_fSlowDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);
	//m_pCalculCom->Compute_Vill_Matrix_X(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CNormalFly::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CNormalFly::Render_GameObject()
{
	if (!m_bDead)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pTextureCom->Set_Texture((_uint)m_fFrame);

		m_pBufferCom->Render_Buffer();
	}
}

void CNormalFly::Revolve_Center()
{
	//m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"CenterFly", L"Proto_Transform"));

	//타임 델타 스케일 조절 예시 _ 적용, TimeDelta 대신 새로 생성한 Timer_Second 값을 사용하면 됨
	_float fSecondDelta = Engine::Get_TimeDelta(L"Timer_Second");

	_vec3 vPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPos);

	m_pTransformCom->Set_Pos(vPos.x + m_fDistance * cos(m_fAngle * (3.14 / 180.f)),
		vPos.y,
		vPos.z - m_fDistance * sin(m_fAngle * (3.14 / 180.f)));

	m_fAngle += 60.f * fSecondDelta;

	m_eCurState = FLY_IDLE;
}

HRESULT CNormalFly::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	//// IDLE
	//pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AttackFlyTexture"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_AttackFlyTexture", pComponent });

	//// DEAD
	//pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FlyDeadTexture"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_FlyDeadTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CNormalFly::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CNormalFly::FLY_IDLE:
			m_iPicNum = 2;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"AttackFly", L"Proto_AttackFlyTexture"));
			//m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_AttackFlyTexture"));
			// Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"MomParts", L"Proto_MomDoorTexture")
			break;

		case CNormalFly::FLY_DEAD:
			m_iPicNum = 11;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"AttackFly", L"Proto_FlyDeadTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CNormalFly* CNormalFly::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex, const _tchar* pLayerTag)
{
	CNormalFly* pInstance = new CNormalFly(pGraphicDev, iIndex);
	pInstance->Set_MyLayer(pLayerTag);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Normal Fly Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CNormalFly::Free()
{
	__super::Free();
}
