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
	void			Run_Item_Effect();

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;


private:
	// 랜덤값으로 한 이미지 지정하면 계속 유지
	_int				m_iPicNum;

	PILLSTATE			m_eCurState;

public:
	static CPill* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()									override;

};

