#pragma once

#include "MapObj.h"
#include "MapObj.h"

class CShadow : public CMapObj
{
private:
	explicit CShadow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShadow(const CShadow& rhs);
	virtual ~CShadow();

public:
	void			Set_Render(_bool bIsRender) { m_bRender = bIsRender; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

private:
	_bool				m_bRender;

	

public:
	static CShadow*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

