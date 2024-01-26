#pragma once
#include "StageToolGui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "Export_System.h"

#include "StageTool.h"


CStageToolGui::CStageToolGui()
{
}

CStageToolGui::CStageToolGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    Ready_ImGuiTools(hWnd, pGraphicDev);
}

CStageToolGui::~CStageToolGui()
{
    Free();
}

HRESULT CStageToolGui::Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_bIsOpend = false;
    m_iSelectedStageIndex = 0;

    m_pGraphicDev = pGraphicDev;

    //���� �����͸� �ҷ��ͼ� �������� ��� ����!
    Load_Level_Data();
    //
    Load_Object_Counts();
    Load_Object_Textures();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    return S_OK;
}

void CStageToolGui::Update_ImGuiTools()
{
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int iListSize = m_mapStage.size();
    vector<string> vecString(iListSize);

    int i = 0;

    for_each(m_mapStage.begin(), m_mapStage.end(),
        [&i, &vecString](auto& iter) {
            vecString[i] = iter.second;
            ++i;
        });

    vector<const char*> items;

    if (0 < vecString.size()) {
        for (const string& str : vecString) {
            items.push_back(str.c_str());
        }
    }

    //�� ������Ʈ���� ���ư�
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("StageTool");                          // Create a window called "Hello, world!" and append into it.

    static CHAR szSelectedStage[MAX_PATH] = "";

    ImGui::Text("Stage List");

    ImGui::SetNextItemWidth(100.f);
    ImGui::BeginListBox("##");
    if (0 < vecString.size())
        ImGui::ListBox("##", &m_iSelectedStageIndex, items.data(), vecString.size());
    ImGui::EndListBox();

    //����Ʈ �ڽ� Ŭ�� �� ����Ǵ� �̺�Ʈ(0 = left, 1 = right, 2 = middle)
    if (ImGui::IsItemClicked())
    {
        m_bIsOpend = true;
    }

    if (m_bIsOpend)
    {

        //Load_Stage_Object(vecString[m_iSelectedStageIndex].c_str());
    }

    ImGui::NewLine();
    ImGui::Text("Position: ");
    ImGui::Text("x: %f", m_vecPickingPos.x);
    ImGui::Text("y: %f", m_vecPickingPos.y);
    ImGui::Text("z: %f", m_vecPickingPos.z);

    // ������� ������ �� ������ �����Ѵ�.
    if (ImGui::Button("Save"))
    {
        string strDirPath = "../Dat/";
        string strFileName = vecString[m_iSelectedStageIndex] + "_Design";
        string strFileExtension = ".dat";

        strFileName = strDirPath + strFileName + strFileExtension;

        // ���� ��Ʈ���� ���ϴ�.
        ofstream fout(strFileName, ios::binary);

        // ���Ͽ� �����͸� ���ϴ�. Key, Name
        //for_each(m_mapStage.begin(), m_mapStage.end(),
        //    [strFilePath, &fout](auto& iter) {
        //        fout << iter.first << ",";
        //        fout << iter.second << endl;
        //    });

        // ���� ��Ʈ���� �ݽ��ϴ�.
        fout.close();
    }

    ImGui::End();

    Popup_Object_Gui();

}

void CStageToolGui::Render_ImGuiTools()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CStageToolGui::Popup_Object_Gui()
{
    ImGui::Begin("Object Placement");

    ImGui::Text("Please select an object to place.");

    Create_Image_Buttons();


    //ImGui::BeginListBox("##");
    //if (0 < vecString.size())
    //    ImGui::ListBox("##", &m_iSelectedStageIndex, items.data(), vecString.size());
    //ImGui::EndListBox();

    ImGui::End();
}

void CStageToolGui::Create_Image_Buttons()
{
    static int iTest = 0;

    for (auto& item : m_umapObjectType)
    {
        ImGui::NewLine();

        string str = Trans_Object_Type_To_String(item.first);
        ImGui::Text("%s", str.c_str());

        int i = 0;

        for (auto& image : item.second)
        {
            string strButtonName = str + to_string(i);

            D3DSURFACE_DESC my_image_desc;
            image->GetLevelDesc(0, &my_image_desc);
            int width = my_image_desc.Width * TEXTURE_SIZE;
            int height = my_image_desc.Height * TEXTURE_SIZE;

            if (ImGui::ImageButton(strButtonName.c_str(), (void*)image, ImVec2(width, height)))
            {
                //TODO: ��ư Ŭ�� �� ������ �Լ��� �ۼ�
                m_pTargetScene->Set_Cursor_Image(image);
            }
            ImGui::SameLine();
        }
    }

    ImGui::Text("%i", iTest);
}

string CStageToolGui::Trans_Object_Type_To_String(int iValue)
{
    switch (iValue)
    {
    case OBJECTS:
        return "Object";
    case MONSTERS:
        return "Monster";
    case BOSSES:
        return "Boss";
    default:
        return "";
    }
}

//��ư ������ ���� �ҷ��´�.
void CStageToolGui::Load_Object_Counts()
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

void CStageToolGui::Load_Object_Textures()
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

void CStageToolGui::Load_Level_Data()
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

void CStageToolGui::Load_Stage_Object(const char* items)
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

void CStageToolGui::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
