#include "../Include/stdafx.h"
#include "..\Header\MapTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Logo.h"

#include "Terrain.h"
#include "DynamicCamera.h"

#include "MapToolGui.h"
#include "StageThemeImg.h"

CMapTool::CMapTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CMapTool::~CMapTool()
{
}

HRESULT CMapTool::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	m_pImGuiTools = new CMapToolGui(g_hWnd, m_pGraphicDev);
	m_pImGuiTools->Set_Target_Scene(this);

	return S_OK;
}

Engine::_int CMapTool::Update_Scene(const _float& fTimeDelta)
{	
	m_pImGuiTools->Update_ImGuiTools();

	if (Engine::Key_Down(DIK_Z))
	{
		Engine::CScene* pScene = nullptr;

		pScene = CLogo::Create(m_pGraphicDev, true);
		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

		return 0;
	}

	return __super::Update_Scene(fTimeDelta);
}

void CMapTool::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CMapTool::Render_Scene()
{
	// DEBUG
	m_pImGuiTools->Render_ImGuiTools();
}

void CMapTool::Set_Theme_Texture(const _tchar* pTextureTag)
{
	m_pThemeImg->Set_Theme_Texture(pTextureTag);
}

HRESULT CMapTool::Ready_Layer_Environment(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CMapTool::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CMapTool::Ready_Layer_UI(const _tchar * pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = m_pThemeImg = CStageThemeImg::Create(m_pGraphicDev, THEME_SIZE_X, THEME_SIZE_Y, 0.f, 0.f);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ThemeImg", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CMapTool::Ready_LightInfo()
{

	return S_OK;
}

CMapTool * CMapTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMapTool *	pInstance = new CMapTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CMapTool::Free()
{
	delete m_pImGuiTools;
	__super::Free();
}
