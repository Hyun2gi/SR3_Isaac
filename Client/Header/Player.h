#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;
class CBullet;

END

class CPlayer :	public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

	// IDLE : 안걷고 그냥 서있음
	// IDLEWALK : 얼굴방향으로 쏨
	// BACKWALK : 뒷통수보면서 쏨
	// SHOOTWALK : 쏘는 자세 (머리통이 작아짐)
	// GOOD : 따봉
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
	bool			Check_Time(const _float& fTimeDelta);

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	_float				m_fFrame = 0.f;
	_float				m_fPicNum; // 이미지 개수
	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

	PLAYERID			m_ePreState;
	PLAYERID			m_eCurState;

	list<CGameObject*>	m_PlayerBulletList;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

