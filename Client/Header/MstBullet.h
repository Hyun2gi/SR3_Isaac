#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CMstBullet : public CGameObject
{
private:
	explicit CMstBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMstBullet(const CMstBullet& rhs);
	virtual ~CMstBullet();

public:
	void	Set_Dir(_vec3& vDir)
	{
		m_vBulletDir = vDir;
	}

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	static CMstBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual HRESULT			Add_Component();
	bool					Check_Time(const _float& fTimeDelta);


private:
	virtual void Free();

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	_float					m_fFrame = 0.f;
	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;

private:
	bool				m_bDead = false;

	_vec3				m_vBulletDir;
};

