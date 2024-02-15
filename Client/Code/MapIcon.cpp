#include "stdafx.h"
#include "MapIcon.h"

#include "Export_Utility.h"

CMapIcon::CMapIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CMapIcon::CMapIcon(const CMapIcon& rhs)
	: Engine::CUI(rhs)
{
}

CMapIcon::~CMapIcon()
{
}

void CMapIcon::Set_IconType(_int iTypeNum)
{
	switch (iTypeNum)
	{
	case 0:
		m_eIconType = ICON_GOLD;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MapIcon_GoldTexture"));
		break;
	case 1:
		m_eIconType = ICON_ARCADE;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MapIcon_ArcadeTexture"));
		break;
	case 2:
		m_eIconType = ICON_DEVIL;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MapIcon_DevilTexture"));
		break;
	case 3:
		m_eIconType = ICON_MOM;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MapIcon_MomTexture"));
		break;
	case 4:
		m_eIconType = ICON_MONSTRO;
		m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_MapIcon_MonstroTexture"));
		break;
	default:
		break;
	}
}

HRESULT CMapIcon::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_bRender = false; // false

	m_eIconType = ICON_NONE;

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CMapIcon::Update_GameObject(const _float& fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	m_fCurFrame = 0.f;

	return 0;
}

void CMapIcon::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMapIcon::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if(m_bRender && ICON_NONE != m_eIconType)
		m_pBufferCom->Render_Buffer();
}

HRESULT CMapIcon::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture
	
	// Gold
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapIcon_GoldTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MapIcon_GoldTexture", pComponent });
	
	// Arcade
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapIcon_ArcadeTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MapIcon_ArcadeTexture", pComponent });
	
	// Mom
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapIcon_MomTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MapIcon_MomTexture", pComponent });
	
	// Monstro
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapIcon_MonstroTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MapIcon_MonstroTexture", pComponent });
	
	// Devil
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapIcon_DevilTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MapIcon_DevilTexture", pComponent });
	
	// Heart
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
	
#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;

}

CMapIcon* CMapIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CMapIcon* pInstance = new CMapIcon(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MapIcon Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMapIcon::Free()
{
	__super::Free();
}
