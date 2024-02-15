#include "stdafx.h"
#include "AttackFly.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Scene.h"
#include "Stage.h"

CAttackFly::CAttackFly(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev),
	m_CenterFly(nullptr)
{
}

CAttackFly::CAttackFly(const CAttackFly& rhs)
	: CMonster(rhs),
	m_CenterFly(rhs.m_CenterFly)
{
}

CAttackFly::~CAttackFly()
{
}

void CAttackFly::Set_NormalFly_ToStage(CLayer* pLayer)
{
	for (vector<CNormalFly*>::iterator iter = m_NormalFlyList.begin();
		iter != m_NormalFlyList.end();)
	{
		pLayer->Add_GameObject(L"NormalFly", *iter);
		++iter;
		m_iNormalCnt++;
	}
}

HRESULT CAttackFly::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bCreate = false;
	m_iNormalCnt = 10;
	m_eMstType = ATTACK_FLY;

	return S_OK;
}

_int CAttackFly::Update_GameObject(const _float& fTimeDelta)
{
	_float fSecondDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	else
		m_fSlowDelta = 0.f;

	CGameObject::Update_GameObject(fSecondDelta);

	

	if (!m_bCreate)
	{
		Create_AttackFly();
		m_bCreate = true;
	}
	else
	{
		if (Check_Fly_Dead())
		{
			m_CenterFly->Set_Dead();
			m_bDead = true;
			
		}
	}

	if (m_CenterFly != nullptr)
		m_CenterFly->Update_GameObject(fSecondDelta);

	if (!m_NormalFlyList.empty())
	{
		for (auto& iter : m_NormalFlyList)
		{
			iter->Update_GameObject(fSecondDelta);
		}
	}

	if (Check_Fly_Dead())
	{
		//true
		Engine::StopSound(SOUND_EFFECT_MON_STOPSUDDEN_SUB);
	}

	//if (m_bDead)
		//return 1;

	return 0;
}

void CAttackFly::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_CenterFly != nullptr)
		m_CenterFly->LateUpdate_GameObject();

	if (!m_NormalFlyList.empty())
	{
		for (auto& iter : m_NormalFlyList)
		{
			iter->LateUpdate_GameObject();
		}
	}
}

void CAttackFly::Render_GameObject()
{
	for (auto& iter : m_NormalFlyList)
	{
		iter->Render_GameObject();
	}
}

HRESULT CAttackFly::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AttackFlyTexture")); //Engine::Clone_Proto(L"Proto_AttackFlyTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_AttackFlyTexture", pComponent });

	// DEAD
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FlyDeadTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FlyDeadTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CAttackFly::Create_AttackFly()
{
	m_CenterFly = CCenterFly::Create(m_pGraphicDev);
	m_CenterFly->Get_Transform()->m_vInfo[INFO_POS] = m_pTransformCom->m_vInfo[INFO_POS];
	m_CenterFly->Set_MyLayer(m_vecMyLayer[0]);

	if (m_CenterFly == nullptr)
		return;

	for (int i = 1; i < 13; ++i)
	{
		CNormalFly* pNormalFly = CNormalFly::Create(m_pGraphicDev, i, m_vecMyLayer[0]);
		//pNormalFly->Set_MyLayer(m_vecMyLayer[0]); // Create 바꿀 필요 없을 듯함
		pNormalFly->Set_TargetTransform(m_CenterFly->Get_Transform());
		m_NormalFlyList.push_back(pNormalFly);
	}
}

_bool CAttackFly::Check_Fly_Dead()
{
	for (auto& iter : m_NormalFlyList)
	{
		if (!(iter->Get_Dead())) // 하나라도 살아있으면 false
			return false;
	}
	return true;
}

CAttackFly* CAttackFly::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAttackFly* pInstance = new CAttackFly(pGraphicDev);

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

	Safe_Release<CCenterFly*>(m_CenterFly);
	m_CenterFly = nullptr;

	for_each(m_NormalFlyList.begin(), m_NormalFlyList.end(), CDeleteObj());
}
