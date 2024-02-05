#pragma once

#include "MapObj.h"
#include "GameObject.h"

#include "ShopNpc.h"
#include "Pill.h"
#include "Epic.h"
#include "Heart.h"

#include "Item.h"

class CShop : public CMapObj
{
private:
	const _float INTERVALX = 1.5f;
	const _float INTERVALZ = 2.f;
private:
	explicit CShop(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShop(const CShop& rhs);
	virtual ~CShop();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Create_Obj();

private:
	CShopNpc*			m_pShopNpc;
	
	CPill*				m_pPill;
	CEpic*				m_pEpic;
	CHeart*				m_pHeart;

public:
	static CShop* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

