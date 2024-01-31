#pragma once

#include "Item.h"
#include "GameObject.h"

class CBrimStone : public CItem
{
	explicit CBrimStone(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBrimStone(const CBrimStone& rhs);
	virtual ~CBrimStone();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

public:
	void			Run_Item_Effect()						override;

private:
	// 랜덤값으로 한 이미지 지정하면 계속 유지
	_float					m_fPicNum;

public:
	static CBrimStone* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()									override;
};