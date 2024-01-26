#include "stdafx.h"
#include "AttackFly.h"

#include "Export_Utility.h"

CAttackFly::CAttackFly(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CMonster(pGraphicDev),
	m_pCenterTransCom(nullptr), m_pTargetTransCom(nullptr),
	m_iID(ATTACKFLY)
{
	m_iIndex = iIndex;
}

CAttackFly::CAttackFly(const CAttackFly& rhs)
	: CMonster(rhs),
	m_pCenterTransCom(nullptr), m_pTargetTransCom(nullptr),
	m_iID(ATTACKFLY)
{
}

CAttackFly::~CAttackFly()
{
}

HRESULT CAttackFly::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);

	m_iHp = 5;

	m_fCallLimit = 0.f;
	m_fSpeed = 1.f;

	m_fDistance = 4.f;
	m_fAngle = 30.f * m_iIndex;

	return S_OK;
}

_int CAttackFly::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 2.f * fTimeDelta * 1.5;

	if (2.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (CENTEROBJ == m_iID)
	{
		m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));
		_vec3 vPlayerPos;
		m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);
		m_pTransformCom->Chase_Target(&vPlayerPos, 3.f, fTimeDelta);
	}
	else if (ATTACKFLY == m_iID)
		Revolve_Center();

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CAttackFly::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CAttackFly::Render_GameObject()
{
	if (CENTEROBJ != m_iID)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pTextureCom->Set_Texture((_uint)m_fFrame);

		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

void CAttackFly::Revolve_Center()
{
	m_pCenterTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"CenterFly", L"Proto_Transform"));

	_vec3 vPos;
	m_pCenterTransCom->Get_Info(INFO_POS, &vPos);

	m_pTransformCom->Set_Pos(vPos.x + m_fDistance * cos(m_fAngle * (3.14 / 180.f)),
		vPos.y,
		vPos.z - m_fDistance * sin(m_fAngle * (3.14 / 180.f)));

	m_fAngle += 1.f;
}

HRESULT CAttackFly::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AttackFlyTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_AttackFlyTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

CAttackFly* CAttackFly::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
	CAttackFly* pInstance = new CAttackFly(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Attack Fly Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CAttackFly::Free()
{
	__super::Free();
}