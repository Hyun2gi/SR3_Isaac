#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CFightPlayerThumbnail : public Engine::CUI
{
private:
	explicit CFightPlayerThumbnail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFightPlayerThumbnail(const CFightPlayerThumbnail& rhs);
	virtual ~CFightPlayerThumbnail();

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

	_bool		m_bIsaac = false;
	_float		m_fMoveTimer = 0.f;
	_float		m_fMoveSpeed = 100.f;
	_float		m_fIncreaseSpeed = 70.f;

	_vec3		m_vDestPos;
	_vec3		m_vSecondDestPos;

	_bool		m_bArrivedFirst = false;

public:
	static CFightPlayerThumbnail*		Create(LPDIRECT3DDEVICE9	pGraphicDev, 
		_float fSizeX, _float fSizeY, 
		_float fPosX, _float fPosY, 
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

