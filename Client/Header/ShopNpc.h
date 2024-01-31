#pragma once

#include "MapObj.h"
#include "GameObject.h"

class CShopNpc : public CMapObj
{
private:
	explicit CShopNpc(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShopNpc(const CShopNpc& rhs);
	virtual ~CShopNpc();

	enum NPCSTATE { NPC_IDLE, NPC_GOOD, NPC_DEAD, NPC_END };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual	void		Motion_Change()						override;
	virtual void		Hit()								override;

private:
	NPCSTATE			m_ePreState;
	NPCSTATE			m_eCurState;

public:
	static CShopNpc*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};

