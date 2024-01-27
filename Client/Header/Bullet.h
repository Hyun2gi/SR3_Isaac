#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CPBullet : public Engine::CGameObject
{
protected:
	explicit CPBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPBullet(const CPBullet& rhs);
	virtual ~CPBullet();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

protected:
	virtual HRESULT			Add_Component();
	bool					Check_Time(const _float& fTimeDelta);

protected:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;


	_float					m_fFrame = 0.f;
	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;
	

public:
	static CPBullet* Create(LPDIRECT3DDEVICE9	pGraphicDev);

protected:
	virtual void Free() override;
};

