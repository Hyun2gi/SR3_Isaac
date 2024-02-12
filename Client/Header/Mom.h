#pragma once

#include "Monster.h"
#include "GameObject.h"

class CMom : public CMonster
{
	const _float LIMIT_Y = 6.1f; // 6.3
	const _float ORIGIN_SCALE = 10.f;
private:
	explicit CMom(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMom(const CMom& rhs);
	virtual ~CMom();

	enum MOMSTATE{MOM_IDLE, MOM_ATTACK, MOM_WAIT, MOM_UP, MOM_END };

public:
	void			Set_Hp_Minus() { m_iHp -= 1; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;
	virtual void		Face_Camera()						override;

	void				Scale_Change();
	void				Attack(const _float& fTimeDelta);
	void				Mom_Default();

	void				Animation_Attack();
	void				Anomation_Up();

private:
	_bool				m_bScaleReduceX;
	_bool				m_bScaleReduceY;
	_bool				m_bScaleChange;
	_int				m_iScaleCountX;
	_int				m_iScaleCountY;

	_bool				m_bReduce;
	_int				m_iRandNum;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;

	MOMSTATE			m_eState;

public:
	static CMom*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;
};

