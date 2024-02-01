#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CDoor : public CMapObj
{
private:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoor(const CDoor& rhs);
	virtual ~CDoor();

	enum DOORTHEMA { THEMA_STAGE, THEMA_BOSS, THEMA_ARCADE, THEMA_GOLD, THEMA_DEVIL, THEMA_END };
	enum DOORSTATE { DOOR_OPEN, DOOR_CLOSE, DOOR_OPENING, DOOR_END };

public:
	void			Set_Thema(int iID);
	void			Set_Open() { m_bOpen = true; }
	_bool			Get_Open() { return m_bOpen; } // 필요 없을 수도
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

	DOORTHEMA			m_eThema;
	
	DOORSTATE			m_ePreState;
	DOORSTATE			m_eCurState;

public:
	static CDoor*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

