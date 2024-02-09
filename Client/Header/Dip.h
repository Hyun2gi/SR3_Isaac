#pragma once

#include "Monster.h"
#include "GameObject.h"

class CDip : public CMonster
{
	const _float HEIGHT_Y = 1.f;
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
	virtual void		Face_Camera()						override;

	void				Change_Dir();
	void				Sliding(const _float& fTimeDelta);
	void				Epic_Time();

private:
	_bool				m_bSliding;
	_bool				m_bEpicTime;
	_int				m_iRandNum;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;
	_float				m_fAccel;

	_vec3				m_vOriginAngle;

	DIPSTATE			m_ePreState;
	DIPSTATE			m_eCurState;

public:
	static CDip* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

