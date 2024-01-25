#pragma once
#include "StageToolGui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "Export_System.h"


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

            // 다음 구분자 위치를 시작 위치로 설정
        }

        m_mapStage.emplace(pair<int, string>(stoi(vecStr[0]),  vecStr[1]));

        vecStr.clear();
    }

    fin.close();

    Load_Folder("../Bin/Resource/Texture/Object", m_iObjFileCount);


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
        // items 벡터의 요소를 vecString의 요소로 채웁니다.
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


    //이미지 버튼 만드는 코드
    //PDIRECT3DTEXTURE9 texture;
    //HRESULT hr = D3DXCreateTextureFromFileA(m_pGraphicDev, "../Bin/Resource/Texture/Object/Poop/Poop_0.png", &texture);

    //D3DSURFACE_DESC my_image_desc;
    //texture->GetLevelDesc(0, &my_image_desc);
    //int width = (int)my_image_desc.Width;
    //int height = (int)my_image_desc.Height;

    //if (ImGui::ImageButton("##",(void*)texture, ImVec2(width, height)))
    //{

    //}

    

    //ImGui::BeginListBox("##");
    //if (0 < vecString.size())
    //    ImGui::ListBox("##", &m_iSelectedStageIndex, items.data(), vecString.size());
    //ImGui::EndListBox();

    ImGui::End();
}

void CStageToolGui::Load_Folder(string strFolderPath, int& iCount)
{
    //폴더 내 파일 갯수 읽어와야하는데 잘 안됨... 시부레



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
