#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CEffect :	public Engine::CGameObject
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();
	void			Height_OnTerrain();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	_float				m_fFrame = 0.f;

public:
	static CEffect*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

