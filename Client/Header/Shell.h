#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CShell : public CMapObj
{
private:
	explicit CShell(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShell(const CShell& rhs);
	virtual ~CShell();

public:
	void			Set_StartUp() { m_bStartUp = !m_bStartUp; }
	_bool			Get_Shaking_Ready() { return m_bShaking_Ready; }
	void			Set_Shaking_Ready() { m_bShaking_Ready = !m_bShaking_Ready; }

	_bool			Get_Reward() { return m_bReward; }
	void			Set_Reward(_bool bISReward) { m_bReward = bISReward; }
	_bool			Get_Lose() { return m_bLose; }
	void			Set_Lose() { m_bLose = true; }
	void			Set_Lose_False() { m_bLose = false; }
	//void			Setting_Reward() { m_bReward = true;}
	//void			Setting_Reward_False() { m_bReward = false; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Hit()								override;

	void				Start_Up(const _float& fTimeDelta);

private:
	_float				m_fSpeed;

	_bool				m_bStartUp;
	_bool				m_bMoveDown;
	_bool				m_bShaking_Ready;
	_bool				m_bReward;
	_bool				m_bLose;

public:
	static CShell*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

