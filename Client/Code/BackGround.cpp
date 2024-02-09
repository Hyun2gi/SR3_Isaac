#include "stdafx.h"
#include "..\Header\BackGround.h"

#include "Export_Utility.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: Engine::CGameObject(rhs)
{
}

CBackGround::~CBackGround()
{
}

HRESULT CBackGround::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Engine::_int CBackGround::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 2 * fTimeDelta * 3.f;

	if (2 < m_fFrame)
		m_fFrame = 0.f;

	Engine::Add_RenderGroup(RENDER_PRIORITY, this);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CBackGround::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CBackGround::Render_GameObject()
{	
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBackGround::Add_Component()
{
	CComponent*		pComponent = nullptr;
		
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TitleTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TitleTexture", pComponent });

	/*pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });*/
		
	return S_OK;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBackGround *	pInstance = new CBackGround(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("BackGround Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBackGround::Free()
{	
	__super::Free();
}
