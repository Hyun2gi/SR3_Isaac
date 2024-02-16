#pragma once

#include "MapObj.h"
#include "GameObject.h"

#include "Machine.h"
#include "SlotCard.h"

class CSlotMC : public CMapObj
{
private:
	const float SCALAR_X = -0.9f;
	const float SCALAR_Y = 0.1f;
private:
	explicit CSlotMC(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSlotMC(const CSlotMC& rhs);
	virtual ~CSlotMC();

public:
	void				Set_Machine_ToStage(CLayer* pLayer);
	_bool				Get_Game() { return m_bGame; }
	void				Set_Game();
	_bool				Get_Reward() { return m_bReward; }
	void				Set_Reward();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Set_Item_Value()					override;

	void				Create_Machine();
	void				Create_Card();
	_bool				Check_Reward();

private:
	_bool				m_bCreate;
	_bool				m_bGame;
	_bool				m_bReward;
	_bool				m_bMachineSet;

	CMachine*			m_pMachine;
	vector<CSlotCard*>	m_pCardList;

public:
	static CSlotMC*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

