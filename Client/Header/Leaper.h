#pragma once

#include "Monster.h"
#include "GameObject.h"

#include "Shadow.h"

class CLeaper : public CMonster
{
	const _float HEIGHT_Y = 0.4f;
	const _float INTERVAL = 3.f;
	const _float SHADOW_SCALE = 0.3f;
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
	virtual void		Motion_Change()						override;
	virtual void		Face_Camera()						override;

	void				Change_Dir(const _float& fTimeDelta);
	void				Check_TargetPos();
	void				MoveTo_Random(const _float& fTimeDelta);
	void				JumpTo_Player(const _float& fTimeDelta);

	void				Create_Shadow();

private:
	_bool				m_bMove;
	_bool				m_bJump;
	_int				m_iRandNum;
	_int				m_iPicNum;
	_float				m_fFrameSpeed = 1.f;
	_float				m_fSoundTimer;

	_float				m_fPower;
	_float				m_fAccelTime;

	_vec3				m_vTargetPos;

	LEAPERSTATE			m_ePreState;
	LEAPERSTATE			m_eCurState;

	CShadow*			m_pShadow;

public:
	static CLeaper* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID);

private:
	virtual void	Free()									override;
};

