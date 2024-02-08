#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CPlayerBullet : public CGameObject
{
private:
	explicit CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerBullet(const CPlayerBullet& rhs);
	virtual ~CPlayerBullet();

	enum IDLEBULLETSTATE {IDLEBULLET_IDLE, IDLEBULLET_EFFECT, IDLEBULLET_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	static CPlayerBullet* Create(LPDIRECT3DDEVICE9	pGraphicDev, const _tchar* pLayerTag);

public:
	void				Set_BulletSpeed(float _speed)
	{
		m_fBulletSpeed = _speed;
	}
	void				Set_BulletCollision()
	{
		m_bCollision = true;
		m_bDead = true;
	}
	bool				Get_BulletState()
	{
		if (m_eCurState == IDLEBULLET_IDLE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool				Get_BulletState()
	{
		if (m_eCurState == IDLEBULLET_IDLE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	virtual HRESULT			Add_Component();
	bool					Check_Time(const _float& fTimeDelta);
	void					Motion_Change();


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
	bool		m_bDeadWait = false;
	bool		m_bCollision = false;

	float		m_fBulletSpeed;
	_int		m_iPicNum;
	float		m_fSpriteSpeed;


	IDLEBULLETSTATE		m_eCurState;
	IDLEBULLETSTATE		m_ePreState;
};

