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
	m_pTransformCom->m_vScale = { 1.5f, 1.5f, 1.5f };
	m_pTransformCom->Set_Pos(20.f, 2.f, 25.f);

	m_fCallLimit = 1.f;
	m_fSpeed = 10.f;

	m_ePreState = DP_END;

	return S_OK;
}

_int CDople::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (DP_DEAD == m_eCurState)
	{
		m_fFrame = 0.f;
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

	/*CComponent* pComponent = CPlayer::GetInstance()->Get_Component_Player_Transform();
	m_pTargetTransCom = dynamic_cast<CTransform*>(pComponent);
	_vec3 vPlayerPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransformCom->Set_Pos(vPlayerPos);*/

	// Player 의 Look 벡터 방향에 따라 상태와 움직임을 바꾸는?

	if (!m_bDead)
	{
		m_eCurState = DP_IDLE;
		Follow_Player();
	}

	if (m_bHit)
	{
		m_bDead = true;
	}

	

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CDople::LateUpdate_GameObject()
{
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
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
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

void CDople::Follow_Player()
{
	CComponent* pComponent = CPlayer::GetInstance()->Get_Component_Player_Transform();
	m_pTargetTransCom = dynamic_cast<CTransform*>(pComponent);

	// LOOK 받으면 X 플레이어 상태를 받아서 조건문 처리하는 게 좋을 듯함
	_vec3 vPlayerPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);

	// 좌, 우는 똑같이. 상, 하는 반대로

	m_pTransformCom->Set_Pos(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z + 5.f);
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
