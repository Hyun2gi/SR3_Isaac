#include "stdafx.h"
#include "..\Header\LoadStage.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "PlayerBullet.h"

#include "StageLoadMgr.h"

//씬
#include "Ending.h"
#include "BossFight.h"

//환경
#include "Terrain.h"
#include "DynamicCamera.h"
#include "DynamicCamera2.h"
#include "Floor.h"
#include "Wall.h"
#include "SkyBox.h"

//몬스터
#include "Fly.h"
#include "AttackFly.h"
#include "Dip.h"
#include "Pacer.h"
#include "Squirt.h"
#include "Leaper.h"
#include "Charger.h"
#include "Dople.h"

//보스
#include "Monstro.h"
#include "Mom.h"
#include "MomParts.h"

//오브젝트
#include "Poop.h"
#include "CampFire.h"
#include "Spike.h"
#include "SlotMC.h"
#include "Shop.h"
#include "ShellGame.h"
#include "Door.h"
#include "Obstacle.h"
#include "MoveXObstacle.h"
#include "MoveZObstacle.h"
#include "Devil.h"

// 총알
#include "EpicBullet.h"

//아이템
#include "Coin.h"
#include "Pill.h"
#include "BrimStone.h"
#include "SadOnion.h"
#include "WhipWorm.h"
#include "Epic.h"
#include "Heart.h"
#include "HeartHalf.h"

// UI
#include "Menu.h"
#include "PlayerCoin.h"
#include "PLCoinFont.h"
#include "PlayerHP.h"
#include "ItemFontUI.h"
#include "MiniMap.h"

CLoadStage::CLoadStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev),
	m_bStartScene(false), m_bEndingPlay(false)
{
}

CLoadStage::~CLoadStage()
{
}

HRESULT CLoadStage::Ready_Scene(int iType)
{
	m_iCurStageKey = iType;
	m_vecMonsterCount.resize(MONSTER_TYPE_END);
	m_bIsCreated = false;
	m_bMenu = false;
	m_bBGM = true;

	//Engine::Create_Scatter(m_pGraphicDev);

	FAILED_CHECK_RETURN(CStageLoadMgr::GetInstance()->Ready_StageLoadMgr(), E_FAIL);

	//FAILED_CHECK_RETURN(Load_Level_Data(), E_FAIL);
	//FAILED_CHECK_RETURN(Load_Stage_Data(), E_FAIL);
	//FAILED_CHECK_RETURN(Load_Connected_Stage_Theme(), E_FAIL);
	//FAILED_CHECK_RETURN(Load_Stage_Design_Data(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_RoomObject(L"RoomObject"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

	m_iLoadDataSize = mapLoadObj.size();

	return S_OK;
}

Engine::_int CLoadStage::Update_Scene(const _float& fTimeDelta)
{
	Engine::Update_Particles(fTimeDelta);

	if (GetAsyncKeyState(VK_DOWN))
	{
		Create_Map_Particles();
	}

	if (m_bIsCreated)
	{
		Setting_UI();

		// 자식 클래스 Layer에 Insert
		Insert_Child();

		// 충돌처리 함수
		Item_Collision();
		Door_Collision();
		Moster_Collision();
		MapObj_Collision();
		Player_Collision_With_Monster();
		Obstacle_Collsion();

		// 아이템 드랍
		Drop_ITem();

		// MiniMap 업데이트
		Update_MiniMap();
		// Mom 스테이지 몬스터 생성 기믹
		if (10 == m_iCurStageKey)
		{
			Link_MomParts_ToLayer();
		}
	}


	_int	iExit = __super::Update_Scene(fTimeDelta);

	CPlayer::GetInstance()->Update_GameObject(fTimeDelta);

	// 맨 처음 방에서는 player에서 bgm 나와야해서 player update보다 순서 뒤로
	// 비지엠 인트로 먼저 재생
	//BGM_INTRO_START();
	
	

	// 비지엠
	BGM_START();

	if (Check_Cube_Arrived() && !m_bIsCreated)
	{
		// m_bStartScene : 한번 왔다간 방인지 아닌지 확인해주는 변수
		if (!m_bStartScene)
		{
			CPlayer::GetInstance()->Set_StopShaking();

			if (m_iCurStageKey == 8)
			{
				// 도플방일때
				CPlayer::GetInstance()->Set_Camera_Cinemachine_04();
			}

			//CPlayer::GetInstance()->Set_Camera_Cinemachine_02();
			if (m_iLoadDataSize <= m_iCreatedCnt)
			{
				m_bIsCreated = true;
			}
		}
		else
		{
			// 한번 왔다간 방임
			CPlayer::GetInstance()->Set_StartCameraMouse();
			m_bIsCreated = true;
		}

		
		//Setting_UI(); // UI 생성
	}
	

	//연출이 필요하다면 큐브가 다 온 이후부터 타이머를 돌면서 하나씩 생성시켜준다.
	if (!m_bStartScene && !m_bIsLoadDataCreated)
	{
		if (m_iCreatedCnt >= m_iLoadDataSize)
		{
			m_bIsLoadDataCreated = true;
			
			// 플레이어 무적 시간 종료 및 플레이어로 돌아가기
			CPlayer::GetInstance()->Set_MapCinemachine(false);

			if (m_iCurStageKey == 8)
			{
				// 도플방일때
				CPlayer::GetInstance()->Set_OnDople();
			}
			else
			{
				// 도플방아닐때
				CPlayer::GetInstance()->Set_Camera_Cinemachine_03();
			}
		}

		if (Check_Cube_Arrived() && m_iCreatedCnt < m_iLoadDataSize && !m_bIsLoadDataCreated)
		{
			// 카메라가 맵 안에 나오고 플레이어 랜더는 꺼짐
			

			m_fSpawnTimer += fTimeDelta;

			//0.2초마다 맵툴로 설치해둔 몬스터/오브젝트를 스폰한다.
			if (0.2f < m_fSpawnTimer)
			{
				m_fSpawnTimer = 0.f;

				FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"MapObj"), E_FAIL);
				FAILED_CHECK_RETURN(Ready_Layer_GameMonster(L"GameMst"), E_FAIL);
				FAILED_CHECK_RETURN(Ready_Layer_GameItem(L"GameItem"), E_FAIL);

				if (1 > m_iCreatedCnt)
					FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);

				++m_iCreatedCnt;
			}
		}
	}
	//연출이 필요 없고 생성되지 않은 상황이면 한번에 생성시켜준다.
	else if (!m_bIsLoadDataCreated)
	{

		FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"MapObj"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_GameMonster(L"GameMst"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_GameItem(L"GameItem"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);
		m_bIsLoadDataCreated = true;

		// 플레이어 무적 시간 종료 및 플레이어로 돌아가기
		CPlayer::GetInstance()->Set_MapCinemachine(false);

		if (m_iCurStageKey == 8)
		{
			//도플방일때
			CPlayer::GetInstance()->Set_OnDople();
		}
		else
		{

		}
	}

	if (m_bIsLoadDataCreated) // 연출이 끝난 경우
	{
		for (auto& iter : m_mapLayer.at(L"GameMst")->Get_ObjectMap()) // 몬스터 움직임 활성화
		{
			dynamic_cast<CMonster*>(iter.second)->Set_LoadCreateEnd();
		}
	}

	//타임 델타 스케일 조절 예시 _ 사용
	if (Engine::Key_Down(DIK_P))
	{
		m_bEndingPlay = true;
	}
	if (Engine::Key_Down(DIK_RETURN))
	{
		dynamic_cast<CMenu*>(m_mapLayer.at(L"UI")->Get_GameObject(L"Menu"))->Set_Render(); // 메뉴 UI 비/활성화

		for (auto& iter : m_mapLayer.at(L"GameMst")->Get_ObjectMap())
		{
			dynamic_cast<CMonster*>(iter.second)->Set_Time_Scale(); // 몬스터 행동 제어 여부 변경
		}
	}

	if (m_bEndingPlay)
		Play_Ending(fTimeDelta);

	return iExit;
}

void CLoadStage::LateUpdate_Scene()
{
	CPlayer::GetInstance()->LateUpdate_GameObject();
	__super::LateUpdate_Scene();

	if (m_bIsCreated)
	{
		Copy_Stage();
		Check_All_Dead();
		Door_Collision();
	}
}

void CLoadStage::Render_Scene()
{
	// DEBUG

}


HRESULT CLoadStage::Ready_Layer_GameObject(const _tchar* pLayerTag)
{
	CPlayer::GetInstance()->Set_LayerTag((_tchar*)pLayerTag);

	Engine::CLayer* pLayer;

	if (m_bStartScene)
	{
		pLayer = Engine::CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		for (auto& iter : mapLoadObj)
		{
			switch (iter.second.iType)
			{
			case OBJECT:
			{
				switch (iter.second.iIndex)
				{
				case POOP:
				{
					pGameObject = CPoop::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Poop", pGameObject), E_FAIL);

					break;
				}
				case CAMPFIRE:
				{
					pGameObject = CCampFire::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Campfire", pGameObject), E_FAIL);

					break;
				}
				case SPIKE:
				{
					pGameObject = CSpike::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Spike", pGameObject), E_FAIL);

					break;
				}
				case SHELL_GAME:
				{
					pGameObject = CShellGame::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ShellGame", pGameObject), E_FAIL);

					break;
				}
				case SLOT_MC:
				{
					pGameObject = CSlotMC::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);

					break;
				}

				case SHOP:
				{
					pGameObject = CShop::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shop", pGameObject), E_FAIL);

					break;
				}

				case OBSTACLE:
				{
					pGameObject = CObstacle::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle", pGameObject), E_FAIL);

					break;
				}

				case OBSTACLE_X:
				{
					pGameObject = CMoveXObstacle::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;

					int randNum1 = rand() % 10 + 5;
					int randNum2 = rand() % 10 + 5;
					int randNumSpeed = rand() % 15 + 10;

					dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Distance_Left(randNum1);
					dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Distance_Right(randNum2);
					dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Speed(randNumSpeed);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle_X", pGameObject), E_FAIL);

					break;
				}

				case OBSTACLE_Z:
				{
					pGameObject = CMoveZObstacle::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;

					int randNum1 = rand() % 10 + 5;
					int randNum2 = rand() % 10 + 5;
					int randNumSpeed = rand() % 15 + 10;

					dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Distance_Up(randNum1);
					dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Distance_Down(randNum2);
					dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Speed(randNumSpeed);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle_Z", pGameObject), E_FAIL);

					break;
				}
				case DEVIL:
				{
					pGameObject = CDevil::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Devil", pGameObject), E_FAIL);

					break;
				}

				}
				break;
			}

			}
		}

	}
	else
	{
		auto	pLayerFind = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

		if (pLayerFind == m_mapLayer.end())
		{
			pLayer = Engine::CLayer::Create();
		}
		else
		{
			pLayer = (*pLayerFind).second;
		}

		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		auto iter = mapLoadObj.at(m_iCreatedCnt);

		switch (iter.iType)
		{
		case OBJECT:
		{
			switch (iter.iIndex)
			{
			case POOP:
			{
				pGameObject = CPoop::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Poop", pGameObject), E_FAIL);

				break;
			}

			case CAMPFIRE:
			{
				pGameObject = CCampFire::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Campfire", pGameObject), E_FAIL);

				break;
			}

			case SPIKE:
			{
				pGameObject = CSpike::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Spike", pGameObject), E_FAIL);

				break;
			}

			case SHELL_GAME:
			{
				pGameObject = CShellGame::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ShellGame", pGameObject), E_FAIL);

				break;
			}

			case SLOT_MC:
			{
				pGameObject = CSlotMC::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);

				break;
			}

			case SHOP:
			{
				pGameObject = CShop::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shop", pGameObject), E_FAIL);

				break;
			}

			case OBSTACLE:
			{
				pGameObject = CObstacle::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle", pGameObject), E_FAIL);

				break;
			}

			case OBSTACLE_X:
			{
				pGameObject = CMoveXObstacle::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;

				int randNum1 = rand() % 10 + 5;
				int randNum2 = rand() % 10 + 5;
				int randNumSpeed = rand() % 30 + 10;

				dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Distance_Left(randNum1);
				dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Distance_Right(randNum2);
				dynamic_cast<CMoveXObstacle*>(pGameObject)->Set_Speed(randNumSpeed);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle_X", pGameObject), E_FAIL);

				break;
			}

			case OBSTACLE_Z:
			{
				pGameObject = CMoveZObstacle::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;

				int randNum1 = rand() % 10 + 5;
				int randNum2 = rand() % 10 + 5;
				int randNumSpeed = rand() % 30 + 10;

				dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Distance_Up(randNum1);
				dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Distance_Down(randNum2);
				dynamic_cast<CMoveZObstacle*>(pGameObject)->Set_Speed(randNumSpeed);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Obstacle_Z", pGameObject), E_FAIL);

				break;
			}

			}

			break;

		}
		}
	}

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_GameMonster(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer;

	if (m_bStartScene)
	{
		pLayer = Engine::CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		for (auto& iter : mapLoadObj)
		{
			switch (iter.second.iType)
			{
			case MONSTER:
			{
				switch (iter.second.iIndex)
				{
				case FLY:
				{
					pGameObject = CFly::Create(m_pGraphicDev, m_vecMonsterCount[FLY]++);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);

					break;
				}
				case ATTACK_FLY:
				{
					pGameObject = CAttackFly::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);

					++m_vecMonsterCount[ATTACK_FLY];

					break;
				}
				case PACER:
				{
					pGameObject = CPacer::Create(m_pGraphicDev, m_vecMonsterCount[PACER]++);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);

					break;
				}
				case LEAPER:
				{
					pGameObject = CLeaper::Create(m_pGraphicDev, m_vecMonsterCount[LEAPER]++);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

					break;
				}
				case SQUIRT:
				{
					pGameObject = CSquirt::Create(m_pGraphicDev, m_vecMonsterCount[SQUIRT]++);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Squirt", pGameObject), E_FAIL);

					++m_vecMonsterCount[SQUIRT];

					break;
				}
				case DIP:
				{
					pGameObject = CDip::Create(m_pGraphicDev, m_vecMonsterCount[DIP]++);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);

					break;
				}
				case DOPLE:
				{
					pGameObject = CDople::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dople", pGameObject), E_FAIL);

					m_vecMonsterCount[DOPLE]++;

					break;
				}
				case CHARGER:
				{
					pGameObject = CCharger::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Charger", pGameObject), E_FAIL);

					++m_vecMonsterCount[CHARGER];

					break;
				}
				}
				break;
			}
			case BOSS:
			{
				switch (iter.second.iIndex)
				{
				case MONSTRO:
				{
					pGameObject = CMonstro::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monstro", pGameObject), E_FAIL);

					break;
				}
				case MOM:
				{
					pGameObject = CMom::Create(m_pGraphicDev);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Mom", pGameObject), E_FAIL);

					break;
				}
				case MOM_PARTS:
				{
					for (int i = 0; i < 4; ++i)
					{
						pGameObject = CMomParts::Create(m_pGraphicDev, i);
						NULL_CHECK_RETURN(pGameObject, E_FAIL);
						dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
						dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;
						pGameObject->Set_MyLayer(pLayerTag);
						dynamic_cast<CMomParts*>(pGameObject)->Setting_Value();
						FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MomParts", pGameObject), E_FAIL);
					}
					break;
				}
				}
				break;
			}

			}
		}
	}
	else
	{
		auto	pLayerFind = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

		if (pLayerFind == m_mapLayer.end())
		{
			pLayer = Engine::CLayer::Create();
		}
		else
		{
			pLayer = (*pLayerFind).second;
		}

		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		auto iter = mapLoadObj.at(m_iCreatedCnt);

		switch (iter.iType)
		{
		case MONSTER:
		{
			switch (iter.iIndex)
			{
			case FLY:
			{
				pGameObject = CFly::Create(m_pGraphicDev, m_vecMonsterCount[FLY]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);

				break;
			}
			case ATTACK_FLY:
			{
				pGameObject = CAttackFly::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);

				++m_vecMonsterCount[ATTACK_FLY];

				break;
			}
			case PACER:
			{
				pGameObject = CPacer::Create(m_pGraphicDev, m_vecMonsterCount[PACER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);

				break;
			}
			case LEAPER:
			{
				pGameObject = CLeaper::Create(m_pGraphicDev, m_vecMonsterCount[LEAPER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

				break;
			}
			case SQUIRT:
			{
				pGameObject = CSquirt::Create(m_pGraphicDev, m_vecMonsterCount[SQUIRT]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Squirt", pGameObject), E_FAIL);

				++m_vecMonsterCount[SQUIRT];

				break;
			}
			case DIP:
			{
				pGameObject = CDip::Create(m_pGraphicDev, m_vecMonsterCount[DIP]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);

				break;
			}
			case DOPLE:
			{
				pGameObject = CDople::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dople", pGameObject), E_FAIL);

				m_vecMonsterCount[DOPLE]++;

				break;
			}
			case CHARGER:
			{
				pGameObject = CCharger::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Charger", pGameObject), E_FAIL);

				++m_vecMonsterCount[CHARGER];

				break;
			}
			}
			break;
		}
		case BOSS:
		{
			switch (iter.iIndex)
			{
			case MONSTRO:
			{
				pGameObject = CMonstro::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monstro", pGameObject), E_FAIL);

				break;
			}
			case MOM:
			{
				pGameObject = CMom::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Mom", pGameObject), E_FAIL);

				break;
			}
			case MOM_PARTS:
			{
				for (int i = 0; i < 4; ++i)
				{
					pGameObject = CMomParts::Create(m_pGraphicDev, i);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.iX;
					dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.iZ;
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CMomParts*>(pGameObject)->Setting_Value();
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MomParts", pGameObject), E_FAIL);
				}
				break;
			}
			}
			break;
		}

		}
	}

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_GameItem(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer;

	if (m_bStartScene)
	{
		pLayer = Engine::CLayer::Create();
		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		for (auto& iter : mapLoadObj)
		{
			switch (iter.second.iType)
			{
			case ITEM:
			{
				switch (iter.second.iIndex)
				{
				case BRIM:
				{
					_vec3 vPos = { iter.second.iX, iter.second.iY, iter.second.iZ };
					_vec3 vDir = { 0, 0, 1 };

					pGameObject = CBrimStone::Create(m_pGraphicDev, 0, vPos, vDir);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BrimStone", pGameObject), E_FAIL);
					break;
				}
				case EPIC:
				{
					_vec3 vPos = { iter.second.iX, iter.second.iY, iter.second.iZ };
					_vec3 vDir = { 0, 0, 1 };

					pGameObject = CEpic::Create(m_pGraphicDev, 0, vPos, vDir);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					pGameObject->Set_MyLayer(pLayerTag);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Epic", pGameObject), E_FAIL);

					++m_vecMonsterCount[ATTACK_FLY];

					break;
				}
				}

			}
			}
		}

	}
	else
	{
		auto	pLayerFind = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

		if (pLayerFind == m_mapLayer.end())
		{
			pLayer = Engine::CLayer::Create();
		}
		else
		{
			pLayer = (*pLayerFind).second;
		}

		NULL_CHECK_RETURN(pLayer, E_FAIL);

		Engine::CGameObject* pGameObject = nullptr;

		auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;

		auto iter = mapLoadObj.at(m_iCreatedCnt);

		switch (iter.iType)
		{
		case ITEM:
		{
			switch (iter.iIndex)
			{
			case BRIM:
			{
				_vec3 vPos = { iter.iX, iter.iY, iter.iZ };
				_vec3 vDir = { 0, 0, 1 };

				pGameObject = CBrimStone::Create(m_pGraphicDev, 0, vPos, vDir);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BrimStone", pGameObject), E_FAIL);
				break;
			}
			case EPIC:
			{
				_vec3 vPos = { iter.iX, iter.iY, iter.iZ };
				_vec3 vDir = { 0, 0, 1 };

				pGameObject = CEpic::Create(m_pGraphicDev, 0, vPos, vDir);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Epic", pGameObject), E_FAIL);

				++m_vecMonsterCount[ATTACK_FLY];

				break;
			}
			}

		}
		}
	}

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_Door(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	_vec3 vTempPos;

	auto vecDoorTheme = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_vecConnectRoom;

	int i = 0;
	for (auto& iter : vecDoorTheme)
	{
		if (1 > iter)
		{
			++i;
			continue;
		}

		switch (i)
		{
		case WALL_LEFT:
			pGameObject = CDoor::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->Set_MyLayer(pLayerTag);

			dynamic_cast<CDoor*>(pGameObject)->Set_Theme(CStageLoadMgr::GetInstance()->Get_Door_TextureName(iter));

			vTempPos = m_pLeftWall->Get_Transform()->m_vInfo[INFO_POS];

			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->Set_Pos(vTempPos.x + DOOR_X_INTERVAL, DOOR_Y_INTERVAL, vTempPos.z);
			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->m_vAngle = m_pLeftWall->Get_Transform()->m_vAngle;
			dynamic_cast<CDoor*>(pGameObject)->Set_Stage_Num_Key(iter);
			dynamic_cast<CDoor*>(pGameObject)->Set_DoorPos(0);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);
			break;
		case WALL_RIGHT:
			pGameObject = CDoor::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->Set_MyLayer(pLayerTag);

			dynamic_cast<CDoor*>(pGameObject)->Set_Theme(CStageLoadMgr::GetInstance()->Get_Door_TextureName(iter));

			vTempPos = m_pRightWall->Get_Transform()->m_vInfo[INFO_POS];

			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->Set_Pos(vTempPos.x - DOOR_X_INTERVAL, DOOR_Y_INTERVAL, vTempPos.z);
			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->m_vAngle = m_pRightWall->Get_Transform()->m_vAngle;
			dynamic_cast<CDoor*>(pGameObject)->Set_Stage_Num_Key(iter);
			dynamic_cast<CDoor*>(pGameObject)->Set_DoorPos(1);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);
			break;
		case WALL_TOP:
			pGameObject = CDoor::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->Set_MyLayer(pLayerTag);

			dynamic_cast<CDoor*>(pGameObject)->Set_Theme(CStageLoadMgr::GetInstance()->Get_Door_TextureName(iter));

			vTempPos = m_pTopWall->Get_Transform()->m_vInfo[INFO_POS];

			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->Set_Pos(vTempPos.x, DOOR_Y_INTERVAL, vTempPos.z - DOOR_X_INTERVAL);
			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->m_vAngle = m_pTopWall->Get_Transform()->m_vAngle;
			dynamic_cast<CDoor*>(pGameObject)->Set_Stage_Num_Key(iter);
			dynamic_cast<CDoor*>(pGameObject)->Set_DoorPos(2);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);
			break;
		case WALL_BOTTOM:
			pGameObject = CDoor::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pGameObject, E_FAIL);
			pGameObject->Set_MyLayer(pLayerTag);

			dynamic_cast<CDoor*>(pGameObject)->Set_Theme(CStageLoadMgr::GetInstance()->Get_Door_TextureName(iter));

			vTempPos = m_pBottomWall->Get_Transform()->m_vInfo[INFO_POS];

			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->Set_Pos(vTempPos.x, DOOR_Y_INTERVAL, vTempPos.z + DOOR_X_INTERVAL);
			dynamic_cast<CDoor*>(pGameObject)->Get_TransformCom()->m_vAngle = m_pBottomWall->Get_Transform()->m_vAngle;
			dynamic_cast<CDoor*>(pGameObject)->Set_Stage_Num_Key(iter);
			dynamic_cast<CDoor*>(pGameObject)->Set_DoorPos(3);
			FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Door", pGameObject), E_FAIL);
			break;
		}

		++i;
	}

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_Environment(const _tchar* pLayerTag)
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
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	// 플레이어에 카메라 설정
	CPlayer::GetInstance()->Set_Camera(pGameObject);
	CPlayer::GetInstance()->Ready_GameObject(m_pGraphicDev);

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_RoomObject(const _tchar* pLayerTag)
{
	CUBE_ACTION_TYPE eAction = (CUBE_ACTION_TYPE)(rand() % ACTION_END);

	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	wstring wstrProto = L"Proto_";
	wstring wstrTag, wstrTheme;

	string strType = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_strType;

	wstrTheme.assign(strType.begin(), strType.end());

	//바닥 추가
	wstrTag = wstrProto + wstrTheme + L"FloorCubeTexture";
	pGameObject = m_pFloor = CFloor::Create(m_pGraphicDev, eAction, m_bStartScene);
	m_pFloor->Set_Cube_Texture_Tag(wstrTag.c_str());
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Floor", pGameObject), E_FAIL);

	//벽 추가
	// 여기는 벽의 큐브 텍스처의 태그를 만들어서 넘겨주는 부분
	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pLeftWall = CWall::Create(m_pGraphicDev, eAction, m_bStartScene);
	m_pLeftWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_LEFT);

	// 여기는 벽면 텍스처의 태그를 만들어서 넘겨주는 부분
	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pLeftWall->Set_Texture_Tag(wstrTag.c_str(), WALL_LEFT);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	//반복
	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pRightWall = CWall::Create(m_pGraphicDev, eAction, m_bStartScene);
	m_pRightWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_RIGHT);

	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pRightWall->Set_Texture_Tag(wstrTag.c_str(), WALL_RIGHT);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pTopWall = CWall::Create(m_pGraphicDev, eAction, m_bStartScene);
	m_pTopWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_TOP);

	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pTopWall->Set_Texture_Tag(wstrTag.c_str(), WALL_TOP);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pBottomWall = CWall::Create(m_pGraphicDev, eAction, m_bStartScene);
	m_pBottomWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_BOTTOM);

	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pBottomWall->Set_Texture_Tag(wstrTag.c_str(), WALL_BOTTOM);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_UI(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	/*pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);*/

	// Menu
	pGameObject = CMenu::Create(m_pGraphicDev, WINCX, WINCY, 0.f, 0.f, 1, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Menu", pGameObject), E_FAIL);

	// Player Coin UI
	pGameObject = CPlayerCoin::Create(m_pGraphicDev, 28.f, 28.f, -350.f, 180.f, 1, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CoinUI", pGameObject), E_FAIL);

	// Player Coin Font UI
	for (int i = 0; i < 2; ++i)
	{
		pGameObject = CPLCoinFont::Create(m_pGraphicDev, 60.f, 60.f, -310 + (i * 20.f), 180.f, 1, 1);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		dynamic_cast<CPLCoinFont*>(pGameObject)->Set_Index(i);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CoinFontUI", pGameObject), E_FAIL);
	}

	// Player HP
	pGameObject = CPlayerHP::Create(m_pGraphicDev, 30.f, 30.f, -370.f, 170.f, 1, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PlayerHP", pGameObject), E_FAIL);


	// MiniMap
	pGameObject = CMiniMap::Create(m_pGraphicDev, 140, 140, 330.f, 230.f, 1, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniMap", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

bool CLoadStage::Check_Cube_Arrived()
{
	return m_pLeftWall->Get_Arrived() && m_pRightWall->Get_Arrived()
		&& m_pTopWall->Get_Arrived() && m_pBottomWall->Get_Arrived()
		&& m_pFloor->Get_Arrived();

}

void CLoadStage::Copy_Stage()
{
	auto& map = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_mapLoadObj;
	CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_bClear = true;

	int iStageMonsterCount = 0;
	for (auto& iter : m_vecMonsterCount) iStageMonsterCount += iter;

	if (iStageMonsterCount)
	{
		for (auto iter = map.begin(); iter != map.end();)
		{
			if (MONSTER == (*iter).second.iType)
				iter = map.erase(iter);
			else
				iter++;
		}
	}

}

bool CLoadStage::Check_Monster_Dead()
{
	multimap<const _tchar*, CGameObject*> map = m_mapLayer.at(L"GameMst")->Get_ObjectMap();

	for (auto& iter : map)
	{
		if (!dynamic_cast<CMonster*>(iter.second)->Get_Dead())
			return false;
	}

	return true;
}

void CLoadStage::Check_All_Dead()
{
	int iCount = 0;

	for (auto& iter : m_vecMonsterCount)
		iCount += iter;

	if (Check_Monster_Dead())
	{
		for (auto& iter : m_mapLayer.at(L"GameDoor")->Get_ObjectMap())
			dynamic_cast<CDoor*>(iter.second)->Set_Open();
	}
}

void CLoadStage::Item_Collision()
{
	// 충돌처리하는 함수
	CGameObject* pObj = m_mapLayer.at(L"GameItem")->Collision_GameObject(CPlayer::GetInstance());

	if (pObj)
	{
		//충돌됨
		if (dynamic_cast<CItem*>(pObj)->Run_Item_Effect())
		{
			// 아이템 습득 UI
			ITEM_TYPE temp = dynamic_cast<CItem*>(pObj)->Get_Item_Type();
			if (PILL == temp || BRIM == temp || EPIC == temp || SAD_ONION == temp || TRINKET == temp)
			{
				dynamic_cast<CItemFontUI*>(m_mapLayer.at(L"UI")->Get_GameObject(L"ItemFontUI"))->Set_Render();
			}
			dynamic_cast<CItemFontUI*>(m_mapLayer.at(L"UI")->Get_GameObject(L"ItemFontUI"))
				->Set_ItemType(dynamic_cast<CItem*>(pObj)->Get_Item_Type());

			if (PILL == temp) // 알약인 경우
			{
				dynamic_cast<CItemFontUI*>(m_mapLayer.at(L"UI")->Get_GameObject(L"ItemFontUI"))->Set_PillState(
					dynamic_cast<CPill*>(pObj)->Get_Pill_Num());
			}
		}
	}
}


void CLoadStage::Moster_Collision()
{
	// Monster <-> Bullet 충돌
	if (m_mapLayer.at(L"GameMst") != nullptr)
	{
		list<CGameObject*>* pBulletList = CPlayer::GetInstance()->Get_Player_BullletList();

		for (list<CGameObject*>::iterator iter = pBulletList->begin();
			iter != pBulletList->end();)
		{
			CGameObject* pMonster = m_mapLayer.at(L"GameMst")->Collision_GameObject(*iter);

			if (pMonster)
			{
				// 일반 총알일때 이펙트 보여주려고 해당부분 처리
				if (CPlayer::GetInstance()->Get_PlayerBulletState() == 0 && // 일반 Bullet
					ATTACK_FLY != dynamic_cast<CMonster*>(pMonster)->Get_MstType() && // 도플, 공격형 파리가 아닌 경우
					DOPLE != dynamic_cast<CMonster*>(pMonster)->Get_MstType())
				{
					if (dynamic_cast<CPlayerBullet*>(*iter)->Get_BulletState() &&		// Bullet이 Dead가 아닌 경우
						!dynamic_cast<CMonster*>(pMonster)->Get_Dead())					// Monster가 Dead가 아닌 경우
					{
						// 일반 총알 충돌처리
						dynamic_cast<CPlayerBullet*>(*iter)->Set_BulletCollision();

						if (dynamic_cast<CMonster*>(pMonster)->Get_IsBoss()) // 보스인 경우
						{
							if (MONSTRO == dynamic_cast<CMonster*>(pMonster)->Get_BossType())
							{
								dynamic_cast<CMonstro*>(pMonster)->Hit();
								break;
							}
							else
								++iter;
						}
						else // 일반 몬스터의 경우 전부 피격 처리 O
						{
							dynamic_cast<CMonster*>(pMonster)->Hit();

							// Squirt 인 경우 Dip 두 마리 생성 
							if (SQUIRT == dynamic_cast<CMonster*>(pMonster)->Get_MstType()) // Squirt인 경우
							{
								if (1 >= dynamic_cast<CMonster*>(pMonster)->Get_HP())
								{
									dynamic_cast<CSquirt*>(pMonster)->Create_Dip(m_mapLayer.at(L"GameMst"));
								}
							}
							break;
						}
					}
					else
						++iter;
				}
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 1)
				{
					if (!dynamic_cast<CMonster*>(pMonster)->Get_Dead())
					{
						dynamic_cast<CMonster*>(pMonster)->Hit();
						// Squirt 인 경우 Dip 두 마리 생성 
						if (SQUIRT == dynamic_cast<CMonster*>(pMonster)->Get_MstType()) // Squirt인 경우
						{
							if (1 >= dynamic_cast<CMonster*>(pMonster)->Get_HP())
							{
								dynamic_cast<CSquirt*>(pMonster)->Create_Dip(m_mapLayer.at(L"GameMst"));
							}
						}
						break;
					}
					else
						++iter;
				}
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 2) // 에픽페투스
				{
					if (dynamic_cast<CEpicBullet*>(*iter)->Get_CanAttacked()) // true일 때 공격
					{
						// 공격 한번 하면 true해서 연속으로 딜 안들어가게
						dynamic_cast<CEpicBullet*>(*iter)->Set_CanAttack();

						if (FLY == dynamic_cast<CMonster*>(pMonster)->Get_MstType() || // Epic과 충돌이 이루어지는 몬스터들
							DIP == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							SQUIRT == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							PACER == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							LEAPER == dynamic_cast<CMonster*>(pMonster)->Get_MstType())
						{
							dynamic_cast<CMonster*>(pMonster)->Set_Dead();

							// Squirt 인 경우 Dip 두 마리 생성 
							if (SQUIRT == dynamic_cast<CMonster*>(pMonster)->Get_MstType()) // Squirt인 경우
							{
								dynamic_cast<CSquirt*>(pMonster)->Create_Dip(m_mapLayer.at(L"GameMst"));
							}
							break;
						}
						else
							++iter;
					}
					else
						++iter;
				}
				else
					++iter;
			}
			else
				++iter;
		}
	}

	// Dople <-> Spike 충돌
	if (Get_GameObject(L"MapObj", L"Spike") != nullptr && Get_GameObject(L"GameMst", L"Dople") != nullptr) // Dople과 Spike가 생성된 경우
	{
		for (auto& iter : m_mapLayer.at(L"MapObj")->Get_ObjectMap())
		{
			if (SPIKE == dynamic_cast<CMapObj*>(iter.second)->Get_Type()) // Spike의 경우
			{
				//CGameObject* pMonster = m_mapLayer.at(L"GameMst")->Collision_GameObject(*iter);
				CGameObject* pMonster = m_mapLayer.at(L"GameMst")->Collision_GameObject(iter.second); ////////////////////////

				if (pMonster) // 충돌한 몬스터가 존재하고
				{
					if (DOPLE == dynamic_cast<CMonster*>(pMonster)->Get_MstType()) // Dople인 경우
					{
						dynamic_cast<CDople*>(pMonster)->Hit();
					}
				}
			}
		}
	}
}

void CLoadStage::MapObj_Collision()
{
	// MapObj <-> PlayerBullet 충돌
	if (m_mapLayer.at(L"MapObj") != nullptr)
	{
		list<CGameObject*>* pBulletList = CPlayer::GetInstance()->Get_Player_BullletList();

		for (list<CGameObject*>::iterator iter = pBulletList->begin();
			iter != pBulletList->end();)
		{
			CGameObject* pMapObj = m_mapLayer.at(L"MapObj")->Collision_GameObject(*iter);

			if (pMapObj)
			{
				if (CPlayer::GetInstance()->Get_PlayerBulletState() == 0) // 일반 Bullet
				{
					if (dynamic_cast<CPlayerBullet*>(*iter)->Get_BulletState() && // Bullet이 Dead가 아닐 때
						!dynamic_cast<CMapObj*>(pMapObj)->Get_Dead())			// MapObj도 Dead가 아닐 때
					{
						// 일반 총알일때 이펙트 보여주려고 해당부분 처리
						if (CPlayer::GetInstance()->Get_PlayerBulletState() == 0)
						{
							// 일반 총알 충돌처리
							dynamic_cast<CPlayerBullet*>(*iter)->Set_BulletCollision();
						}

						if (POOP == dynamic_cast<CMapObj*>(pMapObj)->Get_Type())
						{
							dynamic_cast<CMapObj*>(pMapObj)->Set_Hit();
							break;
						}
						else if (0 == dynamic_cast<CMapObj*>(pMapObj)->Get_ObjID())
						{
							dynamic_cast<CFire*>(pMapObj)->Set_Hit();
							break;
						}
						else
						{
							break;
						}
					}
					++iter;
				}
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 2)// && // 에픽페투스와의 충돌
				{
					if (dynamic_cast<CEpicBullet*>(*iter)->Get_CanAttacked()) // Epic이 로켓 상태일 때(공격 가능)
					{
						dynamic_cast<CMapObj*>(pMapObj)->Set_Hit();
						break;
					}
					else
						++iter;
				}
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 1) // 혈사포
				{
					if (!dynamic_cast<CMapObj*>(pMapObj)->Get_Dead())			// MapObj가 Dead가 아닐 때
					{
						if (POOP == dynamic_cast<CMapObj*>(pMapObj)->Get_Type())
						{
							dynamic_cast<CMapObj*>(pMapObj)->Set_Hit();
							break;
						}
						else if (0 == dynamic_cast<CMapObj*>(pMapObj)->Get_ObjID())
						{
							dynamic_cast<CFire*>(pMapObj)->Set_Hit();
							break;
						}
						else
						{
							break;
						}
					}
					++iter;
				}
			}
			else
				++iter;
		}
	}

	// SlotMC <-> Player 의 충돌
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC") != nullptr)
	{
		CGameObject* pMachine = m_mapLayer.at(L"MapObj")->Collision_GameObject(CPlayer::GetInstance());

		if (pMachine)
		{
			if (1 == dynamic_cast<CMapObj*>(pMachine)->Get_ObjID() &&
				!dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Get_CoolTime()) // 쿨타임 돌지 않을때
			{
				if (0 < CPlayer::GetInstance()->Get_Coin())
				{
					CPlayer::GetInstance()->Set_Coin(-1);
					dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Set_Game();
				}
			}
		}
	}

	// 야바위 충돌
	if (Get_GameObject(L"MapObj", L"ShellGame") != nullptr) // Shell Game이 생성되었을 때
	{
		if (!dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellVec().empty())	// Shell이 있을 때
		{
			CGameObject* pShellObj = m_mapLayer.at(L"MapObj")->Collision_GameObject(CPlayer::GetInstance()); // Player와 충돌한 객체

			if (pShellObj) // Player와 충돌한 객체가 존재할 때
			{
				if (3 == dynamic_cast<CMapObj*>(pShellObj)->Get_ObjID()) // 충돌 대상이 Shell일때
				{
					if (!dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Get_Game() && // 게임 중이 아닐 때
						!dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Get_CheckCoolTime())
					{
						if (0 < CPlayer::GetInstance()->Get_Coin())
						{
							CPlayer::GetInstance()->Set_Coin(-1);
							dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Set_Game(true); // Game True
						}
					}
					else if(dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Get_Game() && // 게임 중이고 
							dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Get_Game_Reward())// Shell Game이 보상 여부가 True인 상태일 때
					{
						dynamic_cast<CShell*>(pShellObj)->Set_StartUp(); // 선택한 Shell 위로 오픈

						if (dynamic_cast<CShell*>(pShellObj)->Get_Reward()) // 당첨이면 (m_bReward = true)
						{
							Engine::CGameObject* pGameObject = nullptr;

							ITEM_TYPE eType = dynamic_cast<CShell*>(pShellObj)->Get_ItemType();
							wstring wstrObjTag = dynamic_cast<CShell*>(pShellObj)->Get_DropItemTag();

							for (int i = 0; i < 3; ++i)
							{
								pGameObject = dynamic_cast<CShell*>(pShellObj)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), i);
								m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);
							}
							dynamic_cast<CShell*>(pShellObj)->Set_Reward(false); // 보상을 줬으니 m_bReward 상태 False로 변환
						}
						else if (dynamic_cast<CShell*>(pShellObj)->Get_Lose()) // 꽝이면 (m_bReward = false)
						{
							Engine::CGameObject* pFly = nullptr;

							_vec3 vPos;
							dynamic_cast<CShell*>(pShellObj)->Get_TransformCom()->Get_Info(INFO_POS, &vPos);

							for (int i = 0; i < 2; ++i) // 파리 두 마리 생성
							{
								pFly = CFly::Create(m_pGraphicDev, i * 2);
								dynamic_cast<CFly*>(pFly)->Get_Transform()->Set_Pos(vPos);
								pFly->Set_MyLayer(L"GameMst");
								m_mapLayer.at(L"GameMst")->Add_GameObject(L"Fly", pFly);
							}
						}

						dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Set_Game(false); // 게임을 False로
						dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Set_Game_Reward(false); // 게임 보상 여부도 False로
						dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))->Set_CheckCoolTime(); // 체크 쿨타임 true로 만듦
					}
				}
			}
		}
	}
}


void CLoadStage::Obstacle_Collsion()
{
	CTransform* pPlayerTrans = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player_Transform());

	if (m_mapLayer.at(L"MapObj") != nullptr)
	{
		auto& mapObj = m_mapLayer.at(L"MapObj")->Get_ObjectMap();

		for (auto& iter : mapObj)
		{
			if (OBSTACLE <= dynamic_cast<CMapObj*>(iter.second)->Get_Type() && dynamic_cast<CMapObj*>(iter.second)->Get_Type() <= OBSTACLE_Z)
			{
				CTransform* pTrans = dynamic_cast<CTransform*>(iter.second->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
				Engine::Check_Collision(pPlayerTrans, pTrans);
			}
		}

		for (auto& iter : mapObj)
		{
			if (OBSTACLE_X <= dynamic_cast<CMapObj*>(iter.second)->Get_Type() && dynamic_cast<CMapObj*>(iter.second)->Get_Type() <= OBSTACLE_Z)
			{
				for (auto& iter2 : mapObj)
				{
					if (iter == iter2) continue;

					if (OBSTACLE == dynamic_cast<CMapObj*>(iter2.second)->Get_Type())
					{
						CTransform* pTrans = dynamic_cast<CTransform*>(iter.second->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
						CTransform* pTrans2 = dynamic_cast<CTransform*>(iter2.second->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

					
						if (Engine::Check_Intersect(pTrans, pTrans2))
						{
							//Engine::Check_Collision(pPlayerTrans, pTrans);

							dynamic_cast<CMoveZObstacle*>(iter.second)->Set_Turn();
						}
					}
				}
			}
			
			
		}
	}

	// Dople <-> 장애물 충돌
	if (m_mapLayer.at(L"GameMst") != nullptr)
	{
		if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"Dople") != nullptr)
		{
			CTransform* pDopleTrans = dynamic_cast<CTransform*>(dynamic_cast<CDople*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"Dople"))->Get_Transform());
			
			if (m_mapLayer.at(L"MapObj") != nullptr)
			{
				auto& mapObj = m_mapLayer.at(L"MapObj")->Get_ObjectMap();

				for (auto& iter : mapObj)
				{
					if (OBSTACLE <= dynamic_cast<CMapObj*>(iter.second)->Get_Type() && dynamic_cast<CMapObj*>(iter.second)->Get_Type() <= OBSTACLE_Z)
					{
						CTransform* pTrans = dynamic_cast<CTransform*>(iter.second->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
						Engine::Check_Collision(pDopleTrans, pTrans);
					}
				}
			}
		}
	}

}

void CLoadStage::Player_Collision_With_Monster()
{
	// 충돌처리하는 함수
	CGameObject* pObj = m_mapLayer.at(L"GameMst")->Collision_GameObject(CPlayer::GetInstance());

	if (pObj && !dynamic_cast<CMonster*>(pObj)->Get_Dead())
	{
		// 플레이어 피 감소
		CPlayer::GetInstance()->Set_Attacked();
	}


	if (m_mapLayer.at(L"MapObj") != nullptr)
	{
		auto& mapObj = m_mapLayer.at(L"MapObj")->Get_ObjectMap();

		for (auto& iter : mapObj)
		{
			if (CAMPFIRE == dynamic_cast<CMapObj*>(iter.second)->Get_Type() && !dynamic_cast<CMapObj*>(iter.second)->Get_Dead())
			{
				CTransform* pTrans = dynamic_cast<CTransform*>(iter.second->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
				CTransform* pPlayerTrans = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component(ID_DYNAMIC, L"Proto_Transform"));
				if (Engine::Check_Intersect(pTrans, pPlayerTrans))
				{
					// 플레이어 피 감소
					CPlayer::GetInstance()->Set_Attacked();
				}
			}
		}
	}

}
void CLoadStage::Drop_ITem()
{
	// 똥
	if (Get_GameObject(L"MapObj", L"Poop") != nullptr)
	{
		if (dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Get_Dead() &&
			!dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Get_Drop())
		{
			Engine::CGameObject* pGameObject = nullptr;

			ITEM_TYPE eType = dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Get_ItemType();
			wstring wstrObjTag = dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Get_DropItemTag();

			pGameObject = dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Create_Item(eType, 2, m_mapLayer.at(L"GameItem"), 0);
			m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);

			dynamic_cast<CPoop*>(Get_GameObject(L"MapObj", L"Poop"))->Set_Drop();
		}
	}

	// 모닥불
	if (Get_GameObject(L"MapObj", L"Fire") != nullptr)
	{
		if (dynamic_cast<CMapObj*>(Get_GameObject(L"MapObj", L"Fire"))->Get_Dead() &&
			!dynamic_cast<CMapObj*>(Get_GameObject(L"MapObj", L"Fire"))->Get_Drop())
		{
			Engine::CGameObject* pGameObject = nullptr;

			ITEM_TYPE eType = dynamic_cast<CFire*>(Get_GameObject(L"MapObj", L"Fire"))->Get_ItemType();
			wstring wstrObjTag = dynamic_cast<CFire*>(Get_GameObject(L"MapObj", L"Fire"))->Get_DropItemTag();

			pGameObject = dynamic_cast<CFire*>(Get_GameObject(L"MapObj", L"Fire"))->Create_Item(eType, 2, m_mapLayer.at(L"GameItem"), 0);
			m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);

			dynamic_cast<CFire*>(Get_GameObject(L"MapObj", L"Fire"))->Set_Drop();
		}
	}

	// 슬롯머신
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC") != nullptr)
	{
		if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Machine") != nullptr)
		{
			if (dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))->Get_Reward())//&& // SlotMC가 보상 true일 때
				//!dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))->Get_Drop()) // SlotMC가 보상을 Drop하지 않았을 때(false)
			{
				CGameObject* pSlotMC = nullptr;
				CGameObject* pDropItem = nullptr;
				pSlotMC = m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC");

				ITEM_TYPE eType = dynamic_cast<CSlotMC*>(pSlotMC)->Get_ItemType(); // 랜덤 결과로부터 Drop 아이템 세팅
				wstring wstrObjTag = dynamic_cast<CSlotMC*>(pSlotMC)->Get_DropItemTag();

				if (HEART == eType)
				{
					pDropItem = dynamic_cast<CMapObj*>(pSlotMC)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), 1);
					m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pDropItem);
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Drop(); // 아이템을 드랍한 녀석임을 표시
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Reward(); // -> false로 변환
				}
				else if (COIN == eType)
				{
					for (int i = 0; i < 3; ++i)
					{
						pDropItem = dynamic_cast<CMapObj*>(pSlotMC)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), i);
						m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pDropItem);
					}
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Drop();
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Reward();
				}
			}
		}
	}
}

void CLoadStage::Insert_Child()
{
	// Normal Fly 추가
	if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"AttackFly") != nullptr &&
		m_mapLayer.at(L"GameMst")->Get_GameObject(L"NormalFly") == nullptr)
	{
		dynamic_cast<CAttackFly*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"AttackFly"))
			->Set_NormalFly_ToStage(m_mapLayer.at(L"GameMst"));
	}

	// Fire (Campfire) 추가 // (auto& iter : m_mapLayer.at(L"GameMst")->Get_ObjectMap()
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Fire") == nullptr)
	{
		for (auto& iter : m_mapLayer.at(L"MapObj")->Get_ObjectMap()) // 맵에 저장된 campfire 전부 순회하며 child 를 추가
		{
			if (CAMPFIRE == dynamic_cast<CMapObj*>(iter.second)->Get_Type())
			{
				if (dynamic_cast<CCampFire*>(iter.second)->Get_Fire() != nullptr)
				{
					dynamic_cast<CCampFire*>(iter.second)->Set_Fire_ToStage(m_mapLayer.at(L"MapObj")); // 한번만 추가할 방법?
				}
			}
		}
	}

	// SlotMC
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC") != nullptr &&
		m_mapLayer.at(L"MapObj")->Get_GameObject(L"Machine") == nullptr)
	{
		dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))
			->Set_Machine_ToStage(m_mapLayer.at(L"MapObj"));
	}

	// Shop
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Shop") != nullptr)
	{
		dynamic_cast<CShop*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"Shop"))->Set_Item_ToStage(m_mapLayer.at(L"GameItem"));
	}

	// Shell Game 추가
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame") != nullptr &&
		m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellNpc") == nullptr &&
		m_mapLayer.at(L"MapObj")->Get_GameObject(L"Shell") == nullptr)
	{
		dynamic_cast<CShellGame*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"ShellGame"))
			->Set_ShellObj_ToStage(m_mapLayer.at(L"MapObj"));
	}

	for (auto& iter : m_mapLayer.at(L"GameMst")->Get_ObjectMap()) // MomParts에 Mom 설정
	{
		if (MOM_PARTS == dynamic_cast<CMonster*>(iter.second)->Get_BossType())
		{
			dynamic_cast<CMomParts*>(iter.second)->Set_Mom(dynamic_cast<CMom*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"Mom")));
		}
	}
}

void CLoadStage::Setting_UI()
{
	// Monstro HP
	if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"Monstro") != nullptr &&
		m_mapLayer.at(L"UI")->Get_GameObject(L"BossHPTool") == nullptr)
	{
		dynamic_cast<CMonstro*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"Monstro"))->Print_UI(m_mapLayer.at(L"UI"));
	}

	// Mom HP
	if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"Mom") != nullptr &&
		m_mapLayer.at(L"UI")->Get_GameObject(L"MomHPTool") == nullptr)
	{
		dynamic_cast<CMom*>(m_mapLayer.at(L"GameMst")->Get_GameObject(L"Mom"))->Print_UI(m_mapLayer.at(L"UI"));
	}

	// 아이템 설명 UI
	CItemFontUI* pItemFontUI = CItemFontUI::Create(m_pGraphicDev, 620, 100, 0.f, 180.f, 1, 1);
	m_mapLayer.at(L"UI")->Add_GameObject(L"ItemFontUI", pItemFontUI);
}

void CLoadStage::Update_MiniMap()
{
	// MiniMap이 있다면 매번 업데이트
	if (m_mapLayer.at(L"UI"))
	{
		if (m_mapLayer.at(L"UI")->Get_GameObject(L"MiniMap"))
		{
			// m_iCurStageKey
			dynamic_cast<CMiniMap*>(m_mapLayer.at(L"UI")->Get_GameObject(L"MiniMap"))->Set_NowRoom(m_iCurStageKey);

		}
	}
}

void CLoadStage::Link_MomParts_ToLayer()
{
	if (m_mapLayer.at(L"GameMst")->Get_GameObject(L"MomParts") != nullptr)
	{
		for (auto& iter : m_mapLayer.at(L"GameMst")->Get_ObjectMap())
		{
			if (MOM_PARTS == dynamic_cast<CMonster*>(iter.second)->Get_BossType())
			{
				dynamic_cast<CMomParts*>(iter.second)->Set_Layer(m_mapLayer.at(L"GameMst"));
			}
		}
	}
}

void CLoadStage::Play_Ending(const _float& fTimeDelta)
{
	m_fEndingTimer -= fTimeDelta;

	if (0 < m_fEndingTimer)
	{
		CTransform* pTest = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player_Transform());

		_matrix mat = *(pTest->Get_WorldMatrix());
		mat._41 = mat._41 + (rand() % 10 - 5);
		mat._42 = mat._42 + (rand() % 10 - 5);
		mat._43 = mat._43 + (rand() % 10 - 5);

		Engine::Create_Dust(m_pGraphicDev, mat);
	}
	else
	{
		//Engine::Kill_Scatter();
		Engine::Set_Ending();

		Engine::CScene* pScene = nullptr;
		pScene = CEnding::Create(m_pGraphicDev);
		Engine::Set_Scene(pScene);

		return;
	}

}

void CLoadStage::Create_Map_Particles()
{
	CTransform* pTest = dynamic_cast<CTransform*>(m_pTopWall->Get_Transform());

	_matrix* mat = pTest->Get_WorldMatrix();

	Engine::Create_Splash_Forward(m_pGraphicDev, *mat);

}

void CLoadStage::BGM_INTRO_START()
{
	// 첫번째 방 인트로 bgm은 dynamic camera에서 시작
	if (m_bBGMIntro)
	{
		StageInfo info = CStageLoadMgr::GetInstance()->Get_StageInfo(m_iCurStageKey);
		string roomtype = info.m_strTheme;

		// 울고 있으면 비지엠으로 안들어감
		if (roomtype == "Normal" && !CPlayer::GetInstance()->Get_Cry_Anim())
		{
			/*Engine::StopSound(SOUND_BGM);
			if (Engine::PlayEffect(L"diptera sonata intro.ogg", SOUND_BGM_INTRO, 0.8f))
			{
				m_bBGMIntro = false;
			}*/
			Engine::StopSound(SOUND_BGM);
			Engine::PlayEffect(L"diptera sonata intro.ogg", SOUND_BGM_INTRO, 0.8f);
		}
		else if (roomtype == "Treasure")
		{
			Engine::StopSound(SOUND_BGM);
			Engine::PlayEffect(L"TreasureRoom.ogg", SOUND_BGM_INTRO, 0.8f);
			m_bBGMIntro = false;
		}
		else if (roomtype == "Devil")
		{
			Engine::PlayEffect(L"DevilRoom.wav", SOUND_BGM_INTRO, 0.8f);
			m_bBGMIntro = false;
		}
		else if (roomtype == "Arcade")
		{
			Engine::StopSound(SOUND_BGM);
			Engine::PlayEffect(L"ArcadeRoom.ogg", SOUND_BGM_INTRO, 0.8f);
			m_bBGMIntro = false;
		}
		else if (roomtype == "Boss")
		{
			m_bBGMIntro = false;
			Engine::StopSound(SOUND_BGM);
			/*if (Engine::PlayEffect(L"boss fight intro jingle v2.1.ogg", SOUND_BGM, 0.8f))
			{
				m_bBGMIntro = false;
			}*/
		}
		
	}
}

void CLoadStage::BGM_START()
{
	/*if (!m_bBGMIntro && m_bBGM)
	{
		StageInfo info = CStageLoadMgr::GetInstance()->Get_StageInfo(m_iCurStageKey);
		string roomtype = info.m_strTheme;
		if (roomtype == "Normal")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::PlayBGM(L"diptera sonata(basement).ogg", 0.8f);
				m_bBGM = false;
			}
		}
		else if (roomtype == "Treasure")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::PlayBGM(L"diptera sonata(basement).ogg", 0.8f);
				m_bBGM = false;
			}
		}
		else if (roomtype == "Devil")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::PlayBGM(L"the calm.ogg", 0.8f);
				m_bBGM = false;
			}
				
		}
		else if (roomtype == "Arcade")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::PlayBGM(L"arcaderoom_loop.ogg", 0.8f);
				m_bBGM = false;
			}
				
			arcaderoom_loop.ogg
		}
		else if (roomtype == "Boss")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::PlayEffect(L"basic boss fight.ogg", SOUND_BGM, 0.8f);
				m_bBGM = false;
			}
			
		}
		
	}*/

	if (m_bBGM)
	{
		StageInfo info = CStageLoadMgr::GetInstance()->Get_StageInfo(m_iCurStageKey);
		string roomtype = info.m_strTheme;
		if (roomtype == "Normal")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO) && !CPlayer::GetInstance()->Get_Cry_Anim())
			{
				Engine::StopSound(SOUND_BGM);
				Engine::PlayBGM(L"diptera sonata(basement).ogg", 0.8f);
				m_bBGM = false;
			}
		}
		else if (roomtype == "Treasure")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::StopSound(SOUND_BGM);
				Engine::PlayBGM(L"diptera sonata(basement).ogg", 0.8f);
				m_bBGM = false;
			}
		}
		else if (roomtype == "Devil")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::StopSound(SOUND_BGM);
				Engine::PlayBGM(L"the calm.ogg", 0.8f);
				m_bBGM = false;
			}

		}
		else if (roomtype == "Arcade")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::StopSound(SOUND_BGM);
				Engine::PlayBGM(L"arcaderoom_loop.ogg", 0.8f);
				m_bBGM = false;
			}

			//arcaderoom_loop.ogg
		}
		else if (roomtype == "Boss")
		{
			if (!Engine::CheckIsPlaying(SOUND_BGM_INTRO))
			{
				Engine::StopSound(SOUND_BGM);
				Engine::PlayEffect(L"basic boss fight.ogg", SOUND_BGM, 0.8f);
				m_bBGM = false;
			}

		}

	}
}

HRESULT CLoadStage::Door_Collision()
{
	if (m_mapLayer.at(L"GameDoor"))
	{
		if (dynamic_cast<CDoor*>(m_mapLayer.at(L"GameDoor")->Get_GameObject(L"Door"))->Get_Open()) // 문이 열렸을 경우에만
		{
			CGameObject* pObj = m_mapLayer.at(L"GameDoor")->Collision_GameObject(CPlayer::GetInstance());

			if (pObj) // 충돌된 문 존재
			{
				if (m_iCurStageKey == 8)
				{
					// 도플방이었을때
					CPlayer::GetInstance()->Set_OffDople();
				}

				//_vec3 playerpos;
				//dynamic_cast<CDoor*>(pObj)->Get_TransformCom()->Get_Info(INFO_POS, &playerpos);
				//CPlayer::GetInstance()->Set_StartPosition(playerpos);

				_vec3 startpos, fullpos;
				int doornum = dynamic_cast<CDoor*>(pObj)->Get_DoorPos();
				/*dynamic_cast<CDoor*>(pObj)->Get_TransformCom()->Get_Info(INFO_POS, &startpos);*/


				if (doornum == 0)
				{
					//left
					startpos = _vec3(27.5, 0, 15.5);
				}
				else if (doornum == 1)
				{
					//right
					startpos = _vec3(3.5, 0, 15.5);
				}
				else if (doornum == 2)
				{
					//top
					startpos = _vec3(15.5, 0, 3.5);
				}
				else if (doornum == 3)
				{
					//bottom
					startpos = _vec3(15.5, 0, 27.5);
				}

				CPlayer::GetInstance()->Set_KeyBlock(true);

				// 스테이지 변경
				Engine::CScene* pScene = nullptr;

				int iStageKey = dynamic_cast<CDoor*>(pObj)->Get_Stage_Num_Key();
				string strTheme = CStageLoadMgr::GetInstance()->Get_StageInfo(iStageKey).m_strTheme;
				bool bClear = CStageLoadMgr::GetInstance()->Get_StageInfo(iStageKey).m_bClear;


				if (strTheme == "Boss")
				{
					// 보스 VS 씬으로 전환해줌
					pScene = CBossFight::Create(m_pGraphicDev, iStageKey);
				}
				else
				{
					// bClear : false면 처음 방문하는방, true면 이미 깬 방
					pScene = CLoadStage::Create(m_pGraphicDev, iStageKey, bClear);
				}


				if (!bClear && strTheme != "Boss")
				{
					CPlayer::GetInstance()->Set_IssacRender(false);
					// false : 처음 방문하는방
					CPlayer::GetInstance()->Set_Camera_Cinemachine_01();
					// 중간에 스폰
					startpos = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);
					CPlayer::GetInstance()->Set_StartPos(startpos);
				}
				else
				{
					CPlayer::GetInstance()->Set_StartPos(startpos);
				}


				NULL_CHECK_RETURN(pScene, -1);

				FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			}
		}
	}
}

CLoadStage* CLoadStage::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType, bool bStratScene)
{
	//Engine::StopSound(SOUND_BGM);
	CLoadStage* pInstance = new CLoadStage(pGraphicDev);
	pInstance->m_bStartScene = bStratScene;
	CPlayer::GetInstance()->Set_Bool_StartScene(true);

	if (!bStratScene)
	{
		//Engine::StopAll();
		//Engine::StopSound(SOUND_EFFECT_ETC_ALLPLAY);
		Engine::PlayEffect(L"earthquake4.wav", SOUND_BGM_EARTHQUAKE, 2.6f);
		// 큐브연출있으면 무적시간 두기
		CPlayer::GetInstance()->Set_MapCinemachine(true);
	}

	if (FAILED(pInstance->Ready_Scene(iType)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLoadStage::Free()
{
	__super::Free();
}
