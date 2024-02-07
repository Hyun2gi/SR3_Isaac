#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CEpicBullet : public CGameObject
{
private:
	explicit CEpicBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEpicBullet(const CEpicBullet& rhs);
	virtual ~CEpicBullet();

	enum EPICSTATE{ EPIC_TARGET, EPIC_BULLET, EPIC_EFFECT, EPIC_END };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void		Motion_Change();

public:
	static CEpicBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag);

public:
	void					Set_Bullet(int _num)
	{
		switch (_num)
		{
		case 1:
			m_eCurState = EPIC_TARGET;
			break;
		case 2:
			m_eCurState = EPIC_BULLET;
			break;
		}
	}

	// 피격당할때의 bool을 return해줌
	bool					Get_CanAttacked()
	{
		if (m_eCurState == EPIC_BULLET)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void					Set_Shoot(_vec3 shootpos);
	void					Set_StartFall(bool start) { m_bStartFall = start; }

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

	int						m_iPicNum;
	_float					m_fSpriteSpeed;
	_vec3					m_vShootPos;
	_vec3					m_vTargetPos;
	_vec3					m_vBulletSpeed;

	bool					m_bStartFall;
	bool					m_bEndingSetTarget;

private:
	_vec3		m_vBulletDir;
	bool		m_bDead = false;
	EPICSTATE	m_eCurState;
	EPICSTATE	m_ePreState;
};

