#include "stdafx.h"
#include "CenterFly.h"

#include "Export_Utility.h"

CCenterFly::CCenterFly(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev),
	m_pTargetTransCom(nullptr)
{
}

CCenterFly::CCenterFly(const CCenterFly& rhs)
	: CMonster(rhs),
	m_pTargetTransCom(rhs.m_pTargetTransCom)
{
}

CCenterFly::~CCenterFly()
{
}

HRESULT CCenterFly::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, 10.f, 30.f); // ���� �ʿ� X

	m_fSpeed = 1.f;

	return S_OK;
}

_int CCenterFly::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"));
	_vec3 vPlayerPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransformCom->Chase_Target(&vPlayerPos, 3.f, fTimeDelta);

	return 0;
}

void CCenterFly::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CCenterFly::Render_GameObject()
{
}

HRESULT CCenterFly::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

CCenterFly* CCenterFly::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCenterFly* pInstance = new CCenterFly(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Center Fly Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCenterFly::Free()
{
	__super::Free();
}