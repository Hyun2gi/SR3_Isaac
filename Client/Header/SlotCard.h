#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CSlotCard : public CMapObj
{
	explicit CSlotCard(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSlotCard(const CSlotCard& rhs);
	virtual ~CSlotCard();

	enum CARDSTATE { CARD_IDLE, CARD_RANDOM, CARD_END };

public:
	void			Set_Random() { m_bRandom = true; }
	void			Set_Index(int iIndex) { m_iIndex = iIndex; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

	void				Change_Scale();
	void				Setting_FirstCard();
	void				Check_Result();


private:
	_bool				m_bRandom;
	_bool				m_bStart;
	_int				m_iIndex;

	CARDSTATE			m_ePreState;
	CARDSTATE			m_eCurState;

public:
	static CSlotCard* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

