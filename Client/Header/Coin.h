#pragma once

#include "Item.h"
#include "GameObject.h"

class CCoin : public CItem
{
private:
	explicit CCoin(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCoin(const CCoin& rhs);
	virtual ~CCoin();

	enum COINSTATE
	{
		COIN_IDLE,
		COIN_GET,
		COIN_END
	};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;


private:
	_float					m_fPicNum;

	COINSTATE				m_ePreState;
	COINSTATE				m_eCurState;

	// 코인 반짝 거리는거 자주 못하게 딜레이
	_int					m_iDelay;

public:
	static CCoin* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()									override;

};

