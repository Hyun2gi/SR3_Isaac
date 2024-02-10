#include "stdafx.h"
#include "..\Header\MoveZObstacle.h"

#include "Export_Utility.h"

CMoveZObstacle::CMoveZObstacle(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev),
	m_fDistanceUp(0.f), m_fDistanceDown(0.f),
	m_bTurn(false)
{
}

CMoveZObstacle::CMoveZObstacle(const CMoveZObstacle& rhs)
	: Engine::CGameObject(rhs)
{

}

CMoveZObstacle::~CMoveZObstacle()
{
}

HRESULT CMoveZObstacle::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
	m_bTurn = false;

	m_fSpeed = 3.f;

	m_vPos = m_pTransformCom->m_vInfo[INFO_POS];

	return S_OK;
}

Engine::_int CMoveZObstacle::Update_GameObject(const _float& fTimeDelta)
{
	__super::Compute_ViewZ(&m_pTransformCom->m_vInfo[INFO_POS]);
	
	CGameObject::Update_GameObject(fTimeDelta);

	Move(fTimeDelta);
	Check_Wall_Collision();

	return 0;
}

void CMoveZObstacle::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	__super::LateUpdate_GameObject();
}

void CMoveZObstacle::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMoveZObstacle::Set_Cute_Texture(const _tchar* pTextureTag)
{
	return E_NOTIMPL;
}

void CMoveZObstacle::Set_Pos(_vec3 vPos)
{
	m_vPos = vPos;
}

HRESULT CMoveZObstacle::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
	m_pTransformCom->m_vInfo[INFO_POS] = { 10, 1, 10 };

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ObstacleTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ObstacleTexture", pComponent });

	return S_OK;
}

void CMoveZObstacle::Move(const _float& fTimeDelta)
{
	_float fUpDst = m_vPos.x - m_fDistanceUp;
	_float fDownDst = m_vPos.x + m_fDistanceDown;

	//°ÅÁþÀÌ¸é ¿ÞÂÊÀ¸·Î ÀÌµ¿
	if (!m_bTurn)
	{
		m_pTransformCom->Set_Pos_Z(-m_fSpeed * fTimeDelta);

		if (m_pTransformCom->m_vInfo[INFO_POS].z <= fUpDst)
			m_bTurn = true;
	}
	else
	{
		m_pTransformCom->Set_Pos_Z(m_fSpeed * fTimeDelta);

		if (m_pTransformCom->m_vInfo[INFO_POS].z >= fDownDst)
			m_bTurn = false;
	}
}

void CMoveZObstacle::Check_Wall_Collision()
{
	_vec3 vPos, vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Scale(&vScale);

	if (vPos.z - vScale.z * 2.f <= 0 && !m_bTurn)
		m_bTurn = true;

	if (vPos.z + vScale.z * 2.f >= VTXCNTZ && m_bTurn)
		m_bTurn = false;

}

CMoveZObstacle * CMoveZObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMoveZObstacle *	pInstance = new CMoveZObstacle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Obstacle Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMoveZObstacle::Free()
{
	__super::Free();
}


