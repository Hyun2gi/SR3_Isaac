#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CEpicBulletMark : public CGameObject
{
private:
	explicit CEpicBulletMark(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEpicBulletMark(const CEpicBulletMark& rhs);
	virtual ~CEpicBulletMark();

	enum EPICMARKSTATE { EPIC_MARK_TARGET, EPIC_MARK_TRACE, EPIC_MARK_END };

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void		Motion_Change();

public:
	static CEpicBulletMark* Create(LPDIRECT3DDEVICE9	pGraphicDev);

public:
	void					Set_Bullet_Mark(int _num)
	{
		switch (_num)
		{
		case 1:
			m_eCurState = EPIC_MARK_TARGET;
			break;
		case 2:
			m_eCurState = EPIC_MARK_TRACE;
			break;
		}
	}

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


private:
	_vec3		m_vBulletDir;
	bool		m_bDead = false;
	EPICMARKSTATE	m_eCurState;
	EPICMARKSTATE	m_ePreState;
};

