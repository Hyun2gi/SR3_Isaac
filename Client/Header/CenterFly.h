#pragma once

#include "Monster.h"
#include "GameObject.h"

BEGIN(Engine)

class CTransform;

END

class CCenterFly : public CMonster
{
private:
	explicit CCenterFly(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCenterFly(const CCenterFly& rhs);
	virtual ~CCenterFly();

public:
	Engine::CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

private:
	Engine::CTransform* m_pTargetTransCom;

public:
	static CCenterFly* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

