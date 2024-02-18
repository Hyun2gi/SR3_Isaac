#pragma once

#include "Scene.h"

#include "Loading.h"

class CEndingBlackBack;
class CGameEnd;
class CEndingHW;
class CEndingBM;
class CEndingHJ;

class CEnding : public Engine::CScene
{
private:
	enum ENDING_IMG
	{
		GAME_END, HW, BM, HJ
	};

private:
	explicit CEnding(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEnding();

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
	static CEnding*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

	CEndingBlackBack* m_pBlackBack = nullptr;
	CGameEnd* pBack = nullptr;
	CEndingHW* pHW = nullptr;
	CEndingBM* pBM = nullptr;
	CEndingHJ* pHJ = nullptr;

	_int	m_iIndex = 0;

private:
	virtual void Free() override;

};

