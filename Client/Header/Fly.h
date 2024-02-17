#pragma once

#include "Monster.h"
#include "GameObject.h"

class CFly : public CMonster
{
private:
	explicit CFly(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CFly(const CFly& rhs);
	virtual ~CFly();

	enum FLYSTATE {FLY_IDLE, FLY_DEAD, FLY_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;
	virtual void		Face_Camera()						override;

	void				Change_Dir(const _float& fTimeDelta);
	void				Move(const _float& fTimeDelta);
	void				Epic_Time();

private:
	_bool					m_bEpicTime;
	_bool					m_bDeadWait;
	_int					m_iRandNum;
	_int					m_iPicNum;
	_float					m_fFrameSpeed = 1.f;

	_vec3				m_vOriginAngle;

	FLYSTATE			m_ePreState;
	FLYSTATE			m_eCurState;

public:
	static CFly* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void		Free()									override;

};

