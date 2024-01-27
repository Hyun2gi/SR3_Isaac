#pragma once

#include "Monster.h"
#include "GameObject.h"

class CMonstro : public CMonster
{
private:
	explicit CMonstro(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonstro(const CMonstro& rhs);
	virtual ~CMonstro();

	enum MONSTROSTATE{MONSTRO_IDLE, MONSTRO_ATTACK, MONSTRO_MOVE, MONSTRO_JUMP, MONSTRO_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT	Add_Component()							override;

	void			MoveTo_Player();
	void			JumpTo_Player();
	void			AttackTo_Player();

private:
	// º¯¼ö

public:
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

