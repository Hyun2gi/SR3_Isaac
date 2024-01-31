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
#include "Mom.h"
#include "MomParts.h"

#include "Poop.h"
#include "CampFire.h"
#include "Spike.h"
#include "SlotMC.h"

#include "BackGround.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "Effect.h"
#include "Coin.h"
#include "Pill.h"
#include "BrimStone.h"
#include "SadOnion.h"
#include "WhipWorm.h"
#include "Epic.h"
#include "Heart.h"
#include "HeartHalf.h"


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

	CPlayer::GetInstance()->Ready_GameObject(m_pGraphicDev);

	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{
	// TODO: 충돌 함수 사용 예
	// 이렇게 사용하면 GameLogic 내에 있는 모든 게임오브젝트랑
	// 대상과 충돌했는지를 검사하고 충돌이 되었다면 충돌된 GameObject 객체를 반환해준다.'
	// 충돌한 객체가 없으면 nullptr을 반환해준다
	//CGameObject* pObj = m_mapLayer.at(L"GameLogic")->Collision_GameObject(/*대상*/);
	//if (pObj)
	//{
	//	//충돌됨
	//}
	
	CPlayer::GetInstance()->Update_GameObject(fTimeDelta);
	return __super::Update_Scene(fTimeDelta);
}

void CStage::LateUpdate_Scene()
{
	CPlayer::GetInstance()->LateUpdate_GameObject();
	__super::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
	// DEBUG
}

HRESULT CStage::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 10.f, -5.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f),
		D3DXToRadian(60.f),
		(_float)WINCX / WINCY,
		0.1f,
		1000.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	CPlayer::GetInstance()->Set_LayerTag((_tchar*)pLayerTag);

	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

#pragma region Monster

	//// Fly
	//for (int i = 0; i < 10; ++i)
	//{
	//	pGameObject = CFly::Create(m_pGraphicDev, i * 2);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	pGameObject->Set_MyLayer(pLayerTag);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);
	//}

	// Attack Fly
	/*pGameObject = CAttackFly::Create(m_pGraphicDev, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	dynamic_cast<CAttackFly*>(pGameObject)->Set_CenterObj();
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CenterFly", pGameObject), E_FAIL);

	for (int i = 1; i < 13; ++i)
	{
		pGameObject = CAttackFly::Create(m_pGraphicDev, i);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		pGameObject->Set_MyLayer(pLayerTag);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);
	}*/

	//// Dip
	//for (int i = 0; i < 5; ++i)
	//{
	//	pGameObject = CDip::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	pGameObject->Set_MyLayer(pLayerTag);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);
	//}

	//// Pacer
	//for (int i = 0; i < 6; ++i)
	//{
	//	pGameObject = CPacer::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	pGameObject->Set_MyLayer(pLayerTag);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);
	//}

	//// Squirt
	//pGameObject = CSquirt::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Squirt", pGameObject), E_FAIL);

	// //Leaper
	//pGameObject = CLeaper::Create(m_pGraphicDev, 0);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

	//// Charger
	//pGameObject = CCharger::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Charger", pGameObject), E_FAIL);

	//// Monstro
	//pGameObject = CMonstro::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monstro", pGameObject), E_FAIL);

	//// Mom
	//pGameObject = CMom::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Mom", pGameObject), E_FAIL);

	//// Mom's Parts
	//for (int i = 0; i < 4; ++i)
	//{
	//	pGameObject = CMomParts::Create(m_pGraphicDev, i);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	pGameObject->Set_MyLayer(pLayerTag);
	//	dynamic_cast<CMomParts*>(pGameObject)->Setting_Value();
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MomParts", pGameObject), E_FAIL);

	//}

#pragma endregion Monster

#pragma region Object

	// Poop
	pGameObject = CPoop::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Poop", pGameObject), E_FAIL);

	// CampFire
	pGameObject = CCampFire::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Campfire", pGameObject), E_FAIL);

	// Spike
	pGameObject = CSpike::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Spike", pGameObject), E_FAIL);

	// SlotMC
	pGameObject = CSlotMC::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);


#pragma endregion Object



	// Coin
	pGameObject = CCoin::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Coin", pGameObject), E_FAIL);

	// Heart
	pGameObject = CHeart::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Heart", pGameObject), E_FAIL);

	// HeartHalf
	pGameObject = CHeartHalf::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HeartHalf", pGameObject), E_FAIL);


	//// Pill
	//pGameObject = CPill::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pill", pGameObject), E_FAIL);

	//// BrimStone
	//pGameObject = CBrimStone::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BrimStone", pGameObject), E_FAIL);

	//// Onion
	//pGameObject = CSadOnion::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SadOnion", pGameObject), E_FAIL);

	//// WhipWorm
	//pGameObject = CWhipWorm::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WhipWorm", pGameObject), E_FAIL);

	//// Epic
	//pGameObject = CEpic::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Epic", pGameObject), E_FAIL);


	/*for (_int i = 0; i < 50; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	}*/

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;



	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_LightInfo()
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pInstance = new CStage(pGraphicDev);

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
