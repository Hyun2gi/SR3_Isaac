#pragma once

#include "Scene.h"

#include "Loading.h"

class CFloor;
class CWall;
class CObstacle;

BEGIN(Engine)

class CParticleScatter;

END

class CLoadStage : public Engine::CScene
{
private:
	const float DOOR_X_INTERVAL = 0.6f;
	const float DOOR_Y_INTERVAL = 2.4f;

	struct LoadObj
	{
		int iType, iIndex;
		_float iX, iY, iZ;
	};

	enum Wall_Axis
	{
		WALL_LEFT,
		WALL_RIGHT,
		WALL_TOP,
		WALL_BOTTOM
	};

	enum CUBE_ACTION_TYPE
	{
		RANDOM_CREATE,
		SHAKE_CREATE,
		EXPANSION_CREATE,
		TURN_CREATE,
		SEQUENTIAL_CREATE,
		ACTION_END
	};

private:
	explicit CLoadStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadStage();

public:
	virtual HRESULT Ready_Scene(int iType);
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_RoomObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameMonster(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameItem(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_Door(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT			Door_Collision();

	bool			Check_Cube_Arrived();

	void			Copy_Stage();

	bool			Check_Monster_Dead();
	void			Check_All_Dead();
  
	void			Item_Collision();
	void			Moster_Collision();
	void			MapObj_Collision();

	void			Player_Collision_With_Monster();

	void			Obstacle_Collsion();

	void			Drop_ITem();

	void			Insert_Child();
	void			Setting_UI();

	void			Play_Ending(const _float& fTimeDelta);

	void			Create_Map_Particles();

public:
	static CLoadStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType, bool bStratScene = false);

private:
	virtual void Free() override;

private:
	CLoading*			m_pLoading;

	//바닥과 벽면을 가지고있게 한다.
	CFloor*				m_pFloor;
	CWall*				m_pLeftWall;
	CWall*				m_pRightWall;
	CWall*				m_pTopWall;
	CWall*				m_pBottomWall;

	//현재 스테이지의 방 번호를 저장하는 멤버변수
	int					m_iCurStageKey;

	/* 연출 관련 */
	// 생성되었던 스테이지인지를 판단하는 불변수
	_bool				m_bIsCreated;
	// 로드한 데이터를 전부 생성했는지를 저장하는 불변수
	_bool				m_bIsLoadDataCreated = false;

	// 연출이 필요한 스테이지인지에 대한 값을 저장하는 불변수
	_bool				m_bStartScene;		//true면 연출 제거, false면 연출 있음

	// 몬스터나 오브젝트, 문 등 맵에 Load되어야 하는 것들을 생성할 때 연출을 위해 사용할 타이머
	_float				m_fSpawnTimer = 0.0f;


	/* 메뉴 관련 */
	_bool				m_bMenu;


	/* 엔딩 관련 */
	// 엔딩 넘어가는 타이머
	_float				m_fEndingTimer = 4.5f;
	// 엔딩 타이머를 시작할 때 사용하는 불변수
	_bool				m_bEndingPlay;

	_int				m_iLoadDataSize = 0;
	_int				m_iCreatedCnt = 0;



	// 맵에 배치한 몬스터의 개수를 카운트해주는 벡터
	vector<int>			m_vecMonsterCount;
};

