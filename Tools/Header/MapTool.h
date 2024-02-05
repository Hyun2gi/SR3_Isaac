#pragma once

#include "Scene.h"

class CMapToolGui;
class CStageThemeImg;

class CMapTool : public Engine::CScene
{
private:
	const float THEME_SIZE_X = 800.f;
	const float THEME_SIZE_Y = 500.f;

private:
	explicit CMapTool(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMapTool();

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

	void	Set_Theme_Texture(const _tchar* pTextureTag);

private:

	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

private:
	CMapToolGui* m_pImGuiTools;
	CStageThemeImg* m_pThemeImg;

public:
	static CMapTool*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};

