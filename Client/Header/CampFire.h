#pragma once

#include "MapObj.h"
#include "GameObject.h"

#include "Wood.h"
#include "Fire.h"

class CCampFire : public CMapObj
{
private:
	explicit CCampFire(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCampFire(const CCampFire& rhs);
	virtual ~CCampFire();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Create_Wood();
	void				Create_Fire();

private:
	// º¯¼ö
	_bool				m_bCreate;

	CWood*				m_pWood;
	CFire*				m_pFire;

public:
	static CCampFire* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

