#include "stdafx.h"
#include "..\Header\CubeObject.h"

#include "Export_Utility.h"

CCubeObject::CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bIsArrived(false)
{
}

CCubeObject::CCubeObject(const CCubeObject& rhs)
	: Engine::CGameObject(rhs), m_bIsArrived(false)
{

}

CCubeObject::~CCubeObject()
{
}

HRESULT CCubeObject::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fS = 1.f;

	return S_OK;
}

Engine::_int CCubeObject::Update_GameObject(const _float& fTimeDelta)
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	if (m_fS > 0.88f)
	{
		m_fS = m_fS - (0.03f * fTimeDelta);
		D3DXVec3Lerp(&m_vTempPos, &m_vDstPos, &m_pTransformCom->m_vInfo[INFO_POS], m_fS);
	}
	else
	{
		m_bIsArrived = true;
	}

	m_pTransformCom->m_vInfo[INFO_POS] = m_vTempPos;
	//m_pTransformCom->Set_Pos(&vTemp);

	//if(m_vDstPos == vTemp)

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CCubeObject::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CCubeObject::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CCubeObject::Set_Cute_Texture(const _tchar* pTextureTag)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(pTextureTag));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ pTextureTag, pComponent });
}

HRESULT CCubeObject::Add_Component()
{
	CComponent*		pComponent = nullptr;
		
	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
	m_pTransformCom->m_vInfo[INFO_POS] = { _float(rand() % 1000) - 500, _float(rand() % 1000) - 500, _float(rand() % 1000) - 500 };
		
	return S_OK;
}

CCubeObject * CCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubeObject *	pInstance = new CCubeObject(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SkyBox Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCubeObject::Free()
{	
	__super::Free();
}


