#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CShell : public CMapObj
{
private:
	explicit CShell(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShell(const CShell& rhs);
	virtual ~CShell();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Hit()								override;

private:

public:
	static CShell* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;


};

