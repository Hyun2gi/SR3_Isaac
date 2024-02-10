#include "stdafx.h"
#include "..\Header\Ending.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "BackGround.h"
#include "Stage.h"
#include "TestStage.h"
#include "LoadStage.h"
#include "Player.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CEnding::~CEnding()
{
}

HRESULT CEnding::Ready_Scene()
{
	//Engine::PlayBGM(L"Title.ogg", 1.f);

	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	
	//m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	//m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE_1);
	//NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

Engine::_int CEnding::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = __super::Update_Scene(fTimeDelta);



	return iExit;
}

void CEnding::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CEnding::Render_Scene()
{

}

HRESULT CEnding::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Intro/intro_bg.png")), E_FAIL);
	// ·Î°í ÅØ½ºÃÄ ¹Ù²åÀ½ _ º¸¹Î

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Intro/Title_%d.png", 2)), E_FAIL);

	return S_OK;
}

HRESULT CEnding::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);
	
	
	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

CEnding * CEnding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEnding *	pInstance = new CEnding(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Logo Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CEnding::Free()
{
	//Safe_Release(m_pLoading);

	__super::Free();
}
