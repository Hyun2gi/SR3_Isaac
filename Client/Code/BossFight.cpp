#include "stdafx.h"
#include "..\Header\BossFight.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "LoadStage.h"
#include "BlackBackground.h"
#include "FightPlayerName.h"
#include "FightPlayerThumbnail.h"
#include "FightBossName.h"
#include "FightBossThumbnail.h"
#include "FightVs.h"

CBossFight::CBossFight(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CBossFight::~CBossFight()
{
}

HRESULT CBossFight::Ready_Scene()
{

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	return S_OK;
}

Engine::_int CBossFight::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = __super::Update_Scene(fTimeDelta);

	m_fSceneChangeTimer += fTimeDelta;

	// 4.5�� ���İ� �Ǹ� �ٽ� ȭ�� �ٱ����� ���������� ������ �������ش�.
	if (4.8f < m_fSceneChangeTimer)
	{
		pPlayerName->Set_Back();
		pPlayerThumbnail->Set_Back();
		pBossThumbnail->Set_Back();
		pBossName->Set_Back();
		pVs->Set_Back();
	}
	//5�ʰ� �Ǹ� �� ��ȯ
	if (5.f < m_fSceneChangeTimer)
	{
		// �������� ����
		Engine::CScene* pScene = nullptr;

		// ���� ���������� ť�� ���� ���ʿ��Ͽ� true���� �Ѱ��ֱ�
		pScene = CLoadStage::Create(m_pGraphicDev, m_iStageKey, true);

		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);
	}

	return iExit;
}

void CBossFight::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CBossFight::Render_Scene()
{

}

HRESULT CBossFight::Ready_Prototype()
{

	return S_OK;
}

HRESULT CBossFight::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	//����̽�, xũ��, yũ��, x��ǥ, y��ǥ, x��ü ũ��, y��ü ũ�� (��üũ��� default ��������)
	pGameObject = CBlackBackground::Create(m_pGraphicDev, WINCX, WINCY, 0.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BlackBackground", pGameObject), E_FAIL);

	pGameObject = pBossName = CFightBossName::Create(m_pGraphicDev, 192.f * 1.5, 64.f * 1.5, 220.f, 200.f, m_iStageKey);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BossName", pGameObject), E_FAIL);

	pGameObject = pBossThumbnail = CFightBossThumbnail::Create(m_pGraphicDev, 256.f * 2, 256.f * 2, 150.f, -100.f, m_iStageKey);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BossThumbnail", pGameObject), E_FAIL);

	pGameObject = pVs = CFightVs::Create(m_pGraphicDev, 128.f * 1.5, 64.f * 1.5, -10.f, 200.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Vs", pGameObject), E_FAIL);

	pGameObject = pPlayerName = CFightPlayerName::Create(m_pGraphicDev, 192.f * 1.5, 64.f * 1.5, -220.f, 200.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PlayerName", pGameObject), E_FAIL);

	pGameObject = pPlayerThumbnail = CFightPlayerThumbnail::Create(m_pGraphicDev, 256.f * 2, 256.f * 2, -150.f, -125.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PlayerThumbnail", pGameObject), E_FAIL);


	//pGameObject = CStageToolFly::Create(m_pGraphicDev, 100.f, 100.f, 120.f, -100.f, 2, 4);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//m_pStageToolFly->Set_RenderIndex(2);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Proto_StageToolFly", pGameObject), E_FAIL);


	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

CBossFight * CBossFight::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iStageKey)
{
	CBossFight *	pInstance = new CBossFight(pGraphicDev);
	pInstance->m_iStageKey = iStageKey;

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Ending Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CBossFight::Free()
{
	__super::Free();
}
