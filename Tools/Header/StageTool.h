#pragma once

#include "Scene.h"

class CStageToolGui;
class CMouseObjectImg;
class CPlacementObject;

class CStageTool : public Engine::CScene
{
private:
	struct PlacementObj
	{
		int iType;
		int iIndex;
		wstring wstrName;
	};

private:
	const float SET_Y_POS = 0.8f;

private:
	explicit CStageTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStageTool();

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

	void	Set_Cursor_Image(int iObjType, int iIndex);

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);

	void			Create_Placement_Object();

private:
	void	Key_Input(const _float& fTimeDelta);

	_vec3	Picking_OnTerrain();

private:
	CStageToolGui* m_pStageTools;
	CMouseObjectImg* m_pMouseImg;

	_vec3	m_vPickingPos;

	int	m_iCurObjType, m_iCurObjIndex;

	//씬에서 가지고 있을 설치한 오브젝트의 정보를 저장하는 벡터
	vector<PlacementObj> m_vecPlacementObj;

public:
	static CStageTool* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;


};

