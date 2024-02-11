#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CGameEnd : public Engine::CUI
{
private:
	explicit CGameEnd(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameEnd(const CGameEnd& rhs);
	virtual ~CGameEnd();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void	Set_Visible(bool bVisible = false) { m_bVisible = bVisible; }

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_float				m_fAnimSpeed = 2.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	bool				m_bVisible = true;

public:
	static CGameEnd*		Create(LPDIRECT3DDEVICE9	pGraphicDev, 
		_float fSizeX, _float fSizeY, 
		_float fPosX, _float fPosY, 
		_int iAnimFrameCount, _int iMaxFrameCount, 
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

