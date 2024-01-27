#pragma once

#include "Monster.h"
#include "GameObject.h"

class CCharger : public CMonster
{
private:
	explicit CCharger(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCharger(const CCharger& rhs);
	virtual ~CCharger();

	enum CHARGERSTATE {CHARGER_IDLE, CHARGER_ATTACK, CHARGER_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT	Add_Component()							override;

	void			Check_Range();

private:
	CHARGERSTATE	m_eState;

public:
	static CCharger* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;
};

