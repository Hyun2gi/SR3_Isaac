#pragma once

#include "Monster.h"
#include "GameObject.h"

class CAttackFly : public CMonster
{
private:
	explicit CAttackFly(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);
	explicit CAttackFly(const CAttackFly& rhs);
	virtual ~CAttackFly();

	enum FLYID { CENTEROBJ, ATTACKFLY };

public:
	void			Set_CenterObj() { m_iID = CENTEROBJ; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

	void			Revolve_Center();

private:
	virtual HRESULT	Add_Component()							override;

private:
	Engine::CTransform* m_pCenterTransCom;
	Engine::CTransform* m_pTargetTransCom;

	_int				m_iID;
	_int				m_iIndex;

	_float				m_fDistance;
	_float				m_fAngle;

public:
	static CAttackFly* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);

private:
	virtual void		Free()								override;

};

