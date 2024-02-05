#pragma once

#include "Monster.h"
#include "GameObject.h"

class CDople : public CMonster
{
private:
	explicit CDople(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDople(const CDople& rhs);
	virtual ~CDople();

	enum DOPLESTATE { DP_IDLE, DP_WALK, DP_BACK, DP_LEFT, DP_RIGHT, DP_DEAD, DP_END };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;
	virtual void		Face_Camera()						override;

	void				Follow_Player();

private:
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;

	DOPLESTATE			m_ePreState;
	DOPLESTATE			m_eCurState;

public:
	static CDople* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;

};

