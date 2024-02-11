#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),
	m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTargetTransCom(nullptr), m_pTextureCom(nullptr), m_pCalculCom(nullptr),
	m_fCallLimit(0.f), m_fAccTimeDelta(0.f), m_fSpeed(0.f), m_fSecAccTimeDelta(0.f), m_fSlowDelta(0.f), m_fColorTimeDelta(0.f),
	m_bDead(false), m_bHit(false), m_bBoss(false), m_eMstType(MONSTER_TYPE_END), m_bScaleReduce(true), m_bHitColor(false), m_eBossType(NONE_BOSS), m_bTimeScale(false), m_bCreate(false)
{
	m_vOriginScale = { 1.f, 1.f, 1.f };
}

CMonster::CMonster(const CMonster& rhs)
	: Engine::CGameObject(rhs),
	m_pBufferCom(rhs.m_pBufferCom), m_pTransformCom(rhs.m_pTransformCom), m_pTargetTransCom(rhs.m_pTargetTransCom),
	m_pTextureCom(rhs.m_pTextureCom), m_pCalculCom(rhs.m_pCalculCom), m_fColorTimeDelta(rhs.m_fColorTimeDelta),
	m_fCallLimit(rhs.m_fCallLimit), m_fAccTimeDelta(rhs.m_fAccTimeDelta), m_fSpeed(rhs.m_fSpeed), m_fSecAccTimeDelta(rhs.m_fSecAccTimeDelta), m_fSlowDelta(rhs.m_fSlowDelta), m_bCreate(rhs.m_bCreate),
	m_bDead(rhs.m_bDead), m_bHit(rhs.m_bHit), m_bBoss(rhs.m_bBoss), m_eMstType(rhs.m_eMstType), m_bScaleReduce(rhs.m_bScaleReduce), m_bHitColor(rhs.m_bHitColor), m_eBossType(rhs.m_eBossType), m_bTimeScale(rhs.m_bTimeScale)
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

void CMonster::Change_Scale() // 호출방법 고민
{
	if (m_bScaleReduce)
	{
		if (m_pTransformCom->m_vScale.x <= m_vOriginScale.x - 1.f)
		{
			m_bScaleReduce = false;
		}

		m_pTransformCom->m_vScale.x -= 0.1f;
	}
	else
	{
		if (m_pTransformCom->m_vScale.x >= m_vOriginScale.x)
		{
			m_bScaleReduce = false;
		}

		m_pTransformCom->m_vScale.x += 0.1f;
	}
}

void CMonster::Change_Color(const _float& fTimeDelta)
{
	m_fColorTimeDelta += fTimeDelta;

	if (m_fColorTimeDelta >= 0.2f)
	{
		D3DXCOLOR temp = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		m_pBufferCom->Set_Color(temp);

		m_fColorTimeDelta = 0.f;

		m_bHitColor = false;

		return;
	}
	else // 타이머가 돌 동안
	{
		D3DXCOLOR temp = D3DXCOLOR(1.f, 0.0f, 0.0f, 1.f);
		m_pBufferCom->Set_Color(temp);
	}
}

void CMonster::Check_Outof_Map()
{
	// 맵 밖에 생성된 경우 맵 안에 랜덤한 값으로 옮겨주기

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (0 > vPos.x || VTXCNTX < vPos.x || 0 > vPos.z || VTXCNTZ < vPos.z)
	{
		m_pTransformCom->Set_Pos((rand() % VTXCNTX - 10) + 5, vPos.y, (rand() % VTXCNTZ - 10) + 5);
	}
}

void CMonster::Create_Start_Particle(_float fCallLimit)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.7f, 5);
	m_fCallLimit = fCallLimit;
	m_bCreate = true;
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

bool CMonster::Check_Color_Time(const _float& fTimeDelta)
{
	return false;
}

void CMonster::Hit_PushBack(const _float& fTimeDelta)
{
	_vec3 vDir;
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));
	m_pTargetTransCom->Get_Info(INFO_LOOK, &vDir);
	m_pTransformCom->Move_Pos(&vDir, 20.f, fTimeDelta);
}

void CMonster::Fix_Y(_float fY)
{
	// y 값 고정
	_vec3 vNewPos;
	m_pTransformCom->Get_Info(INFO_POS, &vNewPos);
	m_pTransformCom->Set_Pos(vNewPos.x, fY, vNewPos.z);
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

