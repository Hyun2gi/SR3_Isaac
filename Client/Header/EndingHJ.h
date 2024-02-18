#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CShader;

END

class CEndingHJ : public Engine::CUI
{
private:
	explicit CEndingHJ(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEndingHJ(const CEndingHJ& rhs);
	virtual ~CEndingHJ();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;


	void	Set_Visible(bool bVisible = false) { m_bVisible = bVisible; }
	void	Set_On() { m_bOn = true; }
	void	Set_Off() { m_bOn = false; }

	const _float& Get_WaitTimer() { return m_fWaitTimer; }
	const _bool& Get_Visible() { return m_bVisible; }
	const _bool& Get_On() { return m_bOn; }

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CShader*	m_pShaderCom = { nullptr };

	_float				m_fAnimSpeed = 2.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	_float				m_fAlpha = 0.f;

	_float				m_fWaitTimer = 3.5f;

	_bool				m_bVisible = false;
	_bool				m_bOn = false;

public:
	static CEndingHJ*		Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY, 
		_float fPosX, _float fPosY, 
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

