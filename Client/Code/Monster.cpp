#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),
	m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTargetTransCom(nullptr), m_pTextureCom(nullptr), m_pCalculCom(nullptr),
	m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f), m_fSecAccTimeDelta(0.f), m_fSlowDelta(0.f),
	m_bDead(false), m_bHit(false)
{
	m_vOriginScale = { 1.f, 1.f, 1.f };
}

CMonster::CMonster(const CMonster& rhs)
	: Engine::CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom), m_pTransformCom(rhs.m_pTransformCom), m_pTargetTransCom(rhs.m_pTargetTransCom),
	m_pTextureCom(rhs.m_pTextureCom), m_pCalculCom(rhs.m_pCalculCom),
	m_fCallLimit(rhs.m_fCallLimit), m_fAccTimeDelta(rhs.m_fAccTimeDelta), m_fSpeed(rhs.m_fSpeed), m_fSecAccTimeDelta(rhs.m_fSecAccTimeDelta), m_fSlowDelta(rhs.m_fSlowDelta),
	m_bDead(rhs.m_bDead), m_bHit(rhs.m_bHit)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	return S_OK;
}

Engine::_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CMonster::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{	
}

HRESULT CMonster::Add_Component()
{
	return S_OK;
}

bool CMonster::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

bool CMonster::Check_Time(const _float& fTimeDelta, float fLimit)
{
	m_fSecAccTimeDelta += fTimeDelta;

	if (m_fSecAccTimeDelta >= fLimit)
	{
		m_fSecAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

void CMonster::Check_Map_Range()
{
	// 맵 밖으로 나갈 시 예외 처리
	// 해당 객체의 Pos 값을 받아와 Terrain 과 비교 후 Terrain 범위를 벗어나면 true 반환

}

void CMonster::Hit_PushBack(const _float& fTimeDelta)
{
	_vec3 vDir;
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));
	m_pTargetTransCom->Get_Info(INFO_LOOK, &vDir);
	m_pTransformCom->Move_Pos(&vDir, 20.f, fTimeDelta);
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster *	pInstance = new CMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Monster Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMonster::Free()
{	
	__super::Free();
}

