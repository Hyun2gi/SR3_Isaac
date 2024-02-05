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
	// ��ü ���� ������踦 �ҷ��� m_mapLevel �� �����صδ� �Լ�
	HRESULT Load_Level_Data();
	
	// ���� ���� ����Ǿ��ִ� �� ������ �ҷ����� �Լ�
	HRESULT Load_Stage_Data();

	// ���� ���������� �������� �ҷ����� �Լ�
	HRESULT Load_Stage_Design_Data();

	// ����� ������ �׸��� Load�ϴ� �Լ�
	HRESULT Load_Connected_Stage_Theme();

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_RoomObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_Door(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

	HRESULT			Door_Collision();

	bool			Check_Cube_Arrived();

public:
	static CLoadStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType);

private:
	virtual void Free() override;

private:
	CLoading* m_pLoading;

	CParticleScatter* pParticleScatter;

	//�ٴڰ� ������ �������ְ� �Ѵ�.
	CFloor* m_pFloor;
	CWall* m_pLeftWall;
	CWall* m_pRightWall;
	CWall* m_pTopWall;
	CWall* m_pBottomWall;

	int m_iCurStageKey;

	bool m_bIsCreated;

	// ���� ��ü�� ������踦 �����ϴ� ��
	map<int, string> m_mapLevel;

	//�ʿ� ��ġ�� ����, ������Ʈ�� �����ϴ� ��
	map<int, LoadObj> m_mapLoadObj;

	// ���� �׸��� Ÿ���� ������ ����
	// �׸�, Ÿ�� ������ ����ȴ�. (0���� �׸�, 1���� Ÿ��)
	vector<string> m_vecStageInfo;

	// �ʿ� ��ġ�� ������ ������ ī��Ʈ���ִ� ����
	vector<int> m_vecMonsterCount;

	// ���� ���������� ������踦 Load�� �����ϴ� ����
	vector<int> m_vecConnectRoom;

	// �� ���⿡ �´� ���� �׸��� �����ϴ� ��
	map<int, string> m_mapDoorTheme;
};

