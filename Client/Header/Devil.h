#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CDevil : public CMapObj
{
private:
	explicit CDevil(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDevil(const CMapObj& rhs);
	virtual ~CDevil();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

private:

public:
	static CDevil* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

