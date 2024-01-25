#include "../Include/stdafx.h"
#include "..\Header\MapTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "Monster.h"
#include "BackGround.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "Effect.h"

#include "MapToolGui.h"

CMapTool::CMapTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CMapTool::~CMapTool()
{
}

HRESULT CMapTool::Ready_Scene()
{
	m_pImGuiTools = new CMapToolGui(g_hWnd, m_pGraphicDev);

	return S_OK;
}

Engine::_int CMapTool::Update_Scene(const _float& fTimeDelta)
{	
	m_pImGuiTools->Update_ImGuiTools();

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
