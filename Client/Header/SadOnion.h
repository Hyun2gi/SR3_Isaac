#pragma once

#include "Item.h"
#include "GameObject.h"

class CSadOnion : public CItem
{
	explicit CSadOnion(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSadOnion(const CSadOnion& rhs);
	virtual ~CSadOnion();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;


private:
	// ���������� �� �̹��� �����ϸ� ��� ����
	_float					m_fPicNum;

public:
	static CSadOnion* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()									override;
};

