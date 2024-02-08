#include "../Include/stdafx.h"
#include "..\Header\StageTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Logo.h"

#include "Terrain.h"
#include "DynamicCamera.h"

#include "StageToolGui.h"
#include "ObjectLoad.h"
#include "MouseObjectImg.h"
#include "PlacementObject.h"


CStageTool::CStageTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStageTool::~CStageTool()
{
}

HRESULT CStageTool::Ready_Scene()
{	m_vPickingPos = { 0, 0, 0 };
	m_iCurObjType = m_iCurObjIndex = 0;
	m_pStageTools = new CStageToolGui(g_hWnd, m_pGraphicDev);
	m_pStageTools->Set_Target_Scene(this);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameObject(L"GameObject"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	return S_OK;
}

Engine::_int CStageTool::Update_Scene(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	m_pStageTools->Set_Picking_Pos(m_vPickingPos);
	m_pStageTools->Update_ImGuiTools();

	_vec3 vecTemp = m_vPickingPos;
	vecTemp.y += SET_Y_POS;

	dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic", L"MouseObjectImg", L"Proto_Transform"))->Set_Pos(vecTemp);

	if (Engine::Key_Down(DIK_Z))
	{
		Engine::CScene* pScene = nullptr;

		pScene = CLogo::Create(m_pGraphicDev, true);
		NULL_CHECK_RETURN(pScene, -1);

		FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

		return;
	}

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
	m_iCurObjType = iObjType;
	m_iCurObjIndex = iIndex;

	string strFileName = "Proto_" + CObjectLoad::GetInstance()->Get_File_Name(m_iCurObjType, m_iCurObjIndex);

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

	pGameObject = m_pMouseImg = CMouseObjectImg::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MouseObjectImg", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });


	return S_OK;
}

HRESULT CStageTool::Ready_Layer_GameObject(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

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

void CStageTool::Create_Placement_Object()
{
	if (m_mapLayer.find(L"GameObject") == m_mapLayer.end())
		Ready_Layer_GameObject(L"GameObject");

	string strFileName = CObjectLoad::GetInstance()->Get_File_Name(m_iCurObjType, m_iCurObjIndex);

	wstring wstr;
	wstr.assign(strFileName.begin(), strFileName.end());

	PlacementObj tObj = { m_iCurObjType, m_iCurObjIndex, wstr };
	m_vecPlacementObj.push_back(tObj);

	CGameObject* pPlacementObj;
	pPlacementObj = CPlacementObject::Create(m_pGraphicDev, tObj.wstrName, m_iCurObjType, m_iCurObjIndex);
	m_mapLayer.at(L"GameObject")->Add_GameObject(tObj.wstrName.c_str(), pPlacementObj);

	_vec3 vTemp = m_vPickingPos;
	vTemp.y += SET_Y_POS;
	dynamic_cast<CTransform*>(pPlacementObj->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS] = vTemp;
}

void CStageTool::Create_Placement_Object(int iObjType, int iIndex, float x, float y, float z)
{
	if (m_mapLayer.find(L"GameObject") == m_mapLayer.end())
		Ready_Layer_GameObject(L"GameObject");

	string strFileName = CObjectLoad::GetInstance()->Get_File_Name(iObjType, iIndex);

	wstring wstr;
	wstr.assign(strFileName.begin(), strFileName.end());

	PlacementObj tObj = { iObjType, iIndex, wstr };
	m_vecPlacementObj.push_back(tObj);

	CGameObject* pPlacementObj;
	pPlacementObj = CPlacementObject::Create(m_pGraphicDev, tObj.wstrName, m_iCurObjType, m_iCurObjIndex);
	m_mapLayer.at(L"GameObject")->Add_GameObject(tObj.wstrName.c_str(), pPlacementObj);

	_vec3 vTemp(x, y, z);
	dynamic_cast<CTransform*>(pPlacementObj->Get_Component(ID_DYNAMIC, L"Proto_Transform"))->m_vInfo[INFO_POS] = vTemp;
}

void CStageTool::Clear_Placement_Object()
{
	m_vecPlacementObj.clear();
	Delete_Layer(L"GameObject");
}

void CStageTool::Key_Input(const _float& fTimeDelta)
{
	if (Engine::Get_DIMouseMove(DIMS_X))
		m_vPickingPos = Picking_OnTerrain();

	if (Engine::Key_Down(DIM_RB))
	{
		//m_vPickingPos = Picking_OnTerrain();
		Create_Placement_Object();
		m_pStageTools->Push_Placement_Obj_List(m_iCurObjType, m_iCurObjIndex, m_vPickingPos.x, m_vPickingPos.y + SET_Y_POS, m_vPickingPos.z);
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
