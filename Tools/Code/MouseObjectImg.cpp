#include "../Include/stdafx.h"
#include "..\Header\MouseObjectImg.h"

#include "Export_Utility.h"

CMouseObjectImg::CMouseObjectImg(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CMouseObjectImg::CMouseObjectImg(const CMouseObjectImg& rhs)
	: Engine::CGameObject(rhs)
{

}

CMouseObjectImg::~CMouseObjectImg()
{
}

HRESULT CMouseObjectImg::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	

	return S_OK;
}

Engine::_int CMouseObjectImg::Update_GameObject(const _float& fTimeDelta)
{

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CMouseObjectImg::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMouseObjectImg::Render_GameObject()
{	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pTextureCom->Set_Texture(0);
	FAILED_CHECK_RETURN(SetUp_Material(), );

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CMouseObjectImg::Add_Component()
{
	CComponent*		pComponent = nullptr;
			
	pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Engine::Clone_Proto(L"Proto_TerrainTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TerrainTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TerrainTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
		
	return S_OK;
}

HRESULT CMouseObjectImg::SetUp_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse  = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tMtrl.Specular  = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

CMouseObjectImg * CMouseObjectImg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMouseObjectImg *	pInstance = new CMouseObjectImg(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Terrain Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMouseObjectImg::Free()
{	
	__super::Free();
}


