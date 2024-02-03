#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CStageThemeImg : public Engine::CUI
{
private:
	explicit CStageThemeImg(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStageThemeImg(const CStageThemeImg& rhs);
	virtual ~CStageThemeImg();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void	Set_Theme_Texture(const _tchar* pTextureTag);

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

public:
	static CStageThemeImg*		Create(LPDIRECT3DDEVICE9	pGraphicDev, 
		_float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

