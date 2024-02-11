#include "stdafx.h"
#include "CenterFly.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Player.h"

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
	//m_pTransformCom->Set_Pos(0.f, 10.f, 30.f); // 추후 필요 X

	m_fSpeed = 1.f;
	m_bDead = true;

	m_eMstType = ATTACK_FLY;

	return S_OK;
}

_int CCenterFly::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");
	
	/*if (m_bDead)
		return 1;*/

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	else
		m_fSlowDelta = 0.f;

	CGameObject::Update_GameObject(m_fSlowDelta);

	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));
	
	_vec3 vPlayerPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransformCom->Chase_Target(&vPlayerPos, 3.f, m_fSlowDelta);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Set_Pos(vPos.x, 2.f, vPos.z);

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
