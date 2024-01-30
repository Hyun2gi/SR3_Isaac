#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
// class CCalculator; // 필요할 지?

END

class CMapObj : public Engine::CGameObject
{
protected:
	explicit CMapObj(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapObj(const CMapObj& rhs);
	virtual ~CMapObj();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

protected:
	virtual HRESULT			Add_Component();
	virtual void			Motion_Change() {};
	virtual void			Hit() {};

protected:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTransform*		m_pTargetTransCom;
	Engine::CTexture*		m_pTextureCom;
	
	_int					m_iHitCount;
	_int					m_iLimitHit;

	_float					m_fFrame = 0.f;

public:
	static CMapObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void	Free() override;

};

