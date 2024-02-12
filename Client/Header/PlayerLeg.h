#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;
class CBullet;

END


class CPlayerLeg : public Engine::CGameObject
{
private:
	explicit CPlayerLeg(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerLeg(const CPlayerLeg& rhs);
	virtual ~CPlayerLeg();

	enum PLAYER_LEG_ID { P_LEG_IDLE, P_LEG_FRONT, P_LEG_LEFT, P_LEG_RIGHT, P_LEG_END};

public:
	virtual HRESULT Ready_GameObject();
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	virtual HRESULT			Add_Component();
	virtual void			Motion_Change();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CPlayerLeg*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	_float				m_fFrame = 0.f;
	_float				m_iPicNum; // 이미지 개수
	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

	_float				m_fSpriteSpeed; // 스프라이트 변화속도

	PLAYER_LEG_ID		m_ePreLegState;
	PLAYER_LEG_ID		m_eCurLegState;

private:
	virtual void Free() override;
};



