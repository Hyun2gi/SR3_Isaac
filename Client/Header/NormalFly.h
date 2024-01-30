#pragma once

#include "Monster.h"
#include "GameObject.h"

BEGIN(Engine)

class CTransform;

END

class CNormalFly : public CMonster
{
private:
	explicit CNormalFly(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);
	explicit CNormalFly(const CNormalFly& rhs);
	virtual ~CNormalFly();

	enum FLYSTATE{FLY_IDLE, FLY_DEAD, FLY_END};

public:
	void			Set_TargetTransform(Engine::CTransform* pTransformCom) { m_pTargetTransCom = pTransformCom; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

	void			Revolve_Center();

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

private:
	Engine::CTransform* m_pTargetTransCom;

	_int				m_iIndex;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;

	_float				m_fDistance;
	_float				m_fAngle;

	FLYSTATE			m_ePreState;
	FLYSTATE			m_eCurState;

public:
	static CNormalFly* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex);

private:
	virtual void		Free()								override;
};

