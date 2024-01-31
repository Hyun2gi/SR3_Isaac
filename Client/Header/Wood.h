#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CWood : public CMapObj
{
private:
	explicit CWood(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWood(const CWood& rhs);
	virtual ~CWood();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

public:
	static CWood*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

