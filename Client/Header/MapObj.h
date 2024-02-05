#pragma once

#include "Base.h"
#include "GameObject.h"

#include "Item.h"
#include "Layer.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCalculator;

END

class CMapObj : public Engine::CGameObject
{
protected:
	explicit CMapObj(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapObj(const CMapObj& rhs);
	virtual ~CMapObj();

	enum MAPOBJID{MOBJ_POOP, MOBJ_FIRE, MOBJ_SPIKE, MOBJ_SLOTMC, };

public:
	_bool			Get_Dead() { return m_bDead; }
	_bool			Get_Drop() { return m_bItemDrop; }
	void			Set_Drop() { m_bItemDrop = true; }
	void			Set_Hit() { m_bHit = true; }

	ITEM_TYPE		Get_ItemType() { return m_eDropItem; }
	wstring			Get_DropItemTag() { return m_wstrDropItemTag; }

	Engine::CTransform* Get_TransformCom() { return m_pTransformCom; }

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

	CItem*			Create_Item(ITEM_TYPE eItemType, _int iSpawnPos, CLayer* pLayer);

protected:
	virtual HRESULT			Add_Component();
	virtual void			Motion_Change() {};
	virtual void			Hit() {};
	virtual void			Set_Item_Value() {};

	bool					Check_Time(const _float& fTimeDelta);
	void					Setting_ItemTag();

protected:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTransform*		m_pTargetTransCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculator;

	_bool					m_bHit;
	_bool					m_bDead;
	_bool					m_bItemDrop;
	
	_int					m_iHitCount;
	_int					m_iLimitHit;

	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;

	_int					m_iPicNum;
	_float					m_fFrame = 0.f;
	_float					m_fFrameSpeed;

	ITEM_TYPE				m_eDropItem;
	wstring					m_wstrDropItemTag;

public:
	static CMapObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void	Free() override;

};

