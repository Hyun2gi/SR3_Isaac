#pragma once

#include "Monster.h"
#include "GameObject.h"

class CMom : public CMonster
{
private:
	explicit CMom(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMom(const CMom& rhs);
	virtual ~CMom();

	enum MOMSTATE{}; // 

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

private:
	// º¯¼ö

public:
	static CMom*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;
};

