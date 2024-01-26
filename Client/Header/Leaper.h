#pragma once

#include "Monster.h"
#include "GameObject.h"

class CLeaper : public CMonster
{
private:
	explicit CLeaper(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CLeaper(const CLeaper& rhs);
	virtual ~CLeaper();

	enum LEAPERSTATE {LEAPER_IDLE, LEAPER_UP, LEAPER_DOWN, LEAPER_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Change_Dir(const _float& fTimeDelta);
	void				Check_TargetPos();
	void				MoveTo_Random(const _float& fTimeDelta);
	void				JumpTo_Player(const _float& fTimeDelta);

private:
	bool				m_bMove;
	bool				m_bJump;
	int					m_iRandNum;

	float				m_fPower;
	float				m_fAccelTime;

	_vec3				m_vTargetPos;

	LEAPERSTATE			m_eState;

public:
	static CLeaper* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

