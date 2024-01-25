#pragma once

#include "Monster.h"
#include "GameObject.h"

class CFly : public CMonster
{
private:
	explicit CFly(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CFly(const CFly& rhs);
	virtual ~CFly();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()					override;

	void				Change_Dir(const _float& fTimeDelta);
	void				Move(const _float& fTimeDelta);

private:
	int					m_iRandNum;

public:
	static CFly* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void		Free()									override;

};

