#pragma once

#include "Base.h"
#include "Engine_Define.h"

struct LoadObj
{
	int iType, iIndex;
	_float iX, iY, iZ;
};

struct StageInfo
{
	int m_iKey;
	vector<int> m_vecConnectRoom;
	map<int, LoadObj> m_mapLoadObj;
	string m_strTheme;
	string m_strType;
};


class CStageLoadMgr : public CBase
{
	DECLARE_SINGLETON(CStageLoadMgr)

private:
	explicit CStageLoadMgr();
	virtual ~CStageLoadMgr();

	HRESULT Load_Data();

public:
	HRESULT	Ready_StageLoadMgr();

	string Get_Door_TextureName(_int iNum);

	map<int, StageInfo>& Get_StageInfo() { return m_mapStageInfo; }

public:

private:
	virtual void		Free(void);

private:
	map<int, StageInfo> m_mapStageInfo;
	bool		isFirstLoad = false;
};

