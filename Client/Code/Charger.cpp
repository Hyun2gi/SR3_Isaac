#include "stdafx.h"
#include "Charger.h"

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

	m_fCallLimit = 0.f;
	m_fSpeed = 2.f;

	m_eState = CHARGER_END;

	return S_OK;
}

_int CCharger::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 4.f * fTimeDelta * 1.5;

	if (4.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	Check_Range();

	if (CHARGER_IDLE == m_eState)
		m_fSpeed = 2.f;
	else if (CHARGER_ATTACK == m_eState)
		m_fSpeed = 5.f;

	_vec3 vTargetPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vTargetPos);

	m_pTransformCom->Chase_Target(&vTargetPos, m_fSpeed, fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CCharger::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CCharger::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CCharger::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ChargerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ChargerTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CCharger::Check_Range()
{
	_vec3 vTargetPos, vPos;

	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));
	m_pTargetTransCom->Get_Info(INFO_POS, &vTargetPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fDistance =
		(fabs(vTargetPos.x - vPos.x) * fabs(vTargetPos.x - vPos.x)) +
		(fabs(vTargetPos.y - vPos.y) * fabs(vTargetPos.y - vPos.y));

	if (100 < fDistance) // 범위에 속하지 않을 때
	{
		m_eState = CHARGER_IDLE;
	}
	else // 범위에 속할 때
	{
		m_eState = CHARGER_ATTACK;
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
