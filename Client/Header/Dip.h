#pragma once

#include "Monster.h"
#include "GameObject.h"

class CDip : public CMonster
{
private:
	explicit CDip(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CDip(const CDip& rhs);
	virtual ~CDip();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;	

private:
	virtual HRESULT		Add_Component()						override;

	void				Change_Dir();
	void				Sliding(const _float& fTimeDelta);

private:
	bool				m_bSliding;
	int					m_iRandNum;
	float				m_fAccel;

public:
	static CDip* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

