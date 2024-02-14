#include "stdafx.h"
#include "MiniMap.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CMiniMap::CMiniMap(const CMiniMap& rhs)
	: Engine::CUI(rhs)
{
}

CMiniMap::~CMiniMap()
{
}

HRESULT CMiniMap::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = true; // 필요할 지? 연출 끝나면 나오는 식으로 (추후 false로)

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CMiniMap::Update_GameObject(const _float& fTimeDelta)
{
	m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);



	return 0;
}

void CMiniMap::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMiniMap::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if (m_bRender)
		m_pBufferCom->Render_Buffer();

}

HRESULT CMiniMap::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

//#pragma region Texture
//
//	// Icon
//	// Gold
//	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Arcade
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Mom
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Monstro
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Devil
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Heart
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//	// 이거 아이콘 으로 가야함
//
//#pragma endregion Texture

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniMapToolTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MiniMapToolTexture", pComponent });


	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

CMiniMap* CMiniMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CMiniMap* pInstance = new CMiniMap(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MiniMap Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMiniMap::Free()
{
	__super::Free();
}
