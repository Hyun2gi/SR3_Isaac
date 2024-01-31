#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CMachine : public CMapObj
{
private:
	explicit CMachine(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMachine(const CMachine& rhs);
	virtual ~CMachine();

	enum MCSTATE { MC_IDLE, MC_BROKEN, MC_END };

public:
	void			Set_Game() { m_bGame = true; }
	void			Set_Broken() { m_bBroken = true; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

	void				Change_Scale();

private:
	_bool				m_bGame;
	_bool				m_bBroken;
	
	MCSTATE				m_ePreState;
	MCSTATE				m_eCurState;

public:
	static CMachine*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

