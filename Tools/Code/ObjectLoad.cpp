#include "..\Header\ObjectLoad.h"

IMPLEMENT_SINGLETON(CObjectLoad)

CObjectLoad::CObjectLoad()
{
}

CObjectLoad::~CObjectLoad()
{
	Free();
}

HRESULT CObjectLoad::Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;
    //레벨 데이터를 불러와서 스테이지 목록 저장!
    Load_Level_Data();
    Load_Object_Counts();
    Load_Object_Textures();

    return S_OK;
}

void CObjectLoad::Load_Level_Data()
{
    string strFilePath = "../Dat/MapLevel.dat";

    ifstream fin(strFilePath);

    string strGetLine = "";
    //코드를 한 줄 읽어온다. (스테이지에 대한 정보는 한줄로 저장하게 만들어뒀기 때문에 가능)

    while (getline(fin, strGetLine))
    {
        vector<string> vecStr;
        int iIndex = 0;

        while (true) {
            // , 위치 찾기
            int pos = strGetLine.find_first_of(',', iIndex);

            // ,를 찾지 못하면 종료
            if (pos == string::npos) {
                break;
            }

            // 분리된 문자열 출력
            vecStr.push_back(strGetLine.substr(iIndex, pos - iIndex));
            iIndex = pos + 1;
            vecStr.push_back(strGetLine.substr(iIndex));
        }

        m_mapStage.emplace(pair<int, string>(stoi(vecStr[0]), vecStr[1]));

        vecStr.clear();
    }

    fin.close();
}

void CObjectLoad::Load_Stage_Object(const char* items)
{
    //.dat 파일을 불려올 경로를 설정해준다.
    string strFilePath = "../Dat/";
    string strExtension = ".dat";
    strFilePath = strFilePath + items + strExtension;

    ifstream fin(strFilePath);

    string strKeys = "";
    //코드를 한 줄 읽어온다. (스테이지에 대한 정보는 한줄로 저장하게 만들어뒀기 때문에 가능)
    getline(fin, strKeys);

    fin.close();
}

void CObjectLoad::Load_Object_Counts()
{
    //폴더 내 파일 갯수 읽어와야하는데 잘 안되서, 총 작업할 Object 갯수와 몬스터 갯수를 저장하는 .txt 파일을 직접 만들어서 사용할 예정
    // 파일명 고정, 값에 변동 있을 시 메모작 켜서 직접 수정해야함!!!
    ifstream fin("../Dat/ObjectCount.txt");

    string strLine = "";
    //코드를 한 줄 읽어온다. (스테이지에 대한 정보는 한줄로 저장하게 만들어뒀기 때문에 가능)
    getline(fin, strLine);

    fin.close();

    int iIndex = 0;

    vecObjCounts.reserve(OBJECT_MAX);

    while (true) {
        // , 위치 찾기
        int pos = strLine.find_first_of(',', iIndex);

        // ,를 찾지 못하면 종료
        if (pos == string::npos) {
            vecObjCounts.push_back(stoi(strLine.substr(iIndex)));
            break;
        }

        // 분리된 문자열 출력
        vecObjCounts.push_back(stoi(strLine.substr(iIndex, pos - iIndex)));
        iIndex = pos + 1;
    }
}

void CObjectLoad::Load_Object_Textures()
{
    if (0 < vecObjCounts.size())
    {
        int iIndex = 0;

        for (auto& iter : vecObjCounts)
        {
            //현재 iIndex에 따라 오브젝트 타입을 문자열로 설정해준다
            vector<PDIRECT3DTEXTURE9> vecTemp;
            vecTemp.reserve(iter);

            for (int i = 0; i < iter; ++i)
            {
                string strPath = "../Bin/Resource/Texture/" + Trans_Object_Type_To_String(iIndex) + "/" + Trans_Object_Type_To_String(iIndex) + "_" + to_string(i) + ".png";

                PDIRECT3DTEXTURE9 texture;
                HRESULT hr = D3DXCreateTextureFromFileA(m_pGraphicDev, strPath.c_str(), &texture);

                //D3DSURFACE_DESC my_image_desc;
                //texture->GetLevelDesc(0, &my_image_desc);
                //int width = (int)my_image_desc.Width;
                //int height = (int)my_image_desc.Height;

                vecTemp.push_back(texture);
            }

            m_umapObjectType.emplace(pair<int, vector<PDIRECT3DTEXTURE9>>(iIndex, vecTemp));

            ++iIndex;
        }
    }
}

string CObjectLoad::Trans_Object_Type_To_String(int iValue)
{
    switch (iValue)
    {
    case OBJECTS:
        return "Object";
    case MONSTERS:
        return "Monster";
    case BOSSES:
        return "Boss";
    case ITEMS:
        return "Item";
    default:
        return "";
    }
}

string CObjectLoad::Get_File_Name(int iObjType, int iIndex)
{
    string str = Trans_Object_Type_To_String(iObjType);

    return str + "_" + to_string(iIndex);
}

string CObjectLoad::Get_File_Path(int iObjType, int iIndex)
{
    string str = Trans_Object_Type_To_String(iObjType);

    string strPath = "../Bin/Resource/Texture/" +
        str + "/" +
        str + "_" +
        to_string(iIndex) + ".png";

    return strPath;
}

void CObjectLoad::Free()
{
    vecObjCounts.clear();
    m_mapStage.clear();
    m_umapObjectType.clear();
}
