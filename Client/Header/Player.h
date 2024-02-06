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

	// IDLE : 안걷고 그냥 서있음
	// IDLEWALK : 얼굴방향으로 쏨
	// BACKWALK : 뒷통수보면서 쏨
	// SHOOTWALK : 쏘는 자세 (머리통이 작아짐)
	// GOOD : 따봉
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

	// keyblock이 true면 key 움직임 막힘
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
			// 풀피로 차게
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
	bool		Get_SafeCamer_Area(); // 카메라 벗어나지 않는 안전지대인지
	list<CGameObject*>* Get_Player_BullletList() { return &m_PlayerBulletList; }
	void		Plus_EpicBulletMark(_vec3 pos); 
	void		Clear_EpicBulletMark(); // 씬 이동때마다 바닥에 있는 에픽 흔적 지우기
	void		Change_LastEpicMark_To_Trace(); // 마지막 epic을 trace로 변경

	int			Get_PlayerCurState();
	int			Get_PlayerBulletState();

	// 에픽에투스 타겟상태가 진행되는지
	// 타겟 상태일때(지형선택하고 떨어지기 전까지) 몬스터 느려짐 + 플레이어 이동 제어
	void		Set_EpicTargetRun(bool _run) { m_bEpicTargetRun = _run; }
	bool		Get_EpicTargetRun() { return m_bEpicTargetRun; }

	// 몬스터가 누워야할 타이밍
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
	// 씬이 시작할때 한가운데에 스폰하기 위해서 bool 값으로 씬이 시작하는지 확인
	bool				m_bStartScene;

	//bullet을 위한 layer 저장해놓는 변수
	_tchar*				m_pLayerTag;
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

	//움직이는 속도
	_float				m_fMoveSpeed;

	//총알 속도(총알 설정위해서)
	_float				m_fBulletSpeed;

	// 총 딜레이 시간 설정
	_float				m_fAttackSpeed;

	// 총 쏘는 딜레이
	_float				m_fShootDelayTime;
	// 임시 delay (thumbs_up에 사용)
	_float				m_fDelayTime;

	_float				m_fHp;
	_float				m_fMaxHp;


	int					m_iCoin;

	bool				m_bMouseYRotataion;

	CGameObject*		m_pCamera;

	bool				m_bEpicTargetRun;
	bool				m_bEpicLieTiming;

	// 한번만 초기화하기
	bool				m_bInitialize = false;

private:
	list<CGameObject*>	m_PlayerBulletList;
	list<CGameObject*>	m_EpicMarkList;

public:
	/*static CPlayer*		Create(LPDIRECT3DDEVICE9	pGraphicDev);*/

private:
	virtual void Free() override;
};

