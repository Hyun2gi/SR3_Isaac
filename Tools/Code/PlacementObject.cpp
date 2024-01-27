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
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	CGameObject::Update_GameObject(fTimeDelta);
	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

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

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CMouseObjectImg::Swap_Texture()
{
	Safe_Release(m_pTextureCom);

	wstring wstr;
	wstr.assign(m_strCurTextureName.begin(), m_strCurTextureName.end());

	int iIndex = 0;

	auto found = find(m_vecTextureNames.begin(), m_vecTextureNames.end(), wstr);

	if (found == m_vecTextureNames.end())
	{
		m_vecTextureNames.push_back(wstr);

		iIndex = m_vecTextureNames.size() - 1;
	}
	else
	{
		iIndex = m_vecTextureNames.end() - found - 1;
	}

	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(wstr.c_str()));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ m_vecTextureNames[iIndex].c_str() , pComponent});
}

HRESULT CMouseObjectImg::Add_Component()
{
	CComponent*		pComponent = nullptr;
			
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_Object_0"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Object_0", pComponent });
	m_vecTextureNames.push_back(L"Proto_Object_0");

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Calculator", pComponent });
		
	return S_OK;
}

CMouseObjectImg * CMouseObjectImg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMouseObjectImg *	pInstance = new CMouseObjectImg(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MouseObjectImg Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMouseObjectImg::Free()
{	
	__super::Free();
}


