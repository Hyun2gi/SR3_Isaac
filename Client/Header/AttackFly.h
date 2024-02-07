#pragma once

#include "Monster.h"
#include "GameObject.h"

#include "CenterFly.h"
#include "NormalFly.h"

#include "Layer.h"

class CAttackFly : public CMonster
{
private:
	explicit CAttackFly(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAttackFly(const CAttackFly& rhs);
	virtual ~CAttackFly();

public:
	vector<CNormalFly*>	Get_NormalFlyVec() { return m_NormalFlyList; }
	void				Set_NormalFly_ToStage(CLayer* pLayer);

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Create_AttackFly();
	_bool				Check_Fly_Dead();

private:
	// º¯¼ö
	_bool				m_bCreate;

	CCenterFly*			m_CenterFly;
	vector<CNormalFly*>	m_NormalFlyList;

public:
	static CAttackFly* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

