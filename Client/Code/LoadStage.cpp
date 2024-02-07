#include "stdafx.h"
#include "..\Header\LoadStage.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "PlayerBullet.h"

#include "StageLoadMgr.h"

//환경
#include "Terrain.h"
#include "DynamicCamera.h"
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

//아이템
#include "Coin.h"
#include "Pill.h"
#include "BrimStone.h"
#include "SadOnion.h"
#include "WhipWorm.h"
#include "Epic.h"
#include "Heart.h"
#include "HeartHalf.h"

CLoadStage::CLoadStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_bStartScene(false)
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

	CPlayer::GetInstance()->Ready_GameObject(m_pGraphicDev);
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
	//pParticleScatter->Update_Particle(fTimeDelta);
	_int	iExit = __super::Update_Scene(fTimeDelta);

	CPlayer::GetInstance()->Update_GameObject(fTimeDelta);

	if (Check_Cube_Arrived() && !m_bIsCreated)
	{
		m_bIsCreated = true;
		FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"GameObject"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Door(L"GameDoor"), E_FAIL);
	}

	//bool bIsIntersect = dynamic_cast<CDoor*>(m_mapLayer.at(L"GameDoor")->Get_GameObject(L"Door"))->Get_Collision();

	//if (bIsIntersect)
	//{
	//	int i = 0;
	//	//pGameObject
	//}

	if (Engine::Key_Down(DIK_1))
	{
		Engine::CScene* pScene = nullptr;

		pScene = CLoadStage::Create(m_pGraphicDev, 1);
		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

		return 0;
	}

	if (Engine::Key_Down(DIK_2))
	{
		Engine::CScene* pScene = nullptr;

		pScene = CLoadStage::Create(m_pGraphicDev, 2);
		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

		return 0;
	}

	if (Engine::Key_Down(DIK_3))
	{
		Engine::CScene* pScene = nullptr;

		pScene = CLoadStage::Create(m_pGraphicDev, 3);
		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

		return 0;
	}

	//타임 델타 스케일 조절 예시 _ 사용
	if (Engine::Key_Down(DIK_P))
	{
		Engine::Set_TimeDeltaScale(L"Timer_Second", 0.1f);
	}
	if (Engine::Key_Down(DIK_O))
	{
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	}

	return iExit;
}

void CLoadStage::LateUpdate_Scene()
{
	CPlayer::GetInstance()->LateUpdate_GameObject();
	Check_All_Dead();
	__super::LateUpdate_Scene();

	//if (m_bIsCreated)
	//	Door_Collision();
}

void CLoadStage::Render_Scene()
{
	// DEBUG
	//pParticleScatter->Render_GameObject();
}


HRESULT CLoadStage::Ready_Layer_GameObject(const _tchar* pLayerTag)
{
	CPlayer::GetInstance()->Set_LayerTag((_tchar*)pLayerTag);

	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	auto mapLoadObj = CStageLoadMgr::GetInstance()->Get_StageInfo().at(m_iCurStageKey).m_mapLoadObj;

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
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Poop", pGameObject), E_FAIL);

				break;
			}
			case CAMPFIRE:
			{
				pGameObject = CCampFire::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Campfire", pGameObject), E_FAIL);

				break;
			}
			case SPIKE:
			{
				pGameObject = CSpike::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Spike", pGameObject), E_FAIL);

				break;
			}
			case SHELL_GAME:
			{
				pGameObject = CShellGame::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ShellGame", pGameObject), E_FAIL);

				break;
			}
			case SLOT_MC:
			{
				pGameObject = CSlotMC::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlotMC", pGameObject), E_FAIL);

				break;
			}

			//TODO: 샵 추가해야함
			}
			break;
		}
		case MONSTER:
		{
			switch (iter.second.iIndex)
			{
			case FLY:
			{
				pGameObject = CFly::Create(m_pGraphicDev, m_vecMonsterCount[FLY]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);

				break;
			}
			case ATTACK_FLY:
			{
				pGameObject = CAttackFly::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);

				++m_vecMonsterCount[ATTACK_FLY];

				break;
			}
			case PACER:
			{
				pGameObject = CPacer::Create(m_pGraphicDev, m_vecMonsterCount[PACER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);

				break;
			}
			case LEAPER:
			{
				pGameObject = CLeaper::Create(m_pGraphicDev, m_vecMonsterCount[LEAPER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

				break;
			}
			case SQUIRT:
			{
				pGameObject = CSquirt::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				pGameObject->Set_MyLayer(pLayerTag);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Squirt", pGameObject), E_FAIL);

				break;
			}
			case DIP:
			{
				pGameObject = CDip::Create(m_pGraphicDev, m_vecMonsterCount[DIP]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);

				break;
			}
			case DOPLE:
			{

				break;
			}
			case CHARGER:
			{
				pGameObject = CCharger::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
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
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monstro", pGameObject), E_FAIL);

				break;
			}
			case MOM:
			{
				pGameObject = CMom::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CTransform*>(pGameObject->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS]
					= { iter.second.iX, iter.second.iY, iter.second.iZ };
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
					pGameObject->Set_MyLayer(pLayerTag);
					dynamic_cast<CMomParts*>(pGameObject)->Setting_Value();
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MomParts", pGameObject), E_FAIL);
				}
				break;
			}
	
			}
			break;
		}
		case ITEM:
		{
			switch (iter.second.iIndex)
			{
			case BRIM:
			{
				//pGameObject = CCoin::Create(m_pGraphicDev);
				//NULL_CHECK_RETURN(pGameObject, E_FAIL);
				//pGameObject->Set_MyLayer(pLayerTag);
				//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Coin", pGameObject), E_FAIL);

				break;
			}
			case EPIC:
			{

				break;
			}
			case SAD_ONION:
			{

				break;
			}
			case TRINKET:
			{

				break;
			}
			case COIN:
			{

				break;
			}
			case HEART:
			{

				break;
			}
			case HEART_HALF:
			{

				break;
			}
			case PILL:
			{

				break;
			}

			}
			break;
		}
		}
	}

	m_mapLayer.insert({ pLayerTag, pLayer });
}

HRESULT CLoadStage::Ready_Layer_Door(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	_vec3 vTempPos;

	auto vecDoorTheme = CStageLoadMgr::GetInstance()->Get_StageInfo().at(m_iCurStageKey).m_vecConnectRoom;

	int i = 0;
	for (auto& iter : vecDoorTheme)
	{
		if (1 > iter) continue;

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

HRESULT CLoadStage::Ready_Layer_Environment(const _tchar * pLayerTag)
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
	pGameObject->Set_MyLayer(pLayerTag);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CLoadStage::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

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

	string strType = CStageLoadMgr::GetInstance()->Get_StageInfo().at(m_iCurStageKey).m_strType;

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

HRESULT CLoadStage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;



	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

bool CLoadStage::Check_Cube_Arrived()
{
	return m_pLeftWall->Get_Arrived() && m_pRightWall->Get_Arrived()
		&& m_pTopWall->Get_Arrived() && m_pBottomWall->Get_Arrived()
		&& m_pFloor->Get_Arrived();
	
}

void CLoadStage::Check_All_Dead()
{
	int iCount = 0;

	for (auto& iter : m_vecMonsterCount)
		iCount += iter;

	if(0 == iCount)
		dynamic_cast<CDoor*>(m_mapLayer.at(L"GameDoor")->Get_GameObject(L"Door"))->Set_Open();
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
					startpos = _vec3(38.5, 0, 20);
				}
				else if (doornum == 1)
				{
					//right
					startpos = _vec3(3.5, 0, 20);
				}
				else if (doornum == 2)
				{
					//top
					startpos = _vec3(20, 0, 3.5);
				}
				else if (doornum == 3)
				{
					//bottom
					startpos = _vec3(20, 0, 38.5);
				}
				

				CPlayer::GetInstance()->Set_KeyBlock(true);
				CPlayer::GetInstance()->Set_StartPos(startpos);

				// 스테이지 변경
				Engine::CScene* pScene = nullptr;

				pScene = CLoadStage::Create(m_pGraphicDev, dynamic_cast<CDoor*>(pObj)->Get_Stage_Num_Key());
				NULL_CHECK_RETURN(pScene, -1);

				FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			}
		}
	}
}

CLoadStage * CLoadStage::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType, bool bStratScene)
{
	CLoadStage *	pInstance = new CLoadStage(pGraphicDev);
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
