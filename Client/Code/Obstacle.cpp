#include "stdafx.h"
#include "..\Header\Obstacle.h"

#include "Export_Utility.h"

CObstacle::CObstacle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CObstacle::CObstacle(const CObstacle& rhs)
	: CMapObj(rhs)
{

}

CObstacle::~CObstacle()
{
}

HRESULT CObstacle::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };

	m_eObjType = OBSTACLE;

	return S_OK;
}

Engine::_int CObstacle::Update_GameObject(const _float& fTimeDelta)
{
	__super::Compute_ViewZ(&m_pTransformCom->m_vInfo[INFO_POS]);

	CGameObject::Update_GameObject(fTimeDelta);


	_vec3 vDir = _vec3(0.f, -1.f, 0.f);

	if (!m_bArrived && 1 < m_pTransformCom->m_vInfo[INFO_POS].y)
	{
		m_pTransformCom->Move_Pos(&vDir, m_fDropSpeed, fTimeDelta);
	}
	else if(!m_bArrived)
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

	return 0;
}

void CObstacle::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	__super::LateUpdate_GameObject();
}

void CObstacle::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CObstacle::Set_Cute_Texture(const _tchar* pTextureTag)
{
	return E_NOTIMPL;
}

void CObstacle::Set_Pos(_vec3 vPos)
{
	//m_fDestY = vPos.y;
	//m_pTransformCom->Set_Pos(vPos);
	//m_pTransformCom->m_vInfo[INFO_POS].y += 100.f;
}

HRESULT CObstacle::Add_Component()
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

CObstacle * CObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CObstacle *	pInstance = new CObstacle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Obstacle Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CObstacle::Free()
{
	__super::Free();
}


