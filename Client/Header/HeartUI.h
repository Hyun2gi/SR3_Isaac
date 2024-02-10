#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CHeartUI : public Engine::CUI
{
	explicit CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHeartUI(const CHeartUI& rhs);
	virtual ~CHeartUI();

public:
	void			Resetting_Texture(_float fFrame) { m_fCurFrame = fFrame; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_float				m_fAnimSpeed = 1.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	_int				m_iTargetHP;

public:
	static CHeartUI* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

