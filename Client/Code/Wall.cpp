#include "stdafx.h"
#include "..\Header\Wall.h"

#include "Export_Utility.h"

#include "CubeObject.h"

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CWall::CWall(const CWall& rhs)
	: Engine::CGameObject(rhs)
{

}

CWall::~CWall()
{
}

HRESULT CWall::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Engine::_int CWall::Update_GameObject(const _float& fTimeDelta)
{

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	for (auto& iter : m_vecCubes)
		iter->Update_GameObject(fTimeDelta);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CWall::LateUpdate_GameObject()
{
	for (auto& iter : m_vecCubes)
		iter->LateUpdate_GameObject();

	__super::LateUpdate_GameObject();
}

void CWall::Render_GameObject()
{	


}

// ť�� �ؽ��� �±׸� �������ָ�, �� �±׿� �´� ť�� ������Ʈ�� Ÿ�� ������ŭ ������Ű�� �������� �������ش�.
HRESULT CWall::Set_Cube_Texture_Tag(const _tchar* pCubeTextureTag, int iAxis)
{
	CCubeObject* pCube = nullptr;

	m_vecCubes.resize(MAX_Y * VTXCNTX);

	for (int i = 0; i < MAX_Y; ++i)
	{
		for (int j = 0; j < VTXCNTX; ++j)
		{
			int iIdx = i * VTXCNTX + j;

			pCube = CCubeObject::Create(m_pGraphicDev);
			
			NULL_CHECK_RETURN(pCube, E_FAIL);
			pCube->Set_Cute_Texture(pCubeTextureTag);
			CTransform* pTemp = dynamic_cast<CTransform*>(pCube->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

			//��� ���� �������� ���� ť���� ������ ������ ����
			switch (iAxis)
			{
			case WALL_LEFT:
				pCube->Set_Dst_Pos({ 0, pTemp->m_vScale.y * i * 2 + pTemp->m_vScale.y, (_float)(j * pTemp->m_vScale.z + (pTemp->m_vScale.z * 0.5)) });
				break;
			case WALL_RIGHT:
				pCube->Set_Dst_Pos({ pTemp->m_vScale.x * VTXCNTX, pTemp->m_vScale.y * i * 2 + pTemp->m_vScale.y, (_float)(j * pTemp->m_vScale.z + (pTemp->m_vScale.z * 0.5)) });
				break;
			case WALL_TOP:
				pCube->Set_Dst_Pos({ (_float)(j * pTemp->m_vScale.x + (pTemp->m_vScale.x * 0.5)), pTemp->m_vScale.y * i * 2 + pTemp->m_vScale.y, pTemp->m_vScale.z * VTXCNTX });
				break;
			case WALL_BOTTOM:
				pCube->Set_Dst_Pos({ (_float)(j * pTemp->m_vScale.x + (pTemp->m_vScale.x * 0.5)), pTemp->m_vScale.y * i * 2 + pTemp->m_vScale.y, 0 });
				break;
			}
				
			m_vecCubes[iIdx] = pCube;
		}

	}

	return S_OK;
}

bool CWall::Get_Arrived()
{
	//�ϳ��� �������� �ʾҴٸ� false�� return �Ѵ�
	for (auto& iter : m_vecCubes)
	{
		if (!iter->Get_Arrived())
			return false;
	}

	return true;
}

HRESULT CWall::Add_Component()
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

CWall * CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWall *	pInstance = new CWall(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SkyBox Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CWall::Free()
{	
	for_each(m_vecCubes.begin(), m_vecCubes.end(), CDeleteObj());
	__super::Free();
}


