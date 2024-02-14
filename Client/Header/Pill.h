#pragma once

#include "Item.h"
#include "GameObject.h"

class CPill : public CItem
{
private:
	explicit CPill(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPill(const CPill& rhs);
	virtual ~CPill();

	enum PILLSTATE
	{
		PILL_0,
		PILL_1,
		PILL_2,
		PILL_3,
		PILL_4,
		PILL_END
	};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

public:
	bool			Run_Item_Effect()						override;
	void			Item_Spawn_Action()						override;
	int				Get_Pill_Num() { return m_iPillEffect; }

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;


private:
	// 랜덤값으로 한 이미지 지정하면 계속 유지
	bool				m_bBadItem;
	_int				m_iPicNum;
	_int				m_iUpTimer;
	_int				m_iPillEffect;

	PILLSTATE			m_eCurState;


public:
	static CPill* Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look);

private:
	virtual void		Free()									override;

};

