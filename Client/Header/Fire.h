#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CFire : public CMapObj
{
private:
	explicit CFire(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFire(const CFire& rhs);
	virtual ~CFire();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

	void			Change_Scale();

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Hit()								override;

public:
	static CFire*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

