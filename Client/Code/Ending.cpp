#include "stdafx.h"
#include "..\Header\Ending.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "EndingBack.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CEnding::~CEnding()
{
}

HRESULT CEnding::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	return S_OK;
}

Engine::_int CEnding::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = __super::Update_Scene(fTimeDelta);

	Engine::Update_Particles(fTimeDelta);

	return iExit;
}

void CEnding::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CEnding::Render_Scene()
{

}

HRESULT CEnding::Ready_Prototype()
{

	return S_OK;
}

HRESULT CEnding::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	//디바이스, x크기, y크기, x좌표, y좌표, x전체 크기, y전체 크기 (전체크기는 default 잡혀있음)
	pGameObject = CEndingBack::Create(m_pGraphicDev, WINCX, WINCY, 0.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	//pGameObject = CMapToolFly::Create(m_pGraphicDev, 100.f, 100.f, -120.f, -100.f, 2, 4);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//m_pMapToolFly->Set_RenderIndex(1);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Proto_MapToolFly", pGameObject), E_FAIL);

	//pGameObject = CStageToolFly::Create(m_pGraphicDev, 100.f, 100.f, 120.f, -100.f, 2, 4);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//m_pStageToolFly->Set_RenderIndex(2);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Proto_StageToolFly", pGameObject), E_FAIL);


	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

CEnding * CEnding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEnding *	pInstance = new CEnding(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Ending Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CEnding::Free()
{
	__super::Free();
}
