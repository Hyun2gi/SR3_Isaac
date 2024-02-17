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
	void			Set_LoadCreateEnd() { m_bIsLoadCreatEnd = true; }

	_bool			Get_Dead() { return m_bDead; }
	void			Set_Dead() { m_bDead = true; }

	_int			Get_HP() { return m_iHp; }
	_bool			Get_Hit() { return m_bHit; }
	void			Hit() { m_bHit = true; }
	void			Hit_False() { m_bHit = false; }

	_bool			Get_IsBoss() { return m_bBoss; }
	MONSTER_TYPE	Get_MstType() { return m_eMstType; }
	BOSS_TYPE		Get_BossType() { return m_eBossType; }

	Engine::CTransform* Get_Transform() { return m_pTransformCom; }

	void			Set_Time_Scale() { m_bTimeScale = !m_bTimeScale; }

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

	void			Hit_PushBack(const _float& fTimeDelta);
	void			Fix_Y(_float fY);
	void			Change_Scale();
	void			Change_Color(const _float& fTimeDelta);
	void			Check_Outof_Map();
	void			Create_Start_Particle(_float fCallLimit);
	void			Rotation_Epic();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTransform* m_pTargetTransCom;	// 플레이어 COM 객체 (Mom's Parts 의 경우 Mom)
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculCom;

	_bool					m_bIsLoadCreatEnd; // 맵 연출 끝났는지 체크

	_bool					m_bCreate;
	_bool					m_bBoss;

	_bool					m_bTimeScale;
	_bool					m_bHit;
	_bool					m_bDead;
	_bool					m_bHitColor;
	_int					m_iHp;

	_float					m_fSpeed;
	_float					m_fFrame = 0.f;

	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;
	_float					m_fSecAccTimeDelta;
	_float					m_fColorTimeDelta;
	_float					m_fSlowDelta;

	_vec3					m_vMoveLook;
	_vec3					m_vOriginScale;

	MONSTER_TYPE			m_eMstType;
	BOSS_TYPE				m_eBossType;

public:
	static CMonster*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

protected:
	virtual void Free() override;
};

