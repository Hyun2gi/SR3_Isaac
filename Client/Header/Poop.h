#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CPoop : public CMapObj
{
private:
	explicit CPoop(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPoop(const CPoop& rhs);
	virtual ~CPoop();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual	void		Motion_Change()						override;
	virtual void		Hit()								override;

private:

public:
	static CPoop*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

