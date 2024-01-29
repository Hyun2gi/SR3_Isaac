#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CBrimStoneBullet : public CGameObject
{
private:
	explicit CBrimStoneBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBrimStoneBullet(const CBrimStoneBullet& rhs);
	virtual ~CBrimStoneBullet();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	static CBrimStoneBullet* Create(LPDIRECT3DDEVICE9	pGraphicDev, const _tchar* pLayerTag);

private:
	virtual HRESULT			Add_Component();
	bool					Check_Time(const _float& fTimeDelta);


private:
	virtual void Free();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;


	_float					m_fFrame = 0.f;
	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;


private:
	_vec3		m_vBulletDir;
	bool		m_bDead = false;
};

