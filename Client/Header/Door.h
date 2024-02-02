#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CDoor : public CMapObj
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoor(const CDoor& rhs);
	virtual ~CDoor();

	enum DOORTHEME { THEME_NORMAL, THEME_BOSS, THEME_ARCADE, THEME_GOLD, THEME_DEVIL, THEME_END };
	enum DOORSTATE { DOOR_OPEN, DOOR_CLOSE, DOOR_OPENING, DOOR_END };

public:
	void			Set_Theme(int iID);
	void			Set_Theme(string strStageKey) { m_strStageKey = strStageKey; }
	void			Set_Open() { m_bOpen = true; }
	_bool			Get_Open() { return m_bOpen; } 
	void			Set_Collision() { m_bCollision = true; }
	_bool			Get_Collision() { return m_bCollision; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

private:
	_bool				m_bOpen;
	_bool				m_bCollision;
	string				m_strStageKey;

	DOORTHEME			m_eTheme;
	
	DOORSTATE			m_ePreState;
	DOORSTATE			m_eCurState;

public:
	static CDoor*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

