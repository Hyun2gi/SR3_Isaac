#pragma once

#include "Base.h"
#include "UI.h"

#include "Monster.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CBossHP : public Engine::CUI
{
private:
	explicit CBossHP(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBossHP(const CBossHP& rhs);
	virtual ~CBossHP();

public:
	void		Set_Target(CMonster* pMonster) { m_pMonster = pMonster; }
	void		Set_IsMom() { m_bIsMom = true; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();
	void			Update_Scale();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_bool				m_bIsMom;

	_float				m_fAnimSpeed = 1.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	_int				m_iTargetHP;

	CMonster*			m_pMonster;

public:
	static CBossHP* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;
};

