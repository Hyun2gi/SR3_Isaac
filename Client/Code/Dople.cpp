#include "stdafx.h"
#include "Dople.h"

#include "Export_System.h"
#include "Export_Utility.h"

CDople::CDople(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CDople::CDople(const CDople& rhs)
	: CMonster(rhs)
{
}

CDople::~CDople()
{
}

HRESULT CDople::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };
	m_pTransformCom->Set_Pos(0.f, 1.5f, 0.f);

	m_fCallLimit = 1.f;
	m_fSpeed = 10.f;

	m_ePreState = DP_END;

	m_eMstType = DOPLE;

	return S_OK;
}

_int CDople::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (DP_DEAD == m_eCurState)
	{
		if(1.f != m_fFrame)
			m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

		if (m_iPicNum < m_fFrame)
			m_fFrame = 1.f;
	}
	else
	{
		m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

		if (m_iPicNum < m_fFrame)
			m_fFrame = 0.f;
	}

	if (m_bDead)
		m_eCurState = DP_DEAD;

	Face_Camera();

	CGameObject::Update_GameObject(m_fSlowDelta);

	if (!m_bDead)
	{
		m_eCurState = DP_IDLE;
		Follow_Player(fTimeDelta);
	}

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CDople::LateUpdate_GameObject()
{
	if (m_bHit)
	{
		m_bDead = true;
	}

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CDople::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CDople::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DopleTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DopleTexture", pComponent });

	// BACK
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DopleBackTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DopleBackTexture", pComponent });

	// LEFT
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DopleLeftTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DopleLeftTexture", pComponent });

	// RIGHT
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DopleRightTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DopleRightTexture", pComponent });

	// DEAD
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DopleDeadTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DopleDeadTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CDople::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CDople::DP_IDLE:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleTexture"));
			break;

		case CDople::DP_WALK:
			m_iPicNum = 5;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleTexture"));
			break;

		case CDople::DP_BACK:
			m_iPicNum = 5;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleBackTexture"));
			break;

		case CDople::DP_LEFT:
			m_iPicNum = 8;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleLeftTexture"));
			break;

		case CDople::DP_RIGHT:
			m_iPicNum = 8;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleRightTexture"));
			break;

		case CDople::DP_DEAD:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.7f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dople", L"Proto_DopleDeadTexture"));
			break;

		}
		m_ePreState = m_eCurState;
	}
}

void CDople::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;

}

void CDople::Follow_Player(const _float& fTimeDelta)
{
	CComponent* pComponent = CPlayer::GetInstance()->Get_Component_Player_Transform();
	m_pTargetTransCom = dynamic_cast<CTransform*>(pComponent);

	_vec3 vPlayerPos, vPlayerDir, vPlayerRight, vPos, vDir;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTargetTransCom->Get_Info(INFO_LOOK, &vPlayerDir);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTargetTransCom->Get_Info(INFO_RIGHT, &vPlayerRight);

	Change_State();

	if (DP_WALK == m_eCurState)
	{
		vDir = (vPlayerDir * -1.f);
		m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
	}
	else if (DP_BACK == m_eCurState)
	{
		vDir = vPlayerDir;
		//vDir = (vPlayerDir * -1.f); // Spike 충돌 테스트용
		m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
	}
	else if (DP_LEFT == m_eCurState)
	{
		vDir = -vPlayerRight;
		m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
	}
	else if (DP_RIGHT == m_eCurState)
	{
		vDir = vPlayerRight;
		m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
	}
}

void CDople::Change_State()
{
	int iPlayerState = CPlayer::GetInstance()->Get_PlayerCurState();

	switch (iPlayerState)
	{
	case 0:
		m_eCurState = DP_IDLE;
		break;
	case 1:
		m_eCurState = DP_BACK;
		break;
	case 2:
		m_eCurState = DP_WALK;
		break;
	case 3:
		m_eCurState = DP_LEFT;
		break;
	case 4:
		m_eCurState = DP_RIGHT;
		break;
	}
}

CDople* CDople::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDople* pInstance = new CDople(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Dople Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDople::Free()
{
	__super::Free();
}
