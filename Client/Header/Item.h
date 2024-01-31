#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CTexture;
class CRcTex;
class CTransform;
class CCalculator;

END
class CItem : public Engine::CGameObject
{
protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CItem(const CItem& rhs);
	virtual ~CItem();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	virtual void Run_Item_Effect();

protected:
	virtual HRESULT			Add_Component();
	virtual void			Motion_Change() {};

	bool			Check_Time(const _float& fTimeDelta);

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTransform* m_pTargetTransCom;	// 플레이어 COM 객체
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculCom;

	_int					m_iHp;
	_float					m_fSpeed;
	_float					m_fFrame = 0.f;

	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;
	_float					m_fSpriteSpeed;
	bool					m_bDead;

public:
	static CItem* Create(LPDIRECT3DDEVICE9	pGraphicDev);

protected:
	virtual void Free() override;
};

