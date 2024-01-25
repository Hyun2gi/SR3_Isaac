#include "../Include/stdafx.h"
#include "..\Header\StageTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "Monster.h"
#include "BackGround.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "SkyBox.h"
#include "Effect.h"

#include "StageToolGui.h"

CStageTool::CStageTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStageTool::~CStageTool()
{
}

HRESULT CStageTool::Ready_Scene()
{
	m_pStageTools = new CStageToolGui(g_hWnd, m_pGraphicDev);

	return S_OK;
}

Engine::_int CStageTool::Update_Scene(const _float& fTimeDelta)
{	
	m_pStageTools->Update_ImGuiTools();
	return __super::Update_Scene(fTimeDelta);
}

void CStageTool::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CStageTool::Render_Scene()
{
	// DEBUG
	m_pStageTools->Render_ImGuiTools();
}

HRESULT CStageTool::Ready_Layer_Environment(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CStageTool::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CStageTool::Ready_Layer_UI(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CStageTool::Ready_LightInfo()
{

	return S_OK;
}

CStageTool * CStageTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageTool *	pInstance = new CStageTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Stage Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CStageTool::Free()
{
	delete m_pStageTools;
	__super::Free();
}
