#pragma once

#include "Item.h"
#include "GameObject.h"

class CPill : public CItem
{
private:
	explicit CPill(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPill(const CPill& rhs);
	virtual ~CPill();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;


private:
	// 랜덤값으로 한 이미지 지정하면 계속 유지
	_float					m_fPicNum;

public:
	static CPill* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()									override;

};

