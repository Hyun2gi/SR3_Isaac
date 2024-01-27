#pragma once

#include "Monster.h"
#include "GameObject.h"

class CSquirt : public CMonster
{
private:
	explicit CSquirt(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSquirt(const CSquirt& rhs);
	virtual ~CSquirt();

	enum SQUIRTSTATE { SQU_IDLE, SQU_SLIDE, SQU_END };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

	void				Sliding(const _float& fTimeDelta);
	void				Check_TargetPos();

private:
	_bool				m_bSliding;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;
	_float				m_fAccel;

	_vec3				m_vTargetPos;
	_vec3				m_vDir;

	SQUIRTSTATE			m_ePreState;
	SQUIRTSTATE			m_eCurState;

public:
	static CSquirt* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;
};

