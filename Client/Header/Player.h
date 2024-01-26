#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CPlayer :	public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

	// IDLE : ¾È°È°í ±×³É ¼­ÀÖÀ½
	// IDLEWALK : ¾ó±¼¹æÇâÀ¸·Î ½ô
	// BACKWALK : µÞÅë¼öº¸¸é¼­ ½ô
	// SHOOTWALK : ½î´Â ÀÚ¼¼ (¸Ó¸®ÅëÀÌ ÀÛ¾ÆÁü)
	// GOOD : µûºÀ
	enum PLAYERID { P_IDLE, P_IDLEWALK, P_BACKWALK, P_LEFTWALK, P_RIGHTWALK, P_SHOOTWALK, P_GOOD, P_END  };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Height_OnTerrain();
	_vec3			Picking_OnTerrain();
	void			Motion_Change();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	_float					m_fFrame = 0.f;

	PLAYERID			m_ePreState;
	PLAYERID			m_eCurState;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

