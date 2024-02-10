#pragma once

#include "Base.h"
#include "UI.h"

#include "Player.h"
#include "HeartUI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CPlayerHP : public Engine::CUI
{
private:
	explicit CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerHP(const CPlayerHP& rhs);
	virtual ~CPlayerHP();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

	void			Create_HeartUI();
	void			Resetting_UI(_float fUIFrame1, _float fUIFrame2, _float fUIFrame3);
	void			Update_UI();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	vector<CHeartUI*>	m_vecHeartUI;

public:
	static CPlayerHP* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

