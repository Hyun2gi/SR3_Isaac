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

    //레벨 데이터를 불러와서 스테이지 목록 저장!
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

    //매 업데이트마다 돌아감
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

    //리스트 박스 클릭 시 실행되는 이벤트(0 = left, 1 = right, 2 = middle)
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

    // 현재까지 지정한 맵 정보를 저장한다.
    if (ImGui::Button("Save"))
    {
        string strDirPath = "../Dat/";
        string strFileName = vecString[m_iSelectedStageIndex] + "_Design";
        string strFileExtension = ".dat";

        strFileName = strDirPath + strFileName + strFileExtension;

        // 파일 스트림을 엽니다.
        ofstream fout(strFileName, ios::binary);

        // 파일에 데이터를 씁니다. Key, Name
        //for_each(m_mapStage.begin(), m_mapStage.end(),
        //    [strFilePath, &fout](auto& iter) {
        //        fout << iter.first << ",";
        //        fout << iter.second << endl;
        //    });

        // 파일 스트림을 닫습니다.
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
                //TODO: 버튼 클릭 시 실행할 함수를 작성
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

//버튼 생성을 위해 불러온다.
void CStageToolGui::Load_Object_Counts()
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

void CStageToolGui::Load_Object_Textures()
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

void CStageToolGui::Load_Level_Data()
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

void CStageToolGui::Load_Stage_Object(const char* items)
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

void CStageToolGui::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
