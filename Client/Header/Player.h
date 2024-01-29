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

class CPlayer :	public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

	// IDLE : 안걷고 그냥 서있음
	// IDLEWALK : 얼굴방향으로 쏨
	// BACKWALK : 뒷통수보면서 쏨
	// SHOOTWALK : 쏘는 자세 (머리통이 작아짐)
	// GOOD : 따봉
	enum PLAYERID { P_IDLE, P_IDLEWALK, P_BACKWALK, P_LEFTWALK, P_RIGHTWALK, P_SHOOTWALK, P_THUMBS_UP, P_END  };
	
	enum BULLETID
	{	P_BULLET_IDLE, P_BULLET_BRIMSTONE,P_BULLET_END};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

public:
	void		Set_KeyBlock(_bool keyblock)
	{
		// keyblock이 true면 key 움직임 막힘
		m_bKeyBlock = keyblock;
	}

	void		Set_MoveSpeed(float _movespeed)
	{
		m_fMoveSpeed += _movespeed;
	}

	void		Set_BulletSpeed(float _bulletspeed)
	{
		m_fBulletSpeed += _bulletspeed;
	}

	void		Set_AttackSpeed(float _attackspeed)
	{
		m_fBulletSpeed -= _attackspeed;
	}

	void		Set_Hp(int _hp)
	{
		m_iHp += _hp;
	}

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Height_OnTerrain();
	_vec3			Picking_OnTerrain();
	void			Motion_Change();
	bool			Check_Time(const _float& fTimeDelta);

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	_float				m_fFrame = 0.f;
	_float				m_fPicNum; // 이미지 개수
	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

	_float				m_fSpriteSpeed; // 스프라이트 변화속도

	_bool				m_bKeyBlock;

	// 플레이어 상태 저장
	PLAYERID			m_ePreState;
	PLAYERID			m_eCurState;

	// Bullet 상태 저장
	BULLETID			m_eCurBulletState;

	list<CGameObject*>	m_PlayerBulletList;

	//움직이는 속도
	float				m_fMoveSpeed;

	//총알 속도(총알 설정위해서)
	float				m_fBulletSpeed;

	// 총 딜레이 시간 설정
	float				m_fAttackSpeed;

	// 총 쏘는 딜레이
	_float				m_fShootDelayTime;
	// 임시 delay (thumbs_up에 사용)
	_float				m_fDelayTime;

	int					m_iHp;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

