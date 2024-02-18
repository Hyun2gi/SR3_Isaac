#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CShader;

END

class CEndingBlackBack : public Engine::CUI
{
private:
	explicit CEndingBlackBack(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEndingBlackBack(const CEndingBlackBack& rhs);
	virtual ~CEndingBlackBack();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void	Set_On() { m_bOff = false; }
	void	Set_Off() { m_bOff = true; }


private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CShader* m_pShaderCom = { nullptr };

	_float				m_fAlpha = 0.f;
	_bool				m_bOff = false;

public:
	static CEndingBlackBack* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_bool bOff = false,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

