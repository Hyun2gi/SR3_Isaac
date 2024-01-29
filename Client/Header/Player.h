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

	// IDLE : �ȰȰ� �׳� ������
	// IDLEWALK : �󱼹������� ��
	// BACKWALK : ��������鼭 ��
	// SHOOTWALK : ��� �ڼ� (�Ӹ����� �۾���)
	// GOOD : ����
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
		// keyblock�� true�� key ������ ����
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
	_float				m_fPicNum; // �̹��� ����
	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

	_float				m_fSpriteSpeed; // ��������Ʈ ��ȭ�ӵ�

	_bool				m_bKeyBlock;

	// �÷��̾� ���� ����
	PLAYERID			m_ePreState;
	PLAYERID			m_eCurState;

	// Bullet ���� ����
	BULLETID			m_eCurBulletState;

	list<CGameObject*>	m_PlayerBulletList;

	//�����̴� �ӵ�
	float				m_fMoveSpeed;

	//�Ѿ� �ӵ�(�Ѿ� �������ؼ�)
	float				m_fBulletSpeed;

	// �� ������ �ð� ����
	float				m_fAttackSpeed;

	// �� ��� ������
	_float				m_fShootDelayTime;
	// �ӽ� delay (thumbs_up�� ���)
	_float				m_fDelayTime;

	int					m_iHp;

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

