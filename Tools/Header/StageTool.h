#pragma once

#include "Scene.h"

class CStageToolGui;

class CStageTool : public Engine::CScene
{
private:
	explicit CStageTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStageTool();

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

private:
	CStageToolGui* m_pStageTools;

public:
	static CStageTool*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;


};

