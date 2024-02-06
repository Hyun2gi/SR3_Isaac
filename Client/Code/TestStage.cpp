#include "stdafx.h"
#include "..\Header\TestStage.h"

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
#include "Dople.h"

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




CTestStage::CTestStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CTestStage::~CTestStage()
{
}

HRESULT CTestStage::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Monster(L"GameMst"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameItem(L"GameItem"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_MapObj(L"MapObj"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);


	//BoundingBox boundingBox;
	//boundingBox._min = D3DXVECTOR3(-10.0f, -5.0f, -10.0f);
	//boundingBox._max = D3DXVECTOR3(10.0f, 5.0f, 10.0f);

	//pScatter = new CParticleScatter(&boundingBox, 30);
	//pScatter->Ready_Particle(m_pGraphicDev);

	//_vec3 origin(0.0f, 0.0f, 5.0f);
	//pExp = new CParticleExplosion(&origin, 10);
	//pExp->Ready_Particle(m_pGraphicDev);

	//_vec3 origin2(0.0f, 0.0f, 5.0f);
	//pSpl = new CParticleSplash(&origin2, 10);
	//pSpl->Ready_Particle(m_pGraphicDev);
	//pSpl->Create_Texture(L"../Bin/Resource/Texture/Particle/BloodExp2/BloodExp_%d.png", 7);

	//_vec3 origin3(0.0f, 0.0f, 5.0f);
	//pBst = new CParticleBurst(&origin3, 3);
	//pBst->Ready_Particle(m_pGraphicDev);
	//pBst->Create_Texture();


	CPlayer::GetInstance()->Ready_GameObject(m_pGraphicDev);

	//파티클 사용 예
	// 맵에 흩뿌려지는 파티클 (딱히 추가할 일 없음)
	Engine::Create_Scatter(m_pGraphicDev, _vec3(-10.0f, -5.0f, -10.0f), _vec3(10.0f, 5.0f, 10.0f));
	Engine::Create_Explosion(m_pGraphicDev, _vec3(0.0f, 0.0f, 5.0f));
	Engine::Create_Splash(m_pGraphicDev, _vec3(0.0f, 0.0f, 5.0f));
	Engine::Create_Burst(m_pGraphicDev, _vec3(0.0f, 0.0f, 5.0f));

	return S_OK;
}

Engine::_int CTestStage::Update_Scene(const _float& fTimeDelta)
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

	// 자식 클래스 Layer에 Insert
	Insert_Child();

	//Engine::Set_TimeDeltaScale(L"Timer_Second", 0.1f); // Second Timer 테스트용 코드

	//Engine::
	CPlayer::GetInstance()->Update_GameObject(fTimeDelta);

	Engine::Update_Particles(fTimeDelta);
	//pScatter->Update_Particle(fTimeDelta);
	//pExp->Update_Particle(fTimeDelta);
	//pSpl->Update_Particle(fTimeDelta);
	//pBst->Update_Particle(fTimeDelta);
	return __super::Update_Scene(fTimeDelta);
}

void CTestStage::LateUpdate_Scene()
{
	CPlayer::GetInstance()->LateUpdate_GameObject();
	__super::LateUpdate_Scene();
}

void CTestStage::Render_Scene()
{
	//pScatter->Render_GameObject();
	//pExp->Render_GameObject();
	//pSpl->Render_GameObject();
	//pBst->Render_GameObject();
	// DEBUG
}

void CTestStage::Drop_ITem()
{
	// 똥
	if (Get_GameObject(L"GameLogic", L"Poop") != nullptr)
	{
		if (dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_Dead() &&
			!dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_Drop())
		{
			Engine::CGameObject* pGameObject = nullptr;
			// Create_Item( ) : 2인자 : spawn Pos 정수 넣어주면 됨
			ITEM_TYPE eType = dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_ItemType();
			wstring wstrObjTag = dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Get_DropItemTag();

			pGameObject = dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Create_Item(eType, 2, m_mapLayer.at(L"GameItem"));
			m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);

			dynamic_cast<CPoop*>(Get_GameObject(L"GameLogic", L"Poop"))->Set_Drop();
		}
	}

	// 모닥불
	if (Get_GameObject(L"MapObj", L"Campfire") != nullptr)
	{
		if (dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Get_Dead() &&
			!dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Get_Drop())
		{
			Engine::CGameObject* pGameObject = nullptr;

			ITEM_TYPE eType = dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Get_ItemType();
			wstring wstrObjTag = dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Get_DropItemTag();

			pGameObject = dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Create_Item(eType, 2, m_mapLayer.at(L"GameItem"));
			m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);

			dynamic_cast<CCampFire*>(Get_GameObject(L"MapObj", L"Campfire"))->Set_Drop();
		}
	}

	// 슬롯머신

	// 야바위
	// npc 와의 충돌   ----> 야바위 객체가 아니어도 해당 함수가 호출됨 (이것까지 레이어를 나누어야 하는...? 먼가 이상함 아닌가?)
	if (Get_GameObject(L"MapObj", L"ShellGame") != nullptr)
	{
		if (dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc() != nullptr)
		{
			if (!dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc()->Get_NPC_Game())
			{
				CGameObject* pShellNpc = m_mapLayer.at(L"MapObj")->Collision_GameObject(CPlayer::GetInstance());
				if (pShellNpc)
				{
					dynamic_cast<CShellNpc*>(dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc())->Set_NpC_Game();
				}
			}
		}

	}

	// shell 과의 충돌
	if (Get_GameObject(L"MapObj", L"ShellGame") != nullptr)
	{
		//if(dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame")))
	}
}

void CTestStage::Insert_Child()
{
	// Normal Fly 추가
	if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"AttackFly") != nullptr &&
		m_mapLayer.at(L"GameMst")->Get_GameObject(L"NormalFly") == nullptr)
	{
		dynamic_cast<CAttackFly*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"AttackFly"))
			->Set_NormalFly_ToStage(m_mapLayer.at(L"GameMst"));
	}

	// Fire (Campfire) 추가
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Campfire") != nullptr &&
		m_mapLayer.at(L"MapObj")->Get_GameObject(L"Fire") == nullptr)
	{
		dynamic_cast<CCampFire*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"Campfire"))
			->Set_Fire_ToStage(m_mapLayer.at(L"MapObj"));
	}
}

HRESULT CTestStage::Ready_Layer_Environment(const _tchar * pLayerTag)
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

HRESULT CTestStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	CPlayer::GetInstance()->Set_LayerTag((_tchar*)pLayerTag);

	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	/*for (_int i = 0; i < 50; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	}*/

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CTestStage::Ready_Layer_Monster(const _tchar* pLayerTag)
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

	// Dople
	pGameObject = CDople::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dople", pGameObject), E_FAIL);

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

HRESULT CTestStage::Ready_Layer_GameItem(const _tchar* pLayerTag)
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

HRESULT CTestStage::Ready_Layer_MapObj(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

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

	//// SlotMC
	//pGameObject = CSlotMC::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//pGameObject->Set_MyLayer(pLayerTag);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);

	// Shop
	pGameObject = CShop::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shop", pGameObject), E_FAIL);

	// Shell Game
	pGameObject = CShellGame::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ShellGame", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CTestStage::Ready_Layer_Door(const _tchar* pLayerTag)
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

HRESULT CTestStage::Ready_Layer_UI(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;



	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CTestStage::Ready_LightInfo()
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

void CTestStage::Run_Collision_Func()
{
	// 충돌처리하는 함수
	CGameObject* pObj = m_mapLayer.at(L"GameItem")->Collision_GameObject(CPlayer::GetInstance());

	if (pObj)
	{
		//충돌됨
		dynamic_cast<CItem*>(pObj)->Run_Item_Effect();
	}
}

void CTestStage::Door_Collision()
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

void CTestStage::Moster_Collision()
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

			if (pMonster)
			{
				dynamic_cast<CMonster*>(pMonster)->Hit();
				break;
			}
			else
				++iter;
		}
	}

	if (Get_GameObject(L"MapObj", L"Spike") != nullptr && Get_GameObject(L"GameMst", L"Dople") != nullptr)
	{
		CGameObject* pDople = m_mapLayer.at(L"GameMst")->Collision_GameObject(Get_GameObject(L"MapObj", L"Spike"));
		if (pDople)
		{
			//dynamic_cast<CShellNpc*>(dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc())->Set_NpC_Game();
			dynamic_cast<CDople*>(pDople)->Hit();
		}
	}
}

void CTestStage::MapObj_Collision()
{
	// MapObj 와 PlayerBullet 의 충돌
	if (m_mapLayer.at(L"MapObj") != nullptr)
	{
		list<CGameObject*>* pBulletList = CPlayer::GetInstance()->Get_Player_BullletList();

		for (list<CGameObject*>::iterator iter = pBulletList->begin();
			iter != pBulletList->end();)
		{
			CGameObject* pMapObj = m_mapLayer.at(L"MapObj")->Collision_GameObject(*iter); // 충돌이 안 됨

			if (pMapObj) // 똥이고 모닥불일 때만
			{
				dynamic_cast<CMapObj*>(pMapObj)->Set_Hit();
				break;
			}
			else
				++iter;
		}
	}

	// SlotMC 과 Shell Game 은 Player 와 충돌

	// Shop Npc 는 Epic 과 충돌
}

CTestStage* CTestStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestStage* pInstance = new CTestStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTestStage::Free()
{
	//Safe_Release(pScatter);
	//Safe_Release(pExp);
	//Safe_Release(pSpl);
	//Safe_Release(pBst);
	__super::Free();
}
