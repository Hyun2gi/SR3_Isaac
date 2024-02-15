#pragma once

#include "Scene.h"

#include "Loading.h"

class CBlackBackground;
class CFightPlayerName;
class CFightPlayerThumbnail;
class CFightBossName;
class CFightBossThumbnail;
class CFightVs;

class CBossFight : public Engine::CScene
{
private:
	explicit CBossFight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossFight();

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

private:
	HRESULT			Ready_Prototype();

	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }

public:
	static CBossFight*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iStageKey);

private:
	_float m_fSceneChangeTimer = 0.f;

	_int	m_iStageKey = 0;

	CFightPlayerName* pPlayerName = nullptr;
	CFightPlayerThumbnail* pPlayerThumbnail = nullptr;
	CFightBossName* pBossName = nullptr;
	CFightBossThumbnail* pBossThumbnail = nullptr;
	CFightVs* pVs = nullptr;

private:
	virtual void Free() override;

};

