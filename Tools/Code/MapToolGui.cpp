#pragma once
#include "MapToolGui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "Export_System.h"


CMapToolGui::CMapToolGui()
{
}

CMapToolGui::CMapToolGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    Ready_ImGuiTools(hWnd, pGraphicDev);
}

CMapToolGui::~CMapToolGui()
{
    Free();
}

HRESULT CMapToolGui::Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_bIsOpend = false;
    m_iSelectedStageIndex = 0;

    m_pGraphicDev = pGraphicDev;

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

        m_mapStage.emplace(pair<int, string>(stoi(vecStr[0]),  vecStr[1]));

        vecStr.clear();
    }

    fin.close();


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

void CMapToolGui::Update_ImGuiTools()
{
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //�� ������Ʈ���� ���ư�
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("MapTool");                          // Create a window called "Hello, world!" and append into it.

    static CHAR	szStageName[MAX_PATH] = "";
    static CHAR	szStageKey[MAX_PATH] = "";

    ImGui::Text("Stage Name: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90.f);
    ImGui::InputText("##Stage Name", szStageName, MAX_PATH, ImGuiInputTextFlags_None);

    ImGui::Text("Key: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputText("##Key", szStageKey, MAX_PATH);
    ImGui::SameLine();

    //�������� ����Ʈ�� �����ִ� map�� ��� �ۼ��� ���������� Add�Ѵ�.
    // Key���� �ߺ����� ���� ��쿡�� emplace�� ����ǰ� ����ó�� ����
    if (ImGui::Button("Add"))
    {
        if (m_mapStage.find(stoi(szStageKey)) == m_mapStage.end())
        {
            string strStageFullName = szStageName;
            strStageFullName = strStageFullName + "_" + szStageKey;

            m_mapStage.emplace(pair<int, string>(stoi(szStageKey), strStageFullName));
        }
    }   

    ImGui::NewLine();

    static CHAR szSelectedStage[MAX_PATH] = "";

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
        // items ������ ��Ҹ� vecString�� ��ҷ� ä��ϴ�.
        for (const string& str : vecString) {
            items.push_back(str.c_str());
        }
    }

    ImGui::Text("Stage List");
    ImGui::SameLine();

    if (ImGui::Button("Save"))
    {
        string strFilePath = "../Dat/MapLevel.dat";

        // ���� ��Ʈ���� ���ϴ�.
        ofstream fout(strFilePath.c_str(), ios::binary);

        // ���Ͽ� �����͸� ���ϴ�. Key, Name
        for_each(m_mapStage.begin(), m_mapStage.end(),
            [strFilePath, &fout](auto& iter) {
                fout << iter.first << ",";
                fout << iter.second << endl;
            });

        // ���� ��Ʈ���� �ݽ��ϴ�.
        fout.close();

    }

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
        Popup_Stage_Connection(vecString[m_iSelectedStageIndex].c_str());
    }

    ImGui::End();

}

void CMapToolGui::Render_ImGuiTools()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

void CMapToolGui::Popup_Stage_Connection(const char* items)
{
    ImGui::Begin("Stage Connection Relationship");

    //ImGui�� Input�ڽ��� ����� �����ʹ� static�� �ƴϸ� ������Ʈ �� ���� �ʱ�ȭ�Ǳ� ������ static���� ��������
    static CHAR	szSKeyLeftRoom[MAX_PATH] = "";
    static CHAR	szSKeyRightRoom[MAX_PATH] = "";
    static CHAR	szSKeyTopRoom[MAX_PATH] = "";
    static CHAR	szSKeyBottomRoom[MAX_PATH] = "";

    //.dat ������ �ҷ��� ��θ� �������ش�.
    string strFilePath = "../Dat/";
    string strExtension = ".dat";
    strFilePath = strFilePath + items + strExtension;

    ifstream fin(strFilePath);

    string strKeys = "";
    //�ڵ带 �� �� �о�´�. (���������� ���� ������ ���ٷ� �����ϰ� �����ױ� ������ ����)
    getline(fin, strKeys);

    fin.close();

    ImGui::Text("Selected Stage Name: %s", items);

    ImGui::NewLine();

    ImGui::Text("Please enter a key value.");

    ImGui::Text("Left: ");
    ImGui::SameLine();
    if (strKeys != "")
    {
        ImGui::Text("%c", strKeys[0]);
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Left", szSKeyLeftRoom, MAX_PATH);

    ImGui::Text("Right: ");
    ImGui::SameLine();
    if (strKeys != "")
    {
        ImGui::Text("%c", strKeys[1]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Right", szSKeyRightRoom, MAX_PATH);

    ImGui::Text("Top: "); 
    ImGui::SameLine();
    if (strKeys != "")
    {
        ImGui::Text("%c", strKeys[2]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Top", szSKeyTopRoom, MAX_PATH);
    
    ImGui::Text("Bottom: "); 
    ImGui::SameLine();
    if (strKeys != "")
    {
        ImGui::Text("%c", strKeys[3]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Bottom", szSKeyBottomRoom, MAX_PATH);

    if (ImGui::Button("SAVE"))
    {
        // ���� ��Ʈ���� ���ϴ�.
        ofstream fout(strFilePath.c_str(), ios::binary);

        // ���Ͽ� �����͸� ���ϴ�.
        fout << szSKeyLeftRoom;
        fout << szSKeyRightRoom;
        fout << szSKeyTopRoom;
        fout << szSKeyBottomRoom;

        // ���� ��Ʈ���� �ݽ��ϴ�.
        fout.close();
    }

    if (ImGui::Button("Close"))
        m_bIsOpend = false;

    ImGui::End();
}

void CMapToolGui::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
