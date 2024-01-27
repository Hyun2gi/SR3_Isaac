#include "../Include/stdafx.h"
#include "..\Header\StageTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "BackGround.h"
#include "Terrain.h"
#include "DynamicCamera.h"

#include "StageToolGui.h"
#include "ObjectLoad.h"
#include "MouseObjectImg.h"

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
	m_pStageTools->Set_Target_Scene(this);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	return S_OK;
}

Engine::_int CStageTool::Update_Scene(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	m_pStageTools->Set_Picking_Pos(m_vecPickingPos);
	m_pStageTools->Update_ImGuiTools();

	_vec3 vecTemp = m_vecPickingPos;
	vecTemp.y += 1.f;

	dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic", L"MouseObjectImg", L"Proto_Transform"))->Set_Pos(vecTemp);


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

void CStageTool::Set_Cursor_Image(int iObjType, int iIndex)
{
	m_pMouseImg = dynamic_cast<CMouseObjectImg*>(Get_GameObject(L"GameLogic", L"MouseObjectImg"));

	string strFileName = "Proto_" + CObjectLoad::GetInstance()->Get_File_Name(iObjType, iIndex);

	m_pMouseImg->Set_Cur_Texture_Name(strFileName);
	m_pMouseImg->Swap_Texture();

}

HRESULT CStageTool::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

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

HRESULT CStageTool::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	pGameObject = CMouseObjectImg::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MouseObjectImg", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStageTool::Ready_Layer_UI(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStageTool::Ready_LightInfo()
{
	D3DLIGHT9			tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);


	return S_OK;
}

void CStageTool::Key_Input(const _float& fTimeDelta)
{
	if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
		m_vecPickingPos = Picking_OnTerrain();

	if (Engine::Get_DIMouseMove(DIMS_X))
	{
		m_vecPickingPos = Picking_OnTerrain();
	}
}

_vec3 CStageTool::Picking_OnTerrain()
{
	CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3());

	CTransform* pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Terrain", L"Proto_Transform"));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

	Engine::CCalculator* pCameraCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Get_Component(ID_STATIC, L"Environment", L"DynamicCamera", L"Proto_Calculator"));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

	return pCameraCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}

CStageTool* CStageTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageTool* pInstance = new CStageTool(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("StageTool Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CStageTool::Free()
{
	delete m_pStageTools;
	__super::Free();
}
