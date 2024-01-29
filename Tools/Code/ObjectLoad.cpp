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
    //���� �����͸� �ҷ��ͼ� �������� ��� ����!
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
    //�ڵ带 �� �� �о�´�. (���������� ���� ������ ���ٷ� �����ϰ� �����ױ� ������ ����)

    while (getline(fin, strGetLine))
    {
        vector<string> vecStr;
        int iIndex = 0;

        while (true) {
            // , ��ġ ã��
            int pos = strGetLine.find_first_of(',', iIndex);

            // ,�� ã�� ���ϸ� ����
            if (pos == string::npos) {
                break;
            }

            // �и��� ���ڿ� ���
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
    //.dat ������ �ҷ��� ��θ� �������ش�.
    string strFilePath = "../Dat/";
    string strExtension = ".dat";
    strFilePath = strFilePath + items + strExtension;

    ifstream fin(strFilePath);

    string strKeys = "";
    //�ڵ带 �� �� �о�´�. (���������� ���� ������ ���ٷ� �����ϰ� �����ױ� ������ ����)
    getline(fin, strKeys);

    fin.close();
}

void CObjectLoad::Load_Object_Counts()
{
    //���� �� ���� ���� �о�;��ϴµ� �� �ȵǼ�, �� �۾��� Object ������ ���� ������ �����ϴ� .txt ������ ���� ���� ����� ����
    // ���ϸ� ����, ���� ���� ���� �� �޸��� �Ѽ� ���� �����ؾ���!!!
    ifstream fin("../Dat/ObjectCount.txt");

    string strLine = "";
    //�ڵ带 �� �� �о�´�. (���������� ���� ������ ���ٷ� �����ϰ� �����ױ� ������ ����)
    getline(fin, strLine);

    fin.close();

    int iIndex = 0;

    vecObjCounts.reserve(OBJECT_MAX);

    while (true) {
        // , ��ġ ã��
        int pos = strLine.find_first_of(',', iIndex);

        // ,�� ã�� ���ϸ� ����
        if (pos == string::npos) {
            vecObjCounts.push_back(stoi(strLine.substr(iIndex)));
            break;
        }

        // �и��� ���ڿ� ���
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
            //���� iIndex�� ���� ������Ʈ Ÿ���� ���ڿ��� �������ش�
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
