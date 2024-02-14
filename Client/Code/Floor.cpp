#include "stdafx.h"
#include "..\Header\Floor.h"

#include "Export_Utility.h"

#include "CubeObject.h"

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), 
	m_bStartScene(false), 
	m_iCubeActionType(0), m_iCubeCount(0),
	m_fCubeCreateTimer(0.f)
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

	m_vecCubes.reserve((VTXCNTZ * VTXCNTX) / 4);

	return S_OK;
}

Engine::_int CFloor::Update_GameObject(const _float& fTimeDelta)
{
	m_fCubeCreateTimer += fTimeDelta;

	if (!m_bCreated)
	{
		if (m_iCubeCount < VTXCNTZ && m_bGood)
		{
			if (m_fCubeCreateTimer > 0.05f)
			{
				m_fCubeCreateTimer = 0.f;
				Create_Cubes(m_bGood);
			}
		}
		else
		{
			m_bGood = false;
			if (m_fCubeCreateTimer > 0.05f)
			{
				m_fCubeCreateTimer = 0.f;
				Create_Cubes(m_bGood);
			}
		}
	}
	

	for (auto& iter : m_vecCubes)
		iter->Update_GameObject(fTimeDelta);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CFloor::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

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
	m_wstrTexture = pCubeTextureTag;

	return S_OK;
}

HRESULT CFloor::Create_Cubes(bool bBool)
{
	CCubeObject* pCube = nullptr;

	if (m_bStartScene)
	{
		for (int i = 0; i < VTXCNTZ; ++i)
		{
			for (int j = 0; j < VTXCNTX; ++j)
			{
				int iIdx = i * VTXCNTZ + j;

				pCube = CCubeObject::Create(m_pGraphicDev, m_bStartScene);
				NULL_CHECK_RETURN(pCube, E_FAIL);
				pCube->Set_Cute_Texture(m_wstrTexture.c_str());
				CTransform* pTemp = dynamic_cast<CTransform*>(pCube->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

				_vec3 vScale(pTemp->m_vScale.x * 2, pTemp->m_vScale.y * 2, pTemp->m_vScale.z * 2);

				pCube->Set_Dst_Pos({ (_float)(j * vScale.x), -vScale.y * 0.5f, (_float)(i * vScale.z) });
				pCube->Set_Cube_Action_Type(m_iCubeActionType);
				m_vecCubes.push_back(pCube);
			}

		}

		m_bCreated = true;
	}
	else
	{

		if (bBool)
		{
			for (int i = 0; i < m_iCubeCount + 1; i++)
			{
				_int iIndex = (m_iCubeCount - i) + (VTXCNTZ * i);

				_float z = iIndex / VTXCNTZ;
				_float x = iIndex - (z * VTXCNTZ);

				pCube = CCubeObject::Create(m_pGraphicDev, m_bStartScene);
				NULL_CHECK_RETURN(pCube, E_FAIL);
				pCube->Set_Cute_Texture(m_wstrTexture.c_str());
				CTransform* pTemp = dynamic_cast<CTransform*>(pCube->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

				_vec3 vScale(pTemp->m_vScale.x * 2, pTemp->m_vScale.y * 2, pTemp->m_vScale.z * 2);

				pCube->Set_Dst_Pos({ (_float)(x * vScale.x), -vScale.y * 0.5f, (_float)(z * vScale.z) });
				pCube->Set_Cube_Action_Type(m_iCubeActionType);
				m_vecCubes.push_back(pCube);
			}

			m_iCubeCount++;
		}
		else
		{
			for (int i = 0; i < m_iCubeCount; i++)
			{
				_int iIndex = VTXCNTZ * (VTXCNTZ - m_iCubeCount) + VTXCNTZ * i + VTXCNTZ - i;

				_float z = iIndex / VTXCNTZ;
				_float x = iIndex - (z * VTXCNTZ);

				pCube = CCubeObject::Create(m_pGraphicDev, m_bStartScene);
				NULL_CHECK_RETURN(pCube, E_FAIL);
				pCube->Set_Cute_Texture(m_wstrTexture.c_str());
				CTransform* pTemp = dynamic_cast<CTransform*>(pCube->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

				_vec3 vScale(pTemp->m_vScale.x * 2, pTemp->m_vScale.y * 2, pTemp->m_vScale.z * 2);

				pCube->Set_Dst_Pos({ (_float)(x * vScale.x), -vScale.y * 0.5f, (_float)(z * vScale.z) });
				pCube->Set_Cube_Action_Type(m_iCubeActionType);
				m_vecCubes.push_back(pCube);
			}

			m_iCubeCount--;
		}
	}
}

bool CFloor::Get_Arrived()
{
	// 사이즈가 1미만이라면 한개도 생성되지 않은 시점이기 때문에 검사를 하지않고 거짓을 반환
	if (1 > m_vecCubes.size()) return false;

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
	CComponent*		pComponent = nullptr;
	//	
	//pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	m_pTransformCom->m_vInfo[INFO_POS] = { VTXCNTX * 0.5f, 0.f, VTXCNTX * 0.5f };
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
		
	return S_OK;
}

CFloor * CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	_int iCubeActionType,
	bool bStartScene)
{
	CFloor *	pInstance = new CFloor(pGraphicDev);
	pInstance->m_bStartScene = bStartScene;
	pInstance->m_iCubeActionType = iCubeActionType;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Floor Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFloor::Free()
{	
	for_each(m_vecCubes.begin(), m_vecCubes.end(), CDeleteObj());
	__super::Free();
}


