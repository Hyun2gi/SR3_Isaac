#pragma once

#include "MapObj.h"
#include "Spike.h"

class CSpike : public CMapObj
{
private:
	explicit CSpike(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSpike(const CSpike& rhs);
	virtual ~CSpike();

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
	static CSpike*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};