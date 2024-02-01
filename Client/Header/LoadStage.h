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
	// ��ü ���� ������踦 �ҷ��� m_mapLevel �� �����صδ� �Լ�
	HRESULT Load_Level_Data();
	
	// ���� ���� ����Ǿ��ִ� �� ������ �ҷ����� �Լ�
	HRESULT Load_Stage_Data();

	// ���� ���������� �������� �ҷ����� �Լ�
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

	//�ٴڰ� ������ �������ְ� �Ѵ�.
	CFloor* m_pFloor;
	CWall* m_pLeftWall;
	CWall* m_pRightWall;
	CWall* m_pTopWall;
	CWall* m_pBottomWall;

	int m_iCurStageKey;
	string m_strCurStageTheme;

	bool m_bIsCreated;

	// ���� ��ü�� ������踦 �����ϴ� ��
	map<int, string> m_mapLevel;

	//�ʿ� ��ġ�� ����, ������Ʈ�� �����ϴ� ��
	map<int, LoadObj> m_mapLoadObj;

	// �ʿ� ��ġ�� ������ ������ ī��Ʈ���ִ� ����
	vector<int> m_vecMonsterCount;

	// ���� ���������� ������踦 Load�� �����ϴ� ����
	vector<int> m_vecConnectRoom;
};

