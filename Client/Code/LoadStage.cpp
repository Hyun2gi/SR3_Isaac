#include "stdafx.h"
#include "..\Header\LoadStage.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"

//환경
#include "Terrain.h"
#include "DynamicCamera.h"

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

//오브젝트
#include "Coin.h"
#include "Pill.h"
#include "BrimStone.h"
#include "SadOnion.h"
#include "WhipWorm.h"
#include "Epic.h"

CLoadStage::CLoadStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLoadStage::~CLoadStage()
{
}

HRESULT CLoadStage::Ready_Scene(int iType)
{
	m_iCurStageKey = iType;
	m_vecMonsterCount.resize(MONSTER_TYPE_END);

	FAILED_CHECK_RETURN(Load_Level_Data(), E_FAIL);
	FAILED_CHECK_RETURN(Load_Stage_Data(iType), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"GameObject"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);


	return S_OK;
}

Engine::_int CLoadStage::Update_Scene(const _float& fTimeDelta)
{	
	return __super::Update_Scene(fTimeDelta);
}

void CLoadStage::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CLoadStage::Render_Scene()
{
	// DEBUG
}

HRESULT CLoadStage::Load_Level_Data()
{
	string strFilePath = "../Dat/MapLevel.dat";

	ifstream fin(strFilePath);

	string strGetLine = "";
	//코드를 한 줄 읽어온다. (스테이지에 대한 정보는 한줄로 저장하게 만들어뒀기 때문에 가능)

	while (getline(fin, strGetLine))
	{
		vector<string> vecStr;
		int iIndex = 0;

		while (true) {
			// , 위치 찾기
			int pos = strGetLine.find_first_of(',', iIndex);

			// ,를 찾지 못하면 종료
			if (pos == string::npos) {
				break;
			}

			// 분리된 문자열 출력
			vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
			iIndex = pos + 1;
			vecStr.push_back(strGetLine.substr(iIndex));
		}

		m_mapLevel.emplace(pair<int, string>(stoi(vecStr[0]), vecStr[1]));

		vecStr.clear();
	}

	fin.close();


	return S_OK;
}

HRESULT CLoadStage::Load_Stage_Data(int iType)
{
	string strFilePath = "../Dat/" +
		m_mapLevel[iType] + "_Design.dat";

	ifstream fin(strFilePath);

	string strGetLine = "";

	while (getline(fin, strGetLine))
	{
		vector<string> vecStr;
		int iIndex = 0;

		int iMapKey = 0;

		while (true) {
			// , 위치 찾기
			int pos = strGetLine.find_first_of(',', iIndex);

			// ,를 찾지 못하면 종료
			if (pos == string::npos) {
				vecStr.push_back(strGetLine.substr(iIndex));
				break;
			}

			// 분리된 문자열 출력
			vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
			iIndex = pos + 1;
		}

		LoadObj tTemp = { stoi(vecStr[0]), stoi(vecStr[1]), stoi(vecStr[2]), stoi(vecStr[3]), stoi(vecStr[4]) };

		m_mapLoadObj.insert({ iMapKey, tTemp });
		//int iType, int iIndex, float x, float y, float z
		vecStr.clear();

		++iMapKey;
	}

	fin.close();

	return S_OK;

}

HRESULT CLoadStage::Ready_Layer_GameObject(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	for (auto& iter : m_mapLoadObj)
	{
		switch (iter.second.iType)
		{
		case OBJECT:
		{
			switch (iter.second.iIndex)
			{
			case POOP:
			{

				break;
			}
			case CAMPFIRE:
			{

				break;
			}
			case SPIKE:
			{

				break;
			}
			case SHELL_GAME:
			{

				break;
			}
			case SLOT_MC:
			{

				break;
			}
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
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Fly", pGameObject), E_FAIL);

				break;
			}
			case ATTACK_FLY:
			{
				pGameObject = CAttackFly::Create(m_pGraphicDev, m_vecMonsterCount[ATTACK_FLY]);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				dynamic_cast<CAttackFly*>(pGameObject)->Set_CenterObj();
				pGameObject->Set_MyLayer(pLayerTag);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CenterFly", pGameObject), E_FAIL);

				int iCount = m_vecMonsterCount[ATTACK_FLY] * 12 + 1;
				for (int i = iCount; i < iCount + 12; ++i)
				{
					pGameObject = CAttackFly::Create(m_pGraphicDev, i);
					pGameObject->Set_MyLayer(pLayerTag);
					NULL_CHECK_RETURN(pGameObject, E_FAIL);
					FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AttackFly", pGameObject), E_FAIL);
				}

				++m_vecMonsterCount[ATTACK_FLY];

				break;
			}
			case PACER:
			{
				pGameObject = CPacer::Create(m_pGraphicDev, m_vecMonsterCount[PACER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pacer", pGameObject), E_FAIL);

				++m_vecMonsterCount[PACER];

				break;
			}
			case LEAPER:
			{
				pGameObject = CLeaper::Create(m_pGraphicDev, m_vecMonsterCount[LEAPER]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leaper", pGameObject), E_FAIL);

				++m_vecMonsterCount[LEAPER];

				break;
			}
			case SQUIRT:
			{

				break;
			}
			case DIP:
			{
				pGameObject = CDip::Create(m_pGraphicDev, m_vecMonsterCount[DIP]++);
				NULL_CHECK_RETURN(pGameObject, E_FAIL);
				FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Dip", pGameObject), E_FAIL);

				++m_vecMonsterCount[DIP];

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

				break;
			}
			case MOM_LEG:
			{

				break;
			}
			case MOM_EYE:
			{

				break;
			}
			case MOM_HAND:
			{

				break;
			}
			case MOM_SKIN:
			{

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

				break;
			}
			case EPIC:
			{

				break;
			}
			case COLLECTIBLES:
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
			case PILL_0:
			{

				break;
			}
			case PILL_1:
			{

				break;
			}
			case PILL_2:
			{

				break;
			}
			case PILL_3:
			{

				break;
			}
			case PILL_4:
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


	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

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

HRESULT CLoadStage::Ready_LightInfo()
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

CLoadStage * CLoadStage::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType)
{
	CLoadStage *	pInstance = new CLoadStage(pGraphicDev);

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
