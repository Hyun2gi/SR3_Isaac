#include "stdafx.h"
#include "..\Header\LoadStage.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "PlayerBullet.h"

#include "StageLoadMgr.h"

#include "Ending.h"

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

	CPlayer::GetInstance()->Ready_GameObject(m_pGraphicDev);
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
	}

	_int	iExit = __super::Update_Scene(fTimeDelta);

	CPlayer::GetInstance()->Update_GameObject(fTimeDelta);

	if (Check_Cube_Arrived() && !m_bIsCreated)
	{
		// m_bStartScene : 한번 왔다간 방인지 아닌지 확인해주는 변수
		if (!m_bStartScene)
		{
			// 한번 왔다간 방이 아닐경우처리
			CPlayer::GetInstance()->Set_IssacRender(true);
			// 올라간 시네머신이 진행 후 내려오는 시네머신 필요
			CPlayer::GetInstance()->Set_Camera_Cinemachine_02();
		}
		else
		{
			// 한번 왔다간 방임
			CPlayer::GetInstance()->Set_StartCameraMouse();
		}

		m_bIsCreated = true;
		FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"MapObj"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_GameMonster(L"GameMst"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_GameItem(L"GameItem"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);
		
		Setting_UI(); // UI 생성
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

	Engine::CLayer* pLayer = Engine::CLayer::Create();
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
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].x = iter.second.iX;
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS].z = iter.second.iZ;

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
}

HRESULT CLoadStage::Ready_Layer_GameMonster(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
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

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_GameItem(const _tchar* pLayerTag)
{
	// 아이템 관련
	Engine::CLayer* pLayer = Engine::CLayer::Create();
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
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	wstring wstrProto = L"Proto_";
	wstring wstrTag, wstrTheme;

	string strType = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_strType;

	wstrTheme.assign(strType.begin(), strType.end());

	//바닥 추가
	wstrTag = wstrProto + wstrTheme + L"FloorCubeTexture";
	pGameObject = m_pFloor = CFloor::Create(m_pGraphicDev, m_bStartScene);
	m_pFloor->Set_Cube_Texture_Tag(wstrTag.c_str());
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Floor", pGameObject), E_FAIL);


	//벽 추가
	// 여기는 벽의 큐브 텍스처의 태그를 만들어서 넘겨주는 부분
	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pLeftWall = CWall::Create(m_pGraphicDev, m_bStartScene);
	m_pLeftWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_LEFT);

	// 여기는 벽면 텍스처의 태그를 만들어서 넘겨주는 부분
	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pLeftWall->Set_Texture_Tag(wstrTag.c_str(), WALL_LEFT);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	//반복
	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pRightWall = CWall::Create(m_pGraphicDev, m_bStartScene);
	m_pRightWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_RIGHT);

	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pRightWall->Set_Texture_Tag(wstrTag.c_str(), WALL_RIGHT);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pTopWall = CWall::Create(m_pGraphicDev, m_bStartScene);
	m_pTopWall->Set_Cube_Texture_Tag(wstrTag.c_str(), WALL_TOP);

	wstrTag = wstrProto + wstrTheme + L"Wall";
	m_pTopWall->Set_Texture_Tag(wstrTag.c_str(), WALL_TOP);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wall", pGameObject), E_FAIL);

	wstrTag = wstrProto + wstrTheme + L"WallCubeTexture";
	pGameObject = m_pBottomWall = CWall::Create(m_pGraphicDev, m_bStartScene);
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
		dynamic_cast<CItem*>(pObj)->Run_Item_Effect();
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
					// 일반 총알 충돌처리
					dynamic_cast<CPlayerBullet*>(*iter)->Set_BulletCollision();

					if (dynamic_cast<CPlayerBullet*>(*iter)->Get_BulletState() &&		// Bullet이 Dead가 아닌 경우
						!dynamic_cast<CMonster*>(pMonster)->Get_Dead())					// Monster가 Dead가 아닌 경우
					{
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
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 1) // 혈사포 (엄마/엄마파츠만)
				{
					if (MOM_PARTS == dynamic_cast<CMonster*>(pMonster)->Get_BossType() || //MOM Parts이거나
						MOM == dynamic_cast<CMonster*>(pMonster)->Get_BossType())		// MOM인 경우
					{
						dynamic_cast<CMonster*>(pMonster)->Hit();
						break;
					}
					else
						++iter;
				}
				else if (CPlayer::GetInstance()->Get_PlayerBulletState() == 2) // 에픽페투스
				{
					if (dynamic_cast<CEpicBullet*>(*iter)->Get_CanAttacked()) // true일 때 공격
					{
						if (FLY == dynamic_cast<CMonster*>(pMonster)->Get_MstType() || // Epic과 충돌이 이루어지는 몬스터들
							DIP == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							SQUIRT == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							PACER == dynamic_cast<CMonster*>(pMonster)->Get_MstType() ||
							LEAPER == dynamic_cast<CMonster*>(pMonster)->Get_MstType())
						{
							dynamic_cast<CMonster*>(pMonster)->Set_Dead();
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
	if (Get_GameObject(L"MapObj", L"Spike") != nullptr && Get_GameObject(L"GameMst", L"Dople") != nullptr)
	{
		CGameObject* pDople = m_mapLayer.at(L"GameMst")->Collision_GameObject(Get_GameObject(L"MapObj", L"Spike"));
		if (pDople)
		{
			if (DOPLE == dynamic_cast<CMonster*>(pDople)->Get_MstType())
				dynamic_cast<CDople*>(pDople)->Hit();
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
			if (1 == dynamic_cast<CMapObj*>(pMachine)->Get_ObjID())
			{
				if (0 < CPlayer::GetInstance()->Get_Coin())
				{
					CPlayer::GetInstance()->Set_Coin(-1);
					//dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Set_Drop_False(); // 
					dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Set_Game();
				}
			}
		}
	}

	// Shop Npc 는 Epic 과 충돌

	// 야바위 충돌
	if (Get_GameObject(L"MapObj", L"ShellGame") != nullptr)
	{
		if (dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc() != nullptr)
		{
			if (!dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc()->Get_NPC_Game())
			{
				CGameObject* pShellObj = m_mapLayer.at(L"MapObj")->Collision_GameObject(CPlayer::GetInstance());

				if (pShellObj)
				{
					if (2 == dynamic_cast<CMapObj*>(pShellObj)->Get_ObjID()) // Npc <-> Player 충돌
					{
						if (0 < CPlayer::GetInstance()->Get_Coin())
						{
							CPlayer::GetInstance()->Set_Coin(-1);
							dynamic_cast<CShellNpc*>(dynamic_cast<CShellGame*>(Get_GameObject(L"MapObj", L"ShellGame"))->Get_ShellNpc())->Set_NpC_Game();
						}
					}
					else if (3 == dynamic_cast<CMapObj*>(pShellObj)->Get_ObjID()) // Shell <-> Player 충돌
					{
						dynamic_cast<CShell*>(pShellObj)->Set_StartUp(); // 선택한 Shell 위로 오픈

						if (dynamic_cast<CShell*>(pShellObj)->Get_Reward())
						{
							Engine::CGameObject* pGameObject = nullptr;

							ITEM_TYPE eType = dynamic_cast<CShell*>(pShellObj)->Get_ItemType();
							wstring wstrObjTag = dynamic_cast<CShell*>(pShellObj)->Get_DropItemTag();

							for (int i = 0; i < 3; ++i)
							{
								pGameObject = dynamic_cast<CShell*>(pShellObj)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), i);
								m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pGameObject);
							}
							dynamic_cast<CShell*>(pShellObj)->Setting_Reward_False();
						}
						else if (dynamic_cast<CShell*>(pShellObj)->Get_Lose())
						{
							Engine::CGameObject* pFly = nullptr;

							_vec3 vPos;
							dynamic_cast<CShell*>(pShellObj)->Get_TransformCom()->Get_Info(INFO_POS, &vPos);
							for (int i = 0; i < 2; ++i)
							{
								pFly = CFly::Create(m_pGraphicDev, i * 2);
								dynamic_cast<CFly*>(pFly)->Get_Transform()->Set_Pos(vPos);
								pFly->Set_MyLayer(L"GameMst");
								m_mapLayer.at(L"GameMst")->Add_GameObject(L"Fly", pFly);
							}
							dynamic_cast<CShell*>(pShellObj)->Set_Lose_False();
						}
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

	
	// 모닥불 피 닳기
	CGameObject* pObj_Fire = m_mapLayer.at(L"MapObj")->Collision_GameObject(CPlayer::GetInstance());

	if (pObj_Fire)
	{
		if (0 == dynamic_cast<CMapObj*>(pObj_Fire)->Get_ObjID())
		{
			// 플레이어 피 감소
			CPlayer::GetInstance()->Set_Attacked();
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
		if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Machine") != nullptr)//Get_GameObject(L"MapObj", L"Machine") != nullptr) // Get_Reward
		{
			//Get_GameObject(L"MapObj", L"SlotMC"))->Get_Reward() && // 문제 발생(메모리 못 읽어옴)
			//!dynamic_cast<CMapObj*>(Get_GameObject(L"MapObj", L"SlotMC"))->Get_Drop()
			if (dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))->Get_Reward() &&
				!dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))->Get_Drop()) // !dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Get_Game()
			{
				CGameObject* pSlotMC = nullptr;
				CGameObject* pDropItem = nullptr;
				pSlotMC = m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"); //Get_GameObject(L"MapObj", L"SlotMC");

				ITEM_TYPE eType = dynamic_cast<CSlotMC*>(pSlotMC)->Get_ItemType();
				wstring wstrObjTag = dynamic_cast<CSlotMC*>(pSlotMC)->Get_DropItemTag();

				if (HEART == eType)
				{
					pDropItem = dynamic_cast<CMapObj*>(pSlotMC)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), 1);
					m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pDropItem);
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Drop();
					dynamic_cast<CSlotMC*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"SlotMC"))->Set_Reward();
				}
				else if (COIN == eType)
				{
					for (int i = 0; i < 3; ++i)
					{
						pDropItem = dynamic_cast<CMapObj*>(pSlotMC)->Create_Item(eType, 1, m_mapLayer.at(L"GameItem"), i);
						m_mapLayer.at(L"GameItem")->Add_GameObject(wstrObjTag.c_str(), pDropItem);
					}
					dynamic_cast<CSlotMC*>(pSlotMC)->Set_Drop();
					dynamic_cast<CSlotMC*>(Get_GameObject(L"MapObj", L"SlotMC"))->Set_Reward();
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

	// Fire (Campfire) 추가
	if (m_mapLayer.at(L"MapObj")->Get_GameObject(L"Campfire") != nullptr &&
		m_mapLayer.at(L"MapObj")->Get_GameObject(L"Fire") == nullptr)
	{
		dynamic_cast<CCampFire*>(m_mapLayer.at(L"MapObj")->Get_GameObject(L"Campfire"))
			->Set_Fire_ToStage(m_mapLayer.at(L"MapObj"));
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

	// Shell Game (Npc) 추가
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
	// Menu
	CMenu* pMenu = CMenu::Create(m_pGraphicDev, WINCX, WINCY, 0.f, 0.f, 1, 1);
	m_mapLayer.at(L"UI")->Add_GameObject(L"Menu", pMenu);

	// Player Coin UI
	CPlayerCoin* pCoinUI = CPlayerCoin::Create(m_pGraphicDev, 28.f, 28.f, -350.f, 180.f, 1, 1);
	m_mapLayer.at(L"UI")->Add_GameObject(L"CoinUI", pCoinUI);

	// Player Coin Font UI
	for (int i = 0; i < 2; ++i)
	{
		CPLCoinFont* pCoinFont = CPLCoinFont::Create(m_pGraphicDev, 60.f, 60.f, -310 + (i * 20.f), 180.f, 1, 1);
		pCoinFont->Set_Index(i);
		m_mapLayer.at(L"UI")->Add_GameObject(L"CoinFontUI", pCoinFont);
	}

	// Player HP
	CPlayerHP* pPlayerHP = CPlayerHP::Create(m_pGraphicDev, 30.f, 30.f, -370.f, 170.f, 1, 1);
	m_mapLayer.at(L"UI")->Add_GameObject(L"PlayerHP", pPlayerHP);

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

#pragma endregion Boss HP

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

HRESULT CLoadStage::Door_Collision()
{
	if (m_mapLayer.at(L"GameDoor"))
	{
		if (dynamic_cast<CDoor*>(m_mapLayer.at(L"GameDoor")->Get_GameObject(L"Door"))->Get_Open()) // 문이 열렸을 경우에만
		{
			CGameObject* pObj = m_mapLayer.at(L"GameDoor")->Collision_GameObject(CPlayer::GetInstance());

			if (pObj) // 충돌된 문 존재
			{
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
				bool bClear = CStageLoadMgr::GetInstance()->Get_StageInfo(iStageKey).m_bClear;

				// bClear : false면 처음 방문하는방, true면 이미 깬 방
				pScene = CLoadStage::Create(m_pGraphicDev, iStageKey, bClear);

				if (!bClear)
				{
					CPlayer::GetInstance()->Set_IssacRender(false);
					// false : 처음 방문하는방
					CPlayer::GetInstance()->Set_Camera_Cinemachine_01();
					// 중간에 스폰
					//startpos = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);
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
	CLoadStage* pInstance = new CLoadStage(pGraphicDev);
	pInstance->m_bStartScene = bStratScene;
	CPlayer::GetInstance()->Set_Bool_StartScene(true);

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
