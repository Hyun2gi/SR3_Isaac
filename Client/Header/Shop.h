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

	void				Create_NPC();
	void				Create_Item();

private:
	CShopNpc*			m_pShopNpc;
	vector<CItem*>		m_ItemVec;

public:
	static CShop* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

