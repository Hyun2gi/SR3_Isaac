#pragma once

#include "Scene.h"

#include "Loading.h"

class CFloor;
class CWall;

BEGIN(Engine)

class CParticleScatter;

END

class CLoadStage : public Engine::CScene
{
private:
	const float DOOR_X_INTERVAL = 1.f;
	const float DOOR_Y_INTERVAL = 2.f;

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
	HRESULT			Ready_Layer_GameItem(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_Door(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT			Door_Collision();

	bool			Check_Cube_Arrived();

	void			Check_All_Dead();

	void			Item_Collision();
	void			Moster_Collision();
	void			MapObj_Collision();

	void			Drop_ITem();

	void			Insert_Child();

public:
	static CLoadStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType, bool bStratScene = false);

private:
	virtual void Free() override;

private:
	CLoading* m_pLoading;

	//바닥과 벽면을 가지고있게 한다.
	CFloor* m_pFloor;
	CWall* m_pLeftWall;
	CWall* m_pRightWall;
	CWall* m_pTopWall;
	CWall* m_pBottomWall;

	int m_iCurStageKey;

	bool m_bIsCreated;
	bool m_bStartScene;

	// 맵에 배치한 몬스터의 개수를 카운트해주는 벡터
	vector<int> m_vecMonsterCount;
};

