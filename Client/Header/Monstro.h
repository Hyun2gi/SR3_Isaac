#pragma once

#include "Monster.h"
#include "GameObject.h"

#include "Layer.h"

class CMonstro : public CMonster
{
	const _float CENTERY = 2.5f;
	const _float ORIGIN_SCALE = 4.f;
private:
	explicit CMonstro(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonstro(const CMonstro& rhs);
	virtual ~CMonstro();

	enum MONSTROSTATE{MONSTRO_IDLE, MONSTRO_ATTACK, MONSTRO_WAIT, MONSTRO_MOVE, MONSTRO_UP, MONSTRO_DOWN, MONSTRO_DEAD, MONSTRO_END};
public:
	void			Print_UI(CLayer* pLayer);

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT	Add_Component()							override;
	virtual void	Motion_Change()							override;
	virtual void	Face_Camera()							override;

	void			MoveTo_Player(const _float& fTimeDelta);
	void			JumpTo_Player(const _float& fTimeDelta);
	void			AttackTo_Player();

	void			Check_TargetPos();
	void			Bullet_Update();
	void			Monstro_Default();

	void			Animation_Attack();
	void			Animation_Dead();

private:
	_bool			m_bDeadWait;
	_bool			m_bJump;
	_bool			m_bBullet;
	_bool			m_bDeadAni;

	_bool			m_bPosChange;
	_bool			m_bScaleReduce;
	_bool			m_bScaleChange;

	_int			m_iPicNum;
	_float			m_fFrameSpeed = 1.f;
	_float			m_fPower;
	_float			m_fAccelTime;
	_float			m_fHitCoolTime; // ÇÇ°Ý ÄðÅ¸ÀÓ(Ç÷»çÆ÷ ´ëÃ¥)

	_vec3			m_vTargetPos;
	_vec3			m_vDeadPos;

	MONSTROSTATE	m_ePreState;
	MONSTROSTATE	m_eCurState;

	list<CGameObject*> m_BulletList;

public:
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

