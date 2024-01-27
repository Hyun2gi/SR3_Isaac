#include "stdafx.h"
#include "Monstro.h"

#include "Export_Utility.h"

CMonstro::CMonstro(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CMonstro::CMonstro(const CMonstro& rhs)
	: CMonster(rhs)
{
}

CMonstro::~CMonstro()
{
}

HRESULT CMonstro::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 3.2f, 10.f);
	m_pTransformCom->m_vScale = { 5.f, 5.f, 0.f };

	m_iHp = 5;

	m_fCallLimit = 7.f;
	m_fSpeed = 10.f;

	m_fPower = 2.f;
	m_fAccelTime = 0.f;

	m_bUp = false;

	m_eState = MONSTRO_IDLE;

	return S_OK;
}

_int CMonstro::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 3.f * fTimeDelta * 0.4f;

	if (3.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	// 기믹1 - 작은 점프로 플레이어 추적
	// 가만히 있다가 일정 시간 후 Move로 State 변경
	// Move State 에서 일정 시간마다 Move true 후 플레이어 위치 체크
	// 해당 Pos로 작은 점프
	// 플레이어와 일정 거리가 되었을 때 기믹3 - 플레이어를 향해 총알 발사
	// 일정 시간마다 기믹2 - 큰 점프 발동

	if (MONSTRO_IDLE == m_eState) // 기본 상태일 때
	{
		if (Check_Time(fTimeDelta))// && !m_bMove) // 일정 시간마다 기믹3 - 큰 점프 발동
		{
			m_bUp = true;
			m_eState = MONSTRO_JUMP;
			Check_TargetPos();

		}
		else
		{
			if (Check_Time(fTimeDelta, 1.f)) // 일정 시간마다 기믹 1 - 작은 점프 발동
			{
				m_eState = MONSTRO_MOVE;
				Check_TargetPos();
			}
		}
	}
	else if (MONSTRO_MOVE == m_eState)
	{
		MoveTo_Player(fTimeDelta);
	}
	else if (MONSTRO_JUMP == m_eState)
	{
		JumpTo_Player(fTimeDelta);
	}

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CMonstro::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMonstro::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonstro::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMonstro::MoveTo_Player(const _float& fTimeDelta)
{
	// 한번씩 점프를 하며 플레이어를 향해 추적
	// Move 모드일 때 일정 시간마다 현재 플레이어의 위치를 체크 후 그 곳으로 작은 점프
	
	// 플레이어 위치를 향한 방향 벡터 구하기
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fY = vPos.y + (m_fPower * m_fAccelTime) - (9.f * m_fAccelTime * m_fAccelTime * 0.5f);
	m_fAccelTime += 0.02f;

	if (fY < 3.0f)
	{
		m_fAccelTime = 0.f;
		fY = 3.2f;
		m_eState = MONSTRO_IDLE;
	}

	m_pTransformCom->Set_Pos(vPos.x, fY, vPos.z);

	vDir = m_vTargetPos - vPos;

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);

}

void CMonstro::JumpTo_Player(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (m_bUp)
	{
		if (vPos.y >= 10.f)
		{
			m_bUp = false;
			vPos = m_vTargetPos;
			vPos.y = 80.f;
		}
		else
		{
			vPos.y += 0.9f;
		}
	}
	else
	{
		if (vPos.y > 3.2f)
			vPos.y -= 0.7f;
		else
		{
			vPos.y = 3.2f;
			m_eState = MONSTRO_IDLE;
		}
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CMonstro::AttackTo_Player()
{
}

void CMonstro::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);

}

CMonster* CMonstro::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonstro* pInstance = new CMonstro(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Monstro Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMonstro::Free()
{
	__super::Free();
}
