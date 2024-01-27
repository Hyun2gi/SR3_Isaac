#include "stdafx.h"
#include "..\Header\Stage.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "Monster.h"

#include "Fly.h"
#include "AttackFly.h"
#include "Dip.h"
#include "Pacer.h"
#include "Squirt.h"
#include "Leaper.h"
#include "Charger.h"

#include "Monstro.h"

#include "BackGround.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "Effect.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);


	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{	
	return __super::Update_Scene(fTimeDelta);
}

void CStage::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
	// DEBUG
}

HRESULT CStage::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, 
		&_vec3(0.f, 10.f, -5.f),
		&_vec3(0.f, 0.f, 1.f), 
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f),
		(_float)WINCX / WINCY, 
		0.1f,
		1000.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);
	
	
		
	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	//// Fly
	//for (int i = 0; i < 10; ++i)
	//{
	//	pGameObject = CFly::Create(m_pGraphicDev, i * 2);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);
	//}

	//// Attack Fly
	//pGameObject = CAttackFly::Create(m_pGraphicDev, 0);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//dynamic_cast<CAttackFly*>(pGameObject)->Set_CenterObj();
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CenterFly", pGameObject), E_FAIL);

	//for (int i = 1; i < 13; ++i)
	//{
	//	pGameObject = CAttackFly::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);
	//}

	//// Dip
	//for (int i = 0; i < 5; ++i)
	//{
	//	pGameObject = CDip::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);
	//}

	//// Pacer
	//for (int i = 0; i < 6; ++i)
	//{
	//	pGameObject = CPacer::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);
	//}

	//// Squirt
	//pGameObject = CSquirt::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Squirt", pGameObject), E_FAIL);

	//// Leaper
	//pGameObject = CLeaper::Create(m_pGraphicDev, 0);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

	//// Charger
	//pGameObject = CCharger::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Charger", pGameObject), E_FAIL);

	// Monstro
	pGameObject = CMonstro::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monstro", pGameObject), E_FAIL);

	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

	/*for (_int i = 0; i < 50; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	}*/
	
	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;



	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_LightInfo()
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse   = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular  = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient	 = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

CStage * CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage *	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CStage::Free()
{
	__super::Free();
}
