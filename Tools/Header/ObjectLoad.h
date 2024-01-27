#pragma once

#include "Base.h"
#include "MyFont.h"
#include "Engine_Define.h"

class CObjectLoad : public CBase
{
	DECLARE_SINGLETON(CObjectLoad)

public:
	const int OBJECT_MAX = 3;
	const float TEXTURE_SIZE = 0.15f;

	struct StageStruct
	{
		string  strStageName;
		int     iKey;
	};

	enum OBJECT_TYPE
	{
		OBJECTS,
		MONSTERS,
		BOSSES
	};

private:
	explicit CObjectLoad();
	virtual ~CObjectLoad();

	void    Load_Level_Data();
	void    Load_Stage_Object(const char* items);
	void    Load_Object_Counts();
	void    Load_Object_Textures();

public:
	HRESULT Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev);

	string Trans_Object_Type_To_String(int iValue);

	map<int, string>& Get_MapStage() { return m_mapStage; }
	vector<int>& Get_VecObjCounts() { return vecObjCounts; }
	unordered_map<int, vector<PDIRECT3DTEXTURE9>>& Get_UmapObjectType() { return m_umapObjectType; }

	string Get_File_Name(int iObjType, int iIndex);
	string Get_File_Path(int iObjType, int iIndex);

public:
	virtual void	Free();

private:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

	map<int, string> m_mapStage;

	vector<int> vecObjCounts;
	unordered_map<int, vector<PDIRECT3DTEXTURE9>> m_umapObjectType;
};
