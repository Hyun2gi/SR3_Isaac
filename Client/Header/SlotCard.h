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
	_int			Get_RewardItem() { return m_iReward; }
	void			Set_RewardResult(_int iResult) { m_iReward = iResult; } // 슬롯머신 랜덤의 결과 값으로 보상 표기

	_bool			Get_Reward() { return m_bReward; }
	void			Set_Reward() { m_bReward = false; } // false가 되어야? 현재 true ---- 몰러

	bool			Get_Random() { return m_bRandom; }
	void			Set_Random() { m_bRandom = true; }
	void			Set_Random_False() { m_bRandom = false; }

	void			Set_Result(_bool bIsTrue) { m_bResult = bIsTrue; }

	void			Set_Index(int iIndex) { m_iIndex = iIndex; }
	void			Set_NoLuckNum(_int iNum) { m_iNoLuckNum = iNum; }

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
	void				Setting_Result();

private:
	_bool				m_bRandom;
	_bool				m_bStart;
	_bool				m_bReward;
	_bool				m_bResult; // 랜덤 휘리릭 끝났고 게임 결과를 보여주는 중


	_int				m_iIndex;
	_int				m_iNoLuckNum; // 꽝 카드
	_int				m_iReward;

	CARDSTATE			m_ePreState;
	CARDSTATE			m_eCurState;

public:
	static CSlotCard* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

