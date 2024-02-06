#pragma once

#include "Base.h"
#include "GameObject.h"

#include "Player.h"

BEGIN(Engine)

class CTexture;
class CRcTex;
class CTransform;
class CCalculator;

END

class CMonster :	public Engine::CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	void			Hit() { m_bHit = true; }
	_bool			Get_IsBoss() { return m_bBoss; }
	MONSTER_TYPE	Get_MstType() { return m_eMstType; }
	Engine::CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

protected:
	virtual HRESULT			Add_Component();
	virtual void			Motion_Change() {};
	virtual void			Face_Camera() {};

	bool			Check_Time(const _float& fTimeDelta);
	bool			Check_Time(const _float& fTimeDelta, float fLimit);
	void			Check_Map_Range();
	void			Hit_PushBack(const _float& fTimeDelta);

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTransform* m_pTargetTransCom;	// 플레이어 COM 객체
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculCom;

	_bool					m_bBoss;

	_bool					m_bHit;
	_bool					m_bDead;
	_int					m_iHp;

	_float					m_fSpeed;
	_float					m_fFrame = 0.f;

	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;
	_float					m_fSecAccTimeDelta;
	_float					m_fSlowDelta;

	_vec3					m_vOriginScale;

	MONSTER_TYPE			m_eMstType;

public:
	static CMonster*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

protected:
	virtual void Free() override;
};

