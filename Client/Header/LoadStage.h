#pragma once

#include "Scene.h"

#include "Loading.h"

class CFloor;
class CWall;

class CLoadStage : public Engine::CScene
{
private:
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
	// 전체 맵의 연결관계를 불러서 m_mapLevel 에 저장해두는 함수
	HRESULT Load_Level_Data();
	
	// 현재 맵의 연결되어있는 방 정보를 불러오는 함수
	HRESULT Load_Stage_Data();

	// 현재 스테이지의 디자인을 불러오는 함수
	HRESULT Load_Stage_Design_Data();

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_RoomObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

	bool Check_Cube_Arrived();

public:
	static CLoadStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType);

private:
	virtual void Free() override;

private:
	map<int, string> m_mapLevel;
	map<int, LoadObj> m_mapLoadObj;

	CLoading* m_pLoading;

	//바닥과 벽면을 가지고있게 한다.
	CFloor* m_pFloor;
	CWall* m_pLeftWall;
	CWall* m_pRightWall;
	CWall* m_pTopWall;
	CWall* m_pBottomWall;

	int m_iCurStageKey;

	bool m_bIsCreated;

	vector<int> m_vecMonsterCount;
	vector<int> m_vecConnectRoom;
};

