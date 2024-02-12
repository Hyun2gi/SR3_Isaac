#include "stdafx.h"
#include "..\Header\Loading.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Player.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);
	
	m_hThread = (HANDLE)_beginthreadex(NULL,	// 디폴트 보안 속성
										0,		// 디폴트 스택 사이즈(1바이트)
										Thread_Main, // 동작 시킬 쓰레드 함수 이름(__stdcall 규약)
										this,		// 쓰레드 함수의 매개 변수로 전달할 값
										0,			// 쓰레드 생성 및 실행을 조절하기 위한 FLAG
										NULL);		// 쓰레드 ID 반환

	m_eID = eID;

	return S_OK;
}

_uint CLoading::Loading_ForStage_Load(int iType)
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriCol", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcCol", CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTex", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTex", CCubeTex::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/StageCube_Up.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HeightTerrainTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Height.bmp")), E_FAIL);

#pragma region MonsterTexture

	// Mst - Fly / AttackFly
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FlyTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/Fly_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AttackFlyTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/AttackFly_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FlyDeadTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/FlyDead_%d.png", 11)), E_FAIL);

	// Mst - Dip
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DipTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dip/Dip_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DipSlideTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dip/DipSlide_%d.png", 2)), E_FAIL);

	// Mst - Pacer
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PacerTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Pacer/Pacer_%d.png", 10)), E_FAIL);

	// Mst - Squirt
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SquirtTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Squirt/Squirt_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SquirtSlideTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Squirt/SquirtSlide_%d.png", 3)), E_FAIL);

	// Mst - Leaper
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/Leaper_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperUpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/LeaperUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperDownTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/LeaperDown_%d.png", 2)), E_FAIL);

	// Mst - Charger
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChargerTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Charger/Charger_%d.png", 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChargerAttackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Charger/ChargerAttack.png", 1)), E_FAIL);

	// Mst - Dople
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DopleTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dople/Dople_%d.png", 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DopleBackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dople/DopleBack_%d.png", 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DopleLeftTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dople/DopleLeft_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DopleRightTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dople/DopleRight_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DopleDeadTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dople/DopleDead_%d.png", 2)), E_FAIL);

	// Boss - Monstro
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/Monstro_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroJumpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroJump_%d.png", 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroAttackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroAttack.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroUpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroDownTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroDown_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroDeadTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroDead.png")), E_FAIL);

	// Boss - Mom
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomLeg.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomEyeTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomEye.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomSkinTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomSkin_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomHandTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomHand_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomDoorTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomDoor.png")), E_FAIL);

#pragma endregion MonsterTexture

#pragma region MapObject

	// Poop
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PoopTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Poop/Poop_%d.png", 5)), E_FAIL);

	// CampFire
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WoodTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Campfire/Campfire.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FireTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Campfire/Fire_%d.png", 6)), E_FAIL);

	// Spike
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SpikeTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Spike/Spike.png")), E_FAIL);

	// SlotMC
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlotMCTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/SlotMC/SlotMC_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrokenSlotMCTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/SlotMC/BrokenSlotMC.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlotCardTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/SlotMC/SlotCard_%d.png", 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlotCardRandTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/SlotMC/SlotRandom_%d.png", 6)), E_FAIL);

	// Shop
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShopNpcTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Shop/ShopNpc_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShopNpcThumbsTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Shop/ShopNpc_ThumbsUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShopNpcDeadTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Shop/ShopNpcDead_%d.png", 2)), E_FAIL);

	// Door (Normal)
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalDoorCloseTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/BasementDoorClose.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalDoorOpenTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/BasementDoorOpen.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalDoorOpneingTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/BasementDoorOpeneing_%d.png", 4)), E_FAIL);

	// Door (Boss)
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossDoorCloseTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/BossDoor/BossDoorClose.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossDoorOpenTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/BossDoor/BossDoorOpen.png")), E_FAIL);

	// Door (Arcade)
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArcadeDoorCloseTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/ArcadeDoor/ArcadeDoorClose.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArcadeDoorOpenTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/ArcadeDoor/ArcadeDoorOpen.png")), E_FAIL);

	// Door (Devil)
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DevilDoorCloseTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/DevilDoor/DevilDoorClose.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DevilDoorOpenTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/DevilDoor/DevilDoorOpen.png")), E_FAIL);

	// Door (Treasure)
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TreasureDoorCloseTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/TreasureDoor/TreasureDoorClose.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TreasureDoorOpenTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Door/TreasureDoor/TreasureDoorOpen.png")), E_FAIL);

	// Shell Game
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShellNpcTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/ShellGame/ShellGameNpc_0.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShellNpcGameTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/ShellGame/ShellGaming_%d.png", 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShellTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/ShellGame/ShellGame.png")), E_FAIL);

	// CoinFont
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoinFontTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Font/CoinFont_%d.png", 3)), E_FAIL);

#pragma endregion MapObject


	//방 배경 관련 텍스쳐들
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EffectTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90)), E_FAIL);

	//기본 텍스처, 큐브로 바꾸기로하고 사용하지않음
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/Skybox.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BasementFloorCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/StageCube.dds")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BasementWallCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Wall/StageWallCube.dds")), E_FAIL);

	//TODO: 텍스쳐 경로 수정 필요하다!
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SheolFloorCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/SheolCube.dds")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SheolWallCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Wall/SheolWallCube.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArcadeFloorCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/ArcadeCube.dds")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArcadeWallCubeTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Wall/ArcadeWallCube.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ObstacleTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Obstacle/Obstacle.dds")), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);



	//현정Loading추가
	//ISAAC 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_idle_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK_SMALL", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_small_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_RIGHT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_right_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LEFT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_left_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_THUMBS_UP", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_thumbs_up_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_ATTACKED", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_attacked.png", 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_GET_BAD_ITEM", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_get_bad_item.png", 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LIE_CRY", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_lie_cry.png", 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LIE_CRY_OPEN_EYE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_lie_cry_open_eye.png", 1)), E_FAIL);

	//눈물추가
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_%d.png", 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear_Effect", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_Effect_%d.png", 10)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MstTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Monstertear/tear_%d.png", 13)), E_FAIL);

	//BrimCenter
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_BrimCenter", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/BrimCenter_%d.png", 4)), E_FAIL);
	//BrimHead
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_BrimHead", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/BrimHead.png", 1)), E_FAIL);


	//Coin 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoinTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/coin_%d.png", 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoinTexture_EFFECT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/coin_effect_%d.png", 5)), E_FAIL);

	//Pill 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Pill", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pill_%d.png", 5)), E_FAIL);

	//우는양파 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_SadOnion", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/collectibles_001_thesadonion.png", 1)), E_FAIL);

	//BrimStone
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_BrimstoneItem", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/BrimstoneItem.png", 1)), E_FAIL);

	//EpicItem
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_EpicItem", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/EpicItem.png", 1)), E_FAIL);

	//EpicItem
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Whipworm", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/trinket_027_whipworm.png", 1)), E_FAIL);

	//pickup_heart
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Heart", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pickup_heart.png", 1)), E_FAIL);

	//pickup_heart_half
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_HeartHalf", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pickup_heart_half.png", 1)), E_FAIL);

	// EPIC 총알
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_EpicBullet", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Effect/EpicBullet.png", 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_EpicEff", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Effect/EpicEff_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_EpicSpace", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Effect/EpicSpace_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_EpicTarget", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Effect/EpicTarget.png", 1)), E_FAIL);

#pragma region UI Texture

	// Menu
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MenuTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Intro/Menu.png")), E_FAIL);

	// Boss HP
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossHPTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Boss/BossHP.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossHPBarTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Boss/BossHPBar.png")), E_FAIL);

	// Player Coin
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerCoinTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Item/coin.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerCoinFontTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Font/Font_%d.png", 10)), E_FAIL);

	// Player Heart
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerHeartTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Item/ui_hearts_%d.png", 3)), E_FAIL);

	//Ending
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EndingBackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Ending/Background.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GameEndTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Ending/GameEnd/Ending_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HWTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Ending/HW/HW_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BMTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Ending/BM/BM_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HJTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Ending/HJ/HJ_%d.png", 2)), E_FAIL);


#pragma endregion UI Texture



	m_bFinish = true;

	//lstrcpy(m_szLoading, L"Loading Complete");

	return 0;


	return _uint();
}

_uint CLoading::Loading_ForStage()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriCol", CTriCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcCol", CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTex", CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTex", CCubeTex::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/StageCube_Up.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HeightTerrainTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Height.bmp")), E_FAIL);

#pragma region MonsterTexture

	// Mst - Fly / AttackFly
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FlyTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/Fly_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AttackFlyTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/AttackFly_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FlyDeadTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Fly/FlyDead_%d.png", 11)), E_FAIL);

	// Mst - Dip
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DipTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dip/Dip_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DipSlideTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Dip/DipSlide_%d.png", 2)), E_FAIL);

	// Mst - Pacer
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PacerTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Pacer/Pacer_%d.png", 10)), E_FAIL);

	// Mst - Squirt
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SquirtTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Squirt/Squirt_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SquirtSlideTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Squirt/SquirtSlide_%d.png", 3)), E_FAIL);

	// Mst - Leaper
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/Leaper_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperUpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/LeaperUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LeaperDownTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Leaper/LeaperDown_%d.png", 2)), E_FAIL);

	// Mst - Charger
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChargerTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Charger/Charger_%d.png", 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChargerAttackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Charger/ChargerAttack.png", 1)), E_FAIL);

	// Boss - Monstro
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/Monstro_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroJumpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroJump_%d.png", 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroAttackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroAttack.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroUpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroDownTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroDown_%d.png", 2)), E_FAIL);
	
	// Boss - Mom
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomLeg.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomEyeTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomEye.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomSkinTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomSkin_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomHandTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomHand_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MomDoorTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Mom/MomDoor.png")), E_FAIL);

#pragma endregion MonsterTexture


	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EffectTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/Skybox.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);



	//현정Loading추가
	//ISAAC 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_idle_%d.png",11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK_SMALL", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_small_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_RIGHT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_right_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LEFT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_left_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_THUMBS_UP", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_thumbs_up_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_ATTACKED", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_attacked.png", 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_GET_BAD_ITEM", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_get_bad_item.png", 1)), E_FAIL);

	//눈물추가
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_%d.png", 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MstTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Monstertear/tear_%d.png", 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear_Effect", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_Effect_%d.png", 10)), E_FAIL);

	//BrimCenter
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_BrimCenter", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/BrimCenter_%d.png", 4)), E_FAIL);
	//BrimHead
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BulletTexture_BrimHead", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/BrimHead.png", 1)), E_FAIL);

	//Coin 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoinTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/coin_%d.png", 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoinTexture_EFFECT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/coin_effect_%d.png", 5)), E_FAIL);

	//Pill 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Pill", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pill_%d.png", 5)), E_FAIL);

	//우는양파 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_SadOnion", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/collectibles_001_thesadonion.png", 1)), E_FAIL);

	//BrimStone
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_BrimstoneItem", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/BrimstoneItem.png", 1)), E_FAIL);

	//EpicItem
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_EpicItem", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/EpicItem.png", 1)), E_FAIL);

	//whipworm
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Whipworm", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/trinket_027_whipworm.png", 1)), E_FAIL);

	//pickup_heart
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_Heart", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pickup_heart.png", 1)), E_FAIL);

	//pickup_heart_half
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemTexture_HeartHalf", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Item/pickup_heart_half.png", 1)), E_FAIL);

	m_bFinish = true;

	lstrcpy(m_szLoading, L"Loading Complete");

	return 0;
}

unsigned int CLoading::Thread_Main(void * pArg)
{
	CLoading*		pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE_1:
		iFlag = pLoading->Loading_ForStage_Load((int)pLoading->Get_LoadingID());
		break;
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	//_endthreadex(0);

	return iFlag;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	CLoading *		pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Loading Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}
