#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CDoor : public CMapObj
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoor(const CDoor& rhs);
	virtual ~CDoor();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

private:
	// º¯¼ö

public:
	static CDoor*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

