#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CItemFontUI : public Engine::CUI
{
	const _float FONT_Y = 95.f;
	const _float DETAIL_Y = 123.f;
private:
	explicit CItemFontUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CItemFontUI(const CItemFontUI& rhs);
	virtual ~CItemFontUI();

public:
	void			Set_Render() { m_bRender = true; m_fAccTimeDelta = 0.f; }
	void			Set_ItemType(ITEM_TYPE eItemType) { m_eItemType = eItemType; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

	bool			Check_Time(const _float& fTimeDelta);

	void			Change_Font();
	void			Animation_Font();		

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_bool				m_bRender;

	_float				m_fAnimSpeed = 1.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

	_tchar				m_szItem[128];
	_tchar				m_szItemDetail[128];
	ITEM_TYPE			m_eItemType;
	_vec2				m_vecFontPos;
	_vec2				m_vecDetailPos;
	_vec2				m_vecEndPos;

public:
	static CItemFontUI* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;


};