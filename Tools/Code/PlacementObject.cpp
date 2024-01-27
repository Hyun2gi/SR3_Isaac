#include "../Include/stdafx.h"
#include "..\Header\PlacementObject.h"

#include "Export_Utility.h"

CPlacementObject::CPlacementObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CPlacementObject::CPlacementObject(const CPlacementObject& rhs)
	: Engine::CGameObject(rhs)
{

}

CPlacementObject::~CPlacementObject()
{
}

HRESULT CPlacementObject::Ready_GameObject(wstring wstrName, int iType, int iIndex)
{
	m_wstrTextureName = wstrName;
	m_wstrProtoName = L"Proto_" + wstrName;
	m_iObjType = iType;
	m_iIndex = iIndex;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Add_TextureComponent(), E_FAIL);

	return S_OK;
}

Engine::_int CPlacementObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	CGameObject::Update_GameObject(fTimeDelta);
	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	return 0;
}

void CPlacementObject::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
	
	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPlacementObject::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CPlacementObject::Add_Component()
{
	CComponent*		pComponent = nullptr;
			
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Calculator", pComponent });
		
	return S_OK;
}

HRESULT CPlacementObject::Add_TextureComponent()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(m_wstrProtoName.c_str()));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ m_wstrProtoName.c_str(), pComponent});

	return S_OK;
}

CPlacementObject * CPlacementObject::Create(LPDIRECT3DDEVICE9	pGraphicDev, wstring wstrName, int iType, int iIndex)
{
	CPlacementObject *	pInstance = new CPlacementObject(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(wstrName, iType, iIndex)))
	{
		Safe_Release(pInstance);
		MSG_BOX("PlacementObject Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlacementObject::Free()
{	
	__super::Free();
}


