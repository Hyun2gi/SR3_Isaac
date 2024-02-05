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

	enum ITEMSPAWN
	{
		SP_IDLE, // 그냥 있음
		SP_SLOT, // 슬롯 머신에서 나옴 (많이 튀어나옴)
		SP_OBJECT, // OBJECT에서 나옴 (조금 튀어나옴)
		SP_SHOP, // 샵에서 구매
		SP_END
	};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	virtual void	Run_Item_Effect();
	void			Set_Item_SpawnSpot(int _spawnspot);
	virtual void	Item_Spawn_Action();
	void			Set_SpawnPos(_vec3 pos) { m_vSpawnPos = pos; }
	void			Set_LookDir(_vec3 dir) { m_vLookVec = dir; }
	void			Set_MoveSpeed(float speed) { m_fMoveSpeed = speed; }
	CTransform*		Get_Transform() { return m_pTransformCom; }
	bool			Get_Item_Dead() { return m_bDead; }

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
	_int					m_iCoin; // 구매해야할 경우 필요한 코인
	_float					m_fSpeed;
	_float					m_fFrame = 0.f;
	_int					m_iTimer;
	
	_float					m_fMoveSpeed;
	_float					m_fCallLimit;
	_float					m_fAccTimeDelta;
	_float					m_fSpriteSpeed;
	bool					m_bDead;

	ITEMSPAWN				m_eCurItemPlace;
	_vec3					m_vSpawnPos;
	_vec3					m_vLookVec;

public:
	static CItem* Create(LPDIRECT3DDEVICE9	pGraphicDev);

protected:
	virtual void Free() override;
};

