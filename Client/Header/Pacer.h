#pragma once

#include "Monster.h"
#include "GameObject.h"

class CPacer : public CMonster
{
private:
	explicit CPacer(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CPacer(const CPacer& rhs);
	virtual ~CPacer();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT Add_Component()							override;

	void			Change_Dir(const _float& fTimeDelta);
	void			Move(const _float& fTimeDelta);

private:
	int					m_iRandNum;

public:
	static CPacer* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

