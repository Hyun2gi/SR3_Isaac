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
#include "Shop.h"
#include "ShellGame.h"

#include "Door.h"
#include "PlayerBullet.h"

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
	FAILED_CHECK_RETURN(Ready_Layer_Monster(L"GameMst"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameItem(L"GameItem"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);
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

	// 충돌처리 함수
	Run_Collision_Func();
	Door_Collision();
	Moster_Collision();
	MapObj_Collision();

	// 아이템 드랍
	Drop_ITem();

	// Normal Fly 추가
	//if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"NormalFly") == nullptr)
	//{
	//	dynamic_cast<CAttackFly*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"AttackFly"))
	//		->Set_NormalFly_ToStage(m_mapLayer.at(L"GameMst"));
	//}

	//Engine::Set_TimeDeltaScale(L"Timer_Second", 0.1f); // Second Timer 테스트용 코드

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

void CStage::Drop_ITem()
{
	// 똥 / 모닥불 -> 같은 자리에 점프하며 생성
	// 슬롯머신 -> 점프하며 대각선으로 뱉어냄
	// 야바위 -> 점프하며 대각선으로 뱉어냄
	// CGameObject* pObj = m_mapLayer.at(L"GameItem")->Collision_GameObject(CPlayer::GetInstance());
	//Get_GameObject
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);
	
	// 똥
	if (Get_GameObject(L"GameLogic", L"Poop") != nullptr)
	{
		if (dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_Dead() &&
			!dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_Drop())
		{
			Engine::CGameObject* pGameObject = nullptr;
			pGameObject = dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Create_Item(COIN, 2, m_mapLayer.at(L"GameItem"));
			m_mapLayer.at(L"GameItem")->Add_GameObject(L"Coin", pGameObject);
			dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Set_Drop();
		}
}

	//if (Get_GameObject(L"GameLogic", L"Campfire") != nullptr)
	//{
	//	// 모닥불
	//	if (dynamic_cast<CCampFire*>(Get_GameObject(L"GameLogic", L"Campfire"))->Get_Dead())
	//	{
	//		Engine::CGameObject* pGameObject = nullptr;

	//		pGameObject = CHeart::Create(m_pGraphicDev, 2);
	//		pGameObject->Set_MyLayer(L"GameItem");
	//		FAILED_CHECK_RETURN(m_mapLayer.at(L"GameItem")->Add_GameObject(L"Heart", pGameObject));
	//	}
	//}

	// 슬롯머신

	// 야바위
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

	//// Spike
	//pGameObject = CSpike::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Spike", pGameObject), E_FAIL);

	//// SlotMC
	//pGameObject = CSlotMC::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);

	//// Shop
	pGameObject = CShop::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shop", pGameObject), E_FAIL);

	//// Shell Game
	//pGameObject = CShellGame::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ShellGame", pGameObject), E_FAIL);

#pragma endregion Object



	/*for (_int i = 0; i < 50; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	}*/

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStage::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

 //	// Fly
	//for (int i = 0; i < 10; ++i)
	//{
	//	pGameObject = CFly::Create(m_pGraphicDev, i * 2);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	pGameObject->Set_MyLayer(pLayerTag);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);
	//}

	//// Attack Fly
	//pGameObject = CAttackFly::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);

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

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameItem(const _tchar* pLayerTag)
{
	// 아이템 관련

	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	//// Coin
	//pGameObject = CCoin::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Coin", pGameObject), E_FAIL);

	//// Heart
	//pGameObject = CHeart::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Heart", pGameObject), E_FAIL);

	//// HeartHalf
	//pGameObject = CHeartHalf::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HeartHalf", pGameObject), E_FAIL);


	//// Pill
	//pGameObject = CPill::Create(m_pGraphicDev, 0, );
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pill", pGameObject), E_FAIL);

	//// BrimStone
	//pGameObject = CBrimStone::Create(m_pGraphicDev,0);
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

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStage::Ready_Layer_Door(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	// Door
	pGameObject = CDoor::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	dynamic_cast<CDoor*>(pGameObject)->Set_Theme("Normal");
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);

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

void CStage::Run_Collision_Func()
{
	// 충돌처리하는 함수
	CGameObject* pObj = m_mapLayer.at(L"GameItem")->Collision_GameObject(CPlayer::GetInstance());

	if (pObj)
	{
		//충돌됨
		dynamic_cast<CItem*>(pObj)->Run_Item_Effect();
	}
}

void CStage::Door_Collision()
{
	if (m_mapLayer.at(L"GameDoor") != nullptr)
	{
		if (dynamic_cast<CDoor*>(m_mapLayer.at(L"GameDoor")->Get_GameObject(L"Door"))->Get_Open()) // 문이 열렸을 경우에만
		{
			CGameObject* pObj = m_mapLayer.at(L"GameDoor")->Collision_GameObject(CPlayer::GetInstance());

			if (pObj) // 충돌된 문 존재
			{
				// 스테이지 변경
				
			}
		}
	}
}

void CStage::Moster_Collision()
{
	// 몬스터 충돌 관련 처리 함수

	// 몬스터 <- 총알
	if (m_mapLayer.at(L"GameMst") != nullptr)
	{
		list<CGameObject*>* pBulletList = CPlayer::GetInstance()->Get_Player_BullletList();

		for (list<CGameObject*>::iterator iter = pBulletList->begin();
			iter != pBulletList->end();)
		{
			CGameObject* pMonster = m_mapLayer.at(L"GameMst")->Collision_GameObject(*iter);
			// AttackFly 는 고민해볼 것
			if (pMonster)
			{
				dynamic_cast<CMonster*>(pMonster)->Hit();
				break;
			}
			else
				++iter;
		}
	}
}

void CStage::MapObj_Collision()
{
	//if(m_mapLayer.at(L"GameLogic"))
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
