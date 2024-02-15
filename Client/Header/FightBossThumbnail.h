#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CFightBossThumbnail : public Engine::CUI
{
private:
	explicit CFightBossThumbnail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFightBossThumbnail(const CFightBossThumbnail& rhs);
	virtual ~CFightBossThumbnail();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_float		m_fMoveTimer = 0.f;
	_float		m_fMoveSpeed = 3.f;

	_int		m_iStageKey = 0;

public:
	static CFightBossThumbnail*		Create(LPDIRECT3DDEVICE9	pGraphicDev, 
		_float fSizeX, _float fSizeY, 
		_float fPosX, _float fPosY, 
		_int iStageKey,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

