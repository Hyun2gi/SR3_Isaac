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
	DECLARE_SINGLETON(CPlayer)

private:
	explicit CPlayer();
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
	virtual HRESULT Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT			Add_Component();

public:
	CComponent*		Get_Component_Player(COMPONENTID eID, const _tchar* pComponentTag)
	{
		auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

		if (iter == m_mapComponent[eID].end())
			return nullptr;

		return iter->second;
	}

	CComponent* Get_Component_Player_Transform()
	{
		auto	iter = find_if(m_mapComponent[ID_DYNAMIC].begin(), m_mapComponent[ID_DYNAMIC].end(), CTag_Finder(L"Proto_Transform"));

		if (iter == m_mapComponent[ID_DYNAMIC].end())
			return nullptr;

		return iter->second;
	}

	// keyblock�� true�� key ������ ����
	void		Set_KeyBlock(_bool keyblock) { m_bKeyBlock = keyblock;}
	void		Set_MoveSpeed(float _movespeed){ m_fMoveSpeed += _movespeed; }
	void		Set_BulletSpeed(float _bulletspeed){ m_fBulletSpeed += _bulletspeed; }
	void		Set_AttackSpeed(float _attackspeed){ m_fAttackSpeed += _attackspeed; }
	void		Set_Hp(float _hp) { if (m_fHp + _hp <= m_fMaxHp){ m_fHp += _hp;} }
	void		Set_To_MaxHp() { m_fHp = m_fMaxHp; }
	void		Set_Coin(int _coin) { m_iCoin += _coin; }
	void		Set_LayerTag(_tchar* pLayerTag) { m_pLayerTag = pLayerTag; }
	void		Set_Bool_MouseYRotation(bool checkY) { m_bMouseYRotataion = checkY; }


	float		Get_BulletSpeed() { return m_fBulletSpeed; }

public:
	void		Bullet_Change_To_Brim();

private:
	void			Key_Input(const _float& fTimeDelta);
	void			Height_OnTerrain();
	_vec3			Picking_OnTerrain();
	void			Motion_Change();
	bool			Check_Time(const _float& fTimeDelta);

private:
	//bullet�� ���� layer �����س��� ����
	_tchar*				m_pLayerTag;
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
	_float				m_fMoveSpeed;

	//�Ѿ� �ӵ�(�Ѿ� �������ؼ�)
	_float				m_fBulletSpeed;

	// �� ������ �ð� ����
	_float				m_fAttackSpeed;

	// �� ��� ������
	_float				m_fShootDelayTime;
	// �ӽ� delay (thumbs_up�� ���)
	_float				m_fDelayTime;

	_float				m_fHp;
	_float				m_fMaxHp;


	int					m_iCoin;

	bool				m_bMouseYRotataion;

public:
	/*static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);*/

private:
	virtual void Free() override;
};

