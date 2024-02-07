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

	// ����Ǿ� �ִ� ���������� ����� �ӽ������� �����ϴ� ��
	map<int, string> mapStageList;

#pragma region �������� ����� �о�鿩��

	//�������� ����� �о�鿩�´�.
	while (getline(fin, strGetLine))
	{
		//0��°�� key��, 1��°�� Stage_
		vector<string> vecStr;
		int iIndex = 0;

		while (true) {
			// , ��ġ ã��
			int pos = strGetLine.find_first_of(',', iIndex);

			// ,�� ã�� ���ϸ� ����
			if (pos == string::npos)
				break;

			// �и��� ���ڿ� ���
			vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
			iIndex = pos + 1;
			vecStr.push_back(strGetLine.substr(iIndex));
		}

		mapStageList.emplace(pair<int, string>(stoi(vecStr[0]), vecStr[1]));

		vecStr.clear();
	}

	fin.close();

#pragma endregion

#pragma region �������� ��� ��ü�� ��ȸ�ϸ� ����� ���� ������ �о�鿩��

	for (auto& i : mapStageList)
	{
		StageInfo stageInfo;

		//���������� �׸��� �� Ÿ�Կ� ���� ������ �����ϴ� �ӽ� ����
		vector<string> vecStageInfo;
		// ������������ ���� ������ �����ϴ� �ӽ� ����
		vector<int> vecConnectRoom;

		strFilePath = "../../Dat/" + mapStageList.at(i.first) + ".dat";

		ifstream fin(strFilePath);

		string strGetLine = "";

		while (getline(fin, strGetLine))
		{
			int iIndex = 0;

			int iCount = 0;

			while (true) {
				// , ��ġ ã��
				int pos = strGetLine.find_first_of(',', iIndex);

				if (3 < iCount)
				{
					vecStageInfo.push_back(strGetLine.substr(iIndex, pos - iIndex));

					// ,�� ã�� ���ϸ� ����
					if (pos == string::npos)
						break;
				}
				else
				{
					// �и��� ���ڿ� ���
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

#pragma region ���� ���������� �������� �ҷ���

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
				// , ��ġ ã��
				int pos = strGetLine.find_first_of(',', iIndex);

				// ,�� ã�� ���ϸ� ����
				if (pos == string::npos) {
					vecStr.push_back(strGetLine.substr(iIndex));
					break;
				}

				// �и��� ���ڿ� ���
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
