#include "stdafx.h"
#include "..\Header\Terrain.h"

#include "Export_Utility.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: Engine::CGameObject(rhs)
{

}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	

	return S_OK;
}

Engine::_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{

	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CTerrain::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CTerrain::Render_GameObject()
{	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pTextureCom->Set_Texture(0);
	FAILED_CHECK_RETURN(SetUp_Material(), );

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CTerrain::Add_Component()
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

HRESULT CTerrain::SetUp_Material()
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

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain *	pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Terrain Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTerrain::Free()
{	
	__super::Free();
}


