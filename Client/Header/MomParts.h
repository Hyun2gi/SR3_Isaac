#pragma once

#include "Monster.h"
#include "GameObject.h"

#include "Layer.h"

#include "Mom.h"

class CMomParts : public CMonster
{
	const _float INTERVAL = 1.f;
	const _float INTERVAL_Y = 4.2f;
	const _float ORIGIN_SCALE_X = 7.f;
private:
	explicit CMomParts(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
	explicit CMomParts(const CMomParts& rhs);
	virtual ~CMomParts();

	enum PARTSSTATE { MOM_DOOR, MOM_EYE, MOM_SKIN, MOM_HAND, MOM_END };

public:
	CMom*			Get_Mom() { return m_pMom; }
	void			Set_Mom(CMom* pMom) { m_pMom = pMom; }
	void			Set_Layer(CLayer* pLayer) { m_pLayer = pLayer; }

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
	void				Animation_Change();

	void				Attack_CreateMst();
	void				Check_CreateMst();
	void				Create_Mst(_vec3 vPos);

private:
	_bool				m_bScaleReduce;
	_bool				m_bScaleChange;
	_bool				m_bMstCreate;
	_int				m_iScaleCount;

	_int				m_iRandNum;
	_int				m_iRandNumMstCreate; // 몬스터 생성 난수
	_int				m_iIndex;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;

	PARTSSTATE			m_ePreState;
	PARTSSTATE			m_eCurState;

	CMom*				m_pMom;
	CLayer*				m_pLayer;

public:
	static CMomParts* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);

private:
	virtual void	Free()									override;
};

