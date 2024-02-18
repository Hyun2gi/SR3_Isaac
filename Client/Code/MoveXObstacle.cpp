#include "stdafx.h"
#include "..\Header\MoveXObstacle.h"

#include "Export_Utility.h"

CMoveXObstacle::CMoveXObstacle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_fDistanceLeft(0.f), m_fDistanceRight(0.f),
	m_bTurn(false)
{
}

CMoveXObstacle::CMoveXObstacle(const CMoveXObstacle& rhs)
	: CMapObj(rhs)
{

}

CMoveXObstacle::~CMoveXObstacle()
{
}

HRESULT CMoveXObstacle::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
	m_bTurn = false;

	m_fSpeed = 3.f;
	m_fDropSpeed = 60.f;

	m_vPos = m_pTransformCom->m_vInfo[INFO_POS];

	m_eObjType = OBSTACLE_X;

	return S_OK;
}

Engine::_int CMoveXObstacle::Update_GameObject(const _float& fTimeDelta)
{
	__super::Compute_ViewZ(&m_pTransformCom->m_vInfo[INFO_POS]);
	
	CGameObject::Update_GameObject(fTimeDelta);
	_vec3 vDir = _vec3(0.f, -1.f, 0.f);

	if (!m_bArrived && 1 < m_pTransformCom->m_vInfo[INFO_POS].y)
	{
		m_pTransformCom->Move_Pos(&vDir, m_fDropSpeed, fTimeDelta);
	}
	else if (!m_bArrived)
	{
		m_bArrived = true;
		m_pTransformCom->m_vInfo[INFO_POS].y = 1;

		_matrix	mat = *(m_pTransformCom->Get_WorldMatrix());

		mat._42 -= 1.f;

		_float fRandNumX = (rand() % 5 + 1) * 0.1f;
		_float fRandNumZ = (rand() % 5 + 1) * 0.1f;
		mat._41 += fRandNumX;
		mat._43 += fRandNumZ;

		CParticleMgr::GetInstance()->Create_Explosion(m_pGraphicDev, mat, 1.f, 30, 4.f);
	}

	if (m_bArrived)
	{
		if (0.5f < m_fDorpTimer)
		{
			Move(fTimeDelta);
			Check_Wall_Collision();
		}
		else
		{
			m_fDorpTimer += fTimeDelta;
		}
	}

	return 0;
}

void CMoveXObstacle::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	__super::LateUpdate_GameObject();
}

void CMoveXObstacle::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMoveXObstacle::Set_Cute_Texture(const _tchar* pTextureTag)
{
	return E_NOTIMPL;
}

void CMoveXObstacle::Set_Pos(_vec3 vPos)
{
	m_vPos = vPos;
}

HRESULT CMoveXObstacle::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
	m_pTransformCom->m_vInfo[INFO_POS] = { 10, 50, 10 };

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ObstacleTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ObstacleTexture", pComponent });

	return S_OK;
}

void CMoveXObstacle::Move(const _float& fTimeDelta)
{
	_float fLeftDst = m_vPos.x - m_fDistanceLeft;
	_float fRightDst = m_vPos.x + m_fDistanceRight;

	//°ÅÁþÀÌ¸é ¿ÞÂÊÀ¸·Î ÀÌµ¿
	if (!m_bTurn)
	{
		m_pTransformCom->Set_Pos_X(-m_fSpeed * fTimeDelta);

		if (m_pTransformCom->m_vInfo[INFO_POS].x <= fLeftDst)
			m_bTurn = true;
	}
	else
	{
		m_pTransformCom->Set_Pos_X(m_fSpeed * fTimeDelta);

		if (m_pTransformCom->m_vInfo[INFO_POS].x >= fRightDst)
			m_bTurn = false;
	}
}

void CMoveXObstacle::Check_Wall_Collision()
{
	_vec3 vPos, vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Scale(&vScale);

	if (vPos.x - vScale.x * 2.f <= 0 && !m_bTurn)
		m_bTurn = true;

	if (vPos.x + vScale.x * 2.f >= VTXCNTX && m_bTurn)
		m_bTurn = false;
}

CMoveXObstacle * CMoveXObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMoveXObstacle *	pInstance = new CMoveXObstacle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MoveXObstacle Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMoveXObstacle::Free()
{
	__super::Free();
}


