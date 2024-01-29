#pragma once

#include "Scene.h"

#include "Loading.h"

class CLoadStage : public Engine::CScene
{
private:
	struct LoadObj
	{
		int iType, iIndex, iX, iY, iZ;
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
	// 현재 스테이지의 디자인을 불러오는 함수
	HRESULT Load_Stage_Data(int iType);

	void	Create_GameObject(int iObjType, int iIndex, float x, float y, float z);

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameObject(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

public:
	static CLoadStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int iType);

private:
	virtual void Free() override;

private:
	map<int, string> m_mapLevel;
	map<int, LoadObj> m_mapLoadObj;

	CLoading* m_pLoading;

	int m_iCurStageKey;

	vector<int> m_vecMonsterCount;
};

