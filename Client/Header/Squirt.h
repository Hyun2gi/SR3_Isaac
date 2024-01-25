#pragma once

#include "Monster.h"
#include "GameObject.h"

class CSquirt : public CMonster
{
private:
	explicit CSquirt(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSquirt(const CSquirt& rhs);
	virtual ~CSquirt();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Sliding(const _float& fTimeDelta);
	void				Setting_Direction();

private:
	bool				m_bSliding;
	//int					m_iRandNum;
	float				m_fAccel;

public:
	static CSquirt* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free()									override;
};

