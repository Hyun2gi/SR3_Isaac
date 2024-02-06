#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CCoinFont : public CMapObj
{
private:
	explicit CCoinFont(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCoinFont(const CCoinFont& rhs);
	virtual ~CCoinFont();

public:
	void			Set_State(_int iState);

enum COINSTATE { COIN_3, COIN_5, COIN_15 };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Change_Texture();
	void				Fall_Down();

private:
	COINSTATE			m_eState;

public:
	static CCoinFont*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

