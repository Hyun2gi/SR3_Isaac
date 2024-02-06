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
	enum PLAYERID { P_IDLE, P_IDLEWALK, P_BACKWALK, P_LEFTWALK, P_RIGHTWALK, P_SHOOTWALK,  P_THUMBS_UP, P_GET_GOOD_ITEM, P_END  };
	
	enum BULLETID
	{	P_BULLET_IDLE, P_BULLET_BRIMSTONE, P_BULLET_EPIC, P_BULLET_END};

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

	Engine::CRcTex* Get_Component_Player_TexBuffer()
	{
		return m_pBufferCom;
	}

	// keyblock�� true�� key ������ ����
	void		Set_KeyBlock(_bool keyblock) { m_bKeyBlock = keyblock;}
	void		Set_MoveSpeed(float _movespeed){ m_fMoveSpeed += _movespeed; }
	void		Set_BulletSpeed(float _bulletspeed){ m_fBulletSpeed += _bulletspeed; }
	void		Set_AttackSpeed(float _attackspeed){ m_fAttackSpeed += _attackspeed; }
	void		Set_Hp(float _hp) 
	{ 
		if (m_fHp + _hp <= m_fMaxHp)
		{ 
			m_fHp += _hp;
		}
		else
		{
			// Ǯ�Ƿ� ����
			m_fHp = m_fMaxHp;
		}
	}



	void		Set_To_MaxHp() { m_fHp = m_fMaxHp; }
	void		Set_Coin(int _coin) { m_iCoin += _coin; }
	void		Set_LayerTag(_tchar* pLayerTag) { m_pLayerTag = pLayerTag; }
	void		Set_Bool_MouseYRotation(bool checkY) { m_bMouseYRotataion = checkY; }
	void		Set_Player_Pos(_vec3 pos);
	void		Set_Hp_To_MaxHp() { m_fHp = m_fMaxHp; }

	float		Get_MaxHp() { return m_fMaxHp; }
	float		Get_Hp() { return m_fHp; }
	float		Get_BulletSpeed() { return m_fBulletSpeed; }
	int			Get_Coin() { return m_iCoin; }
	void		Set_Bool_StartScene(bool _start) { m_bStartScene = _start; }
	void		Set_StartPosition(_vec3 _position);
	void		Set_MouseRotation(float xRad, float yRad);
	void		Set_BulletType(int _bullet);
	void		Set_EpicFall();


	void		Set_Item_Get_Anim();
	void		Set_Camera(CGameObject* _cam) { m_pCamera = _cam; }

	bool		Get_Camera_WallBlock();
	bool		Get_SafeCamer_Area(); // ī�޶� ����� �ʴ� ������������
	list<CGameObject*>* Get_Player_BullletList() { return &m_PlayerBulletList; }
	void		Plus_EpicBulletMark(_vec3 pos); 
	void		Clear_EpicBulletMark(); // �� �̵������� �ٴڿ� �ִ� ���� ���� �����
	void		Change_LastEpicMark_To_Trace(); // ������ epic�� trace�� ����

	int			Get_PlayerCurState();
	int			Get_PlayerBulletState();

	// ���ȿ����� Ÿ�ٻ��°� ����Ǵ���
	// Ÿ�� �����϶�(���������ϰ� �������� ������) ���� ������ + �÷��̾� �̵� ����
	void		Set_EpicTargetRun(bool _run) { m_bEpicTargetRun = _run; }
	bool		Get_EpicTargetRun() { return m_bEpicTargetRun; }

	// ���Ͱ� �������� Ÿ�̹�
	bool		Get_EpicLieTiming() { return m_bEpicLieTiming; }
	bool		Set_EpicLieTiming(bool timing) { m_bEpicLieTiming = timing; }

public:
	void		Bullet_Change_To_Brim();

private:
	void			Key_Input(const _float& fTimeDelta);
	void			Height_OnTerrain();
	_vec3			Picking_OnTerrain();
	void			Motion_Change();
	bool			Check_Time(const _float& fTimeDelta);

private:
	// ���� �����Ҷ� �Ѱ���� �����ϱ� ���ؼ� bool ������ ���� �����ϴ��� Ȯ��
	bool				m_bStartScene;

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

	CGameObject*		m_pCamera;

	bool				m_bEpicTargetRun;
	bool				m_bEpicLieTiming;

	// �ѹ��� �ʱ�ȭ�ϱ�
	bool				m_bInitialize = false;

private:
	list<CGameObject*>	m_PlayerBulletList;
	list<CGameObject*>	m_EpicMarkList;

public:
	/*static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);*/

private:
	virtual void Free() override;
};

