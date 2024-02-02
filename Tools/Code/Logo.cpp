#include "../Include/stdafx.h"
#include "..\Header\Logo.h"

#include "Export_System.h"
#include "Export_Utility.h"


#include "BackGround.h"
#include "MapToolFly.h"
#include "StageToolFly.h"
#include "LogoBack.h"
#include "StageTool.h"
#include "MapTool.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	m_bIsMaptool = true;

	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	
	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

Engine::_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = __super::Update_Scene(fTimeDelta);

	m_pMapToolFly->Set_Selected(m_bIsMaptool);
	m_pStageToolFly->Set_Selected(!m_bIsMaptool);

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState('1'))
			m_bIsMaptool = true;

		if (GetAsyncKeyState('2'))
			m_bIsMaptool = false;

		if (GetAsyncKeyState(VK_RETURN))
		{
			if (m_bIsMaptool)
			{
				Engine::CScene* pScene = nullptr;

				pScene = CMapTool::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pScene, -1);

				FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

				return 0;
			}
			else
			{
				Engine::CScene*		pScene = nullptr;

				pScene = CStageTool::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pScene, -1);

				FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

				return 0;
			}
		}

	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	Engine::Render_Font(L"Font_Default", m_pLoading->Get_String(), &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", CTransform::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/MapTool/Logo/ToolLogo.png")), E_FAIL);

	//맵 툴 선택하는 파리
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapToolFly", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/MapTool/MapToolFly/MapToolFly_%d.png", 4)), E_FAIL);
	
	//스테이지 툴 선택하는 파리
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StageToolFly", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/MapTool/StageToolFly/StageToolFly_%d.png", 4)), E_FAIL);
	
	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	//TODO: 백그라운드 출력 시 플라이들 가려지는 문제 해결해야함, 출력순서를 지정해주는 로직을 작성해야함.
	 
	//디바이스, x크기, y크기, x좌표, y좌표, x전체 크기, y전체 크기 (전체크기는 default 잡혀있음)
	pGameObject = m_pLogoBack = CLogoBack::Create(m_pGraphicDev, WINCX, WINCY, 0.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	pGameObject = m_pMapToolFly = CMapToolFly::Create(m_pGraphicDev, 100.f, 100.f, -120.f, -100.f, 2, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pMapToolFly->Set_RenderIndex(1);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Proto_MapToolFly", pGameObject), E_FAIL);
	
	pGameObject = m_pStageToolFly = CStageToolFly::Create(m_pGraphicDev, 100.f, 100.f, 120.f, -100.f, 2, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	m_pStageToolFly->Set_RenderIndex(2);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Proto_StageToolFly", pGameObject), E_FAIL);

	
	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

CLogo * CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo *	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Logo Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	__super::Free();
}
