#pragma once

#include "Monster.h"
#include "GameObject.h"

#include "Mom.h"

class CMomParts : public CMonster
{
private:
	explicit CMomParts(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
	explicit CMomParts(const CMomParts& rhs);
	virtual ~CMomParts();

	enum PARTSSTATE { MOM_DOOR, MOM_EYE, MOM_SKIN, MOM_HAND, MOM_END };

public:
	void			Set_Mom(CMom* pMom) { m_pMom = pMom; }
	_bool			Get_DoorState();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

	void				Setting_Value();

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

	void				Set_RandNum();
	void				Change_State();

private:
	// º¯¼ö
	_int				m_iRandNum;
	_int				m_iIndex;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;

	PARTSSTATE			m_ePreState;
	PARTSSTATE			m_eCurState;

	CMom*				m_pMom;

public:
	static CMomParts* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);

private:
	virtual void	Free()									override;
};

