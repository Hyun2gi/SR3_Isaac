#include "stdafx.h"
#include "..\Header\Loading.h"
#include "Export_System.h"
#include "Export_Utility.h"

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

	// Boss - Monstro
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/Monstro_%d.png", 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroJumpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroJump_%d.png", 5)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroAttackTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroAttack.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroUpTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroUp_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonstroDownTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Monstro/MonstroDown_%d.png", 2)), E_FAIL);

	// Boss - Mom

#pragma endregion MonsterTexture


	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EffectTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/StageCube.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);



	//현정Loading추가
	//ISAAC 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_idle_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK_SMALL", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_small_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_RIGHT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_right_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LEFT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_left_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_THUMBS_UP", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_thumbs_up_%d.png", 2)), E_FAIL);

	//눈물추가
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_%d.png", 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MstTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Monstertear/tear_%d.png", 13)), E_FAIL);

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

	m_bFinish = true;

	lstrcpy(m_szLoading, L"Loading Complete");

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

#pragma endregion MonsterTexture


	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EffectTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTexture", CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/Terrain/StageCube.dds")), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Calculator", CCalculator::Create(m_pGraphicDev)), E_FAIL);



	//현정Loading추가
	//ISAAC 이미지
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_IDLE", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_idle_%d.png",11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_BACK_SMALL", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_back_small_%d.png", 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_RIGHT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_right_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_LEFT", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_left_%d.png", 8)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTexture_THUMBS_UP", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Isaac/isaac_thumbs_up_%d.png", 2)), E_FAIL);

	//눈물추가
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Tear/Tear_%d.png", 13)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MstTear", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Object/Monstertear/tear_%d.png", 13)), E_FAIL);

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
	case LOADING_STAGE_0:
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
