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
	CLoading* m_pLoading;

	//바닥과 벽면을 가지고있게 한다.
	CFloor* m_pFloor;
	CWall* m_pLeftWall;
	CWall* m_pRightWall;
	CWall* m_pTopWall;
	CWall* m_pBottomWall;

	int m_iCurStageKey;
	string m_strCurStageTheme;

	bool m_bIsCreated;

	// 레벨 전체의 연결관계를 저장하는 맵
	map<int, string> m_mapLevel;

	//맵에 설치될 몬스터, 오브젝트를 저장하는 맵
	map<int, LoadObj> m_mapLoadObj;

	// 맵에 배치한 몬스터의 개수를 카운트해주는 벡터
	vector<int> m_vecMonsterCount;

	// 현재 스테이지의 연결관계를 Load해 저장하는 벡터
	vector<int> m_vecConnectRoom;
};

