#include "stdafx.h"
#include "..\Header\StageLoadMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CStageLoadMgr)

CStageLoadMgr::CStageLoadMgr()
{
}


CStageLoadMgr::~CStageLoadMgr()
{
}

HRESULT CStageLoadMgr::Ready_StageLoadMgr()
{
	if(!isFirstLoad)
		FAILED_CHECK_RETURN(Load_Data(), E_FAIL);

	return S_OK;
}

string CStageLoadMgr::Get_Door_TextureName(_int iNum)
{
	return m_mapStageInfo.at(iNum).m_strTheme;
}

HRESULT CStageLoadMgr::Load_Data()
{
	isFirstLoad = true;
	string strFilePath = "../../Dat/MapLevel.dat";
	ifstream fin(strFilePath);

	string strGetLine = "";

	// 저장되어 있는 스테이지의 목록을 임시적으로 저장하는 맵
	map<int, string> mapStageList;

#pragma region 스테이지 목록을 읽어들여옴

	//스테이지 목록을 읽어들여온다.
	while (getline(fin, strGetLine))
	{
		//0번째에 key값, 1번째에 Stage_
		vector<string> vecStr;
		int iIndex = 0;

		while (true) {
			// , 위치 찾기
			int pos = strGetLine.find_first_of(',', iIndex);

			// ,를 찾지 못하면 종료
			if (pos == string::npos)
				break;

			// 분리된 문자열 출력
			vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
			iIndex = pos + 1;
			vecStr.push_back(strGetLine.substr(iIndex));
		}

		mapStageList.emplace(pair<int, string>(stoi(vecStr[0]), vecStr[1]));

		vecStr.clear();
	}

	fin.close();

#pragma endregion

#pragma region 스테이지 목록 전체를 순회하며 연결된 방의 정보를 읽어들여옴

	for (auto& i : mapStageList)
	{
		StageInfo stageInfo;

		//스테이지의 테마와 방 타입에 대한 정보를 저장하는 임시 벡터
		vector<string> vecStageInfo;
		// 스테이지들의 연결 정보를 저장하는 임시 벡터
		vector<int> vecConnectRoom;

		strFilePath = "../../Dat/" + mapStageList.at(i.first) + ".dat";

		ifstream fin(strFilePath);

		string strGetLine = "";

		while (getline(fin, strGetLine))
		{
			int iIndex = 0;

			int iCount = 0;

			while (true) {
				// , 위치 찾기
				int pos = strGetLine.find_first_of(',', iIndex);

				if (3 < iCount)
				{
					vecStageInfo.push_back(strGetLine.substr(iIndex, pos - iIndex));

					// ,를 찾지 못하면 종료
					if (pos == string::npos)
						break;
				}
				else
				{
					// 분리된 문자열 출력
					vecConnectRoom.push_back(stoi(strGetLine.substr(iIndex, pos - iIndex)));
				}

				iIndex = pos + 1;

				iCount++;
			}
		}

		stageInfo.m_iKey = i.first;
		stageInfo.m_strTheme = vecStageInfo[1];
		stageInfo.m_strType = vecStageInfo[0];
		stageInfo.m_vecConnectRoom = vecConnectRoom;

		m_mapStageInfo.emplace(pair<int, StageInfo>({ i.first,stageInfo }));

		fin.close();
	}

	
#pragma endregion

#pragma region 현재 스테이지의 디자인을 불러옴

	for (auto& i : mapStageList)
	{
		strFilePath = "../../Dat/" + mapStageList.at(i.first) + "_Design.dat";

		ifstream fin(strFilePath);

		strGetLine = "";

		int iMapKey = 0;

		map<int, LoadObj> mapLoadObj;

		while (getline(fin, strGetLine))
		{
			vector<string> vecStr;
			int iIndex = 0;

			while (true) {
				// , 위치 찾기
				int pos = strGetLine.find_first_of(',', iIndex);

				// ,를 찾지 못하면 종료
				if (pos == string::npos) {
					vecStr.push_back(strGetLine.substr(iIndex));
					break;
				}

				// 분리된 문자열 출력
				vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
				iIndex = pos + 1;
			}

			LoadObj tTemp = { stoi(vecStr[0]), stoi(vecStr[1]), stof(vecStr[2]), stof(vecStr[3]), stof(vecStr[4]) };

			mapLoadObj.insert({ iMapKey, tTemp });
			//int iType, int iIndex, float x, float y, float z
			vecStr.clear();

			++iMapKey;
		}

		fin.close();

		m_mapStageInfo.at(i.first).m_mapLoadObj = mapLoadObj;
	}

	
#pragma endregion

	return S_OK;
}

void CStageLoadMgr::Free(void)
{
	//for_each(m_mapStageInfo.begin(), m_mapStageInfo.end(), CDeleteMap());
	m_mapStageInfo.clear();
}
