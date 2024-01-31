#include "stdafx.h"
#include "..\Header\Floor.h"

#include "Export_Utility.h"

#include "CubeObject.h"

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CFloor::CFloor(const CFloor& rhs)
	: Engine::CGameObject(rhs)
{

}

CFloor::~CFloor()
{
}

HRESULT CFloor::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Engine::_int CFloor::Update_GameObject(const _float& fTimeDelta)
{

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	for (auto& iter : m_vecCubes)
		iter->Update_GameObject(fTimeDelta);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CFloor::LateUpdate_GameObject()
{
	for (auto& iter : m_vecCubes)
		iter->LateUpdate_GameObject();

	__super::LateUpdate_GameObject();
}

void CFloor::Render_GameObject()
{	
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev ->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pTextureCom->Set_Texture(0);

	//m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

// 큐브 텍스쳐 태그를 지정해주면, 그 태그에 맞는 큐브 오브젝트를 타일 개수만큼 생성시키고 포지션을 지정해준다.
HRESULT CFloor::Set_Cube_Texture_Tag(const _tchar* pCubeTextureTag)
{
	CCubeObject* pCube = nullptr;

	m_vecCubes.resize(VTXCNTZ * VTXCNTX);

	for (int i = 0; i < VTXCNTZ; ++i)
	{
		for (int j = 0; j < VTXCNTX; ++j)
		{
			int iIdx = i * VTXCNTZ + j;

			pCube = CCubeObject::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pCube, E_FAIL);
			pCube->Set_Cute_Texture(pCubeTextureTag);
			CTransform* pTemp = dynamic_cast<CTransform*>(pCube->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
			pCube->Set_Dst_Pos({ (_float)(j * pTemp->m_vScale.x + (pTemp->m_vScale.x * 0.5)), -	pTemp->m_vScale.y, (_float)(i * pTemp->m_vScale.z + (pTemp->m_vScale.z * 0.5)) });
			m_vecCubes[iIdx] = pCube;
		}

	}

	return S_OK;
}

bool CFloor::Get_Arrived()
{
	//하나라도 도착하지 않았다면 false를 return 한다
	for (auto& iter : m_vecCubes)
	{
		if (!iter->Get_Arrived())
			return false;
	}

	return true;

}

HRESULT CFloor::Add_Component()
{
	//CComponent*		pComponent = nullptr;
	//	
	//pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	//pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
	//	
	return S_OK;
}

CFloor * CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFloor *	pInstance = new CFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SkyBox Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFloor::Free()
{	
	for_each(m_vecCubes.begin(), m_vecCubes.end(), CDeleteObj());
	__super::Free();
}


