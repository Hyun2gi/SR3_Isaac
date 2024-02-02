#include "stdafx.h"
#include "Squirt.h"

#include "Export_Utility.h"

CSquirt::CSquirt(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CSquirt::CSquirt(const CSquirt& rhs)
	: CMonster(rhs)
{
}

CSquirt::~CSquirt()
{
}

HRESULT CSquirt::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));

	m_iHp = 6;

	m_fCallLimit = 3;
	m_fSpeed = 2.f;

	m_bSliding = false;
	m_fAccel = 10.f;

	m_ePreState = SQU_END;

	return S_OK;
}

_int CSquirt::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	Face_Camera();

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(fTimeDelta);

		m_bHit = false;

		if (0 >= m_iHp)
		{
			m_bDead = true;
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta) && !m_bSliding)
	{
		Check_TargetPos();
		m_bSliding = true;
	}

	if (m_bSliding)
	{
		Sliding(fTimeDelta);
		m_eCurState = SQU_SLIDE;
	}
	else
		m_eCurState = SQU_IDLE;

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSquirt::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CSquirt::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CSquirt::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SquirtTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SquirtTexture", pComponent });

	// SLIDE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SquirtSlideTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SquirtSlideTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CSquirt::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CSquirt::SQU_IDLE:
			m_iPicNum = 2;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Squirt", L"Proto_SquirtTexture"));
			break;

		case CSquirt::SQU_SLIDE:
			m_iPicNum = 3;
			m_fFrameSpeed = 0.3f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Squirt", L"Proto_SquirtSlideTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CSquirt::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CSquirt::Sliding(const _float& fTimeDelta)
{
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	m_pTransformCom->Move_Pos(&m_vDir, m_fSpeed * m_fAccel, fTimeDelta);

	m_fAccel -= 0.1;

	if (m_fAccel <= 0.f)
	{
		m_bSliding = false;
		m_fAccel = 10.f;
	}
}

void CSquirt::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);

	m_vDir = m_vTargetPos - m_pTransformCom->m_vInfo[INFO_POS];
}

CSquirt* CSquirt::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSquirt* pInstance = new CSquirt(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Squirt Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSquirt::Free()
{
	__super::Free();
}
