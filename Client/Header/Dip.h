#pragma once

#include "Monster.h"
#include "GameObject.h"

class CDip : public CMonster
{
private:
	explicit CDip(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CDip(const CDip& rhs);
	virtual ~CDip();

	enum DIPSTATE {DIP_IDLE, DIP_SLIDE, DIP_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;	

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

	void				Change_Dir();
	void				Sliding(const _float& fTimeDelta);

private:
	_bool				m_bSliding;
	_int				m_iRandNum;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;
	_float				m_fAccel;

	DIPSTATE			m_ePreState;
	DIPSTATE			m_eCurState;

public:
	static CDip* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

