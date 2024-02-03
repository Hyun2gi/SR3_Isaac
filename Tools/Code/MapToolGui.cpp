#pragma once
#include "MapToolGui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "Export_System.h"

#include "MapTool.h"

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
    m_iSelectedRoomThemeIndex = 0;

    m_pGraphicDev = pGraphicDev;

    Load_MapLevel();
    Load_Room_Type();
    Load_Room_Theme();

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

    if (0 < vecString.size()) 
    {
        for (const string& str : vecString) 
            items.push_back(str.c_str());
    }

    ImGui::Text("Stage List");
    ImGui::SameLine();

    if (ImGui::Button("Save"))
        Save_Stage_List();

    ImGui::SetNextItemWidth(100.f);
    ImGui::BeginListBox("##");
    if (0 < vecString.size())
        ImGui::ListBox("##", &m_iSelectedStageIndex, items.data(), vecString.size());
    ImGui::EndListBox();

    //����Ʈ �ڽ� Ŭ�� �� ����Ǵ� �̺�Ʈ(0 = left, 1 = right, 2 = middle)
    if (ImGui::IsItemClicked())
    {
        m_bIsOpend = true;
        Set_Index_Val(vecString[m_iSelectedStageIndex].c_str());
    }
     
    if (m_bIsOpend)
        Popup_Stage_Connection(vecString[m_iSelectedStageIndex].c_str());

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

    ImGui::Text("Selected Stage Name: %s", items);

    //�׸� ����� �����ִ� ��Ӵٿ�ڽ�
    vector<const char*> vecRoomThemeText;

    if (0 < m_vecRoomTheme.size())
    {
        for (const string& str : m_vecRoomTheme)
            vecRoomThemeText.push_back(str.c_str());
    }

    ImGui::Text("Room Theme: ");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(120.f);
    ImGui::Combo("##0", &m_iSelectedRoomThemeIndex, vecRoomThemeText.data(), m_vecRoomTheme.size());

    Change_Theme_Texture();

    // �������� Ÿ�� ����� �����ִ� ��Ӵٿ�ڽ�
    vector<const char*> vecRoomTypeText;

    if (0 < m_vecRoomType.size())
    {
        for (const string& str : m_vecRoomType)
            vecRoomTypeText.push_back(str.c_str());
    }

    ImGui::Text("Room Type: ");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(120.f);
    ImGui::Combo("##1", &m_iSelectedRoomTypeIndex, vecRoomTypeText.data(), m_vecRoomType.size());

    ImGui::NewLine();

    ImGui::Text("Please enter a key value.");

    ImGui::Text("Left: ");
    ImGui::SameLine();
    if (m_strGetLine != "")
    {
        ImGui::Text("%c", m_strGetLine[0]);
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Left", szSKeyLeftRoom, MAX_PATH);

    ImGui::Text("Right: ");
    ImGui::SameLine();
    if (m_strGetLine != "")
    {
        ImGui::Text("%c", m_strGetLine[2]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Right", szSKeyRightRoom, MAX_PATH);

    ImGui::Text("Top: "); 
    ImGui::SameLine();
    if (m_strGetLine != "")
    {
        ImGui::Text("%c", m_strGetLine[4]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Top", szSKeyTopRoom, MAX_PATH);
    
    ImGui::Text("Bottom: "); 
    ImGui::SameLine();
    if (m_strGetLine != "")
    {
        ImGui::Text("%c", m_strGetLine[6]);
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(30.f);
    ImGui::InputText("##Bottom", szSKeyBottomRoom, MAX_PATH);

    if (ImGui::Button("SAVE"))
    {
        //.dat ������ �ҷ��� ��θ� �������ش�.
        string strFilePath = "../../Dat/";
        string strExtension = ".dat";
        strFilePath = strFilePath + items + strExtension;

        // ���� ��Ʈ���� ���ϴ�.
        ofstream fout(strFilePath.c_str(), ios::binary);

        // ���Ͽ� �����͸� ���ϴ�.
        fout << szSKeyLeftRoom << ",";
        fout << szSKeyRightRoom << ",";
        fout << szSKeyTopRoom << ",";
        fout << szSKeyBottomRoom << ",";
        fout << vecRoomThemeText[m_iSelectedRoomThemeIndex] << ",";
        fout << vecRoomTypeText[m_iSelectedRoomTypeIndex];

        // ���� ��Ʈ���� �ݽ��ϴ�.
        fout.close();
    }

    if (ImGui::Button("Close"))
        m_bIsOpend = false;

    ImGui::End();
}

void CMapToolGui::Load_MapLevel()
{

    string strFilePath = "../../Dat/MapLevel.dat";

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

void CMapToolGui::Load_Room_Type()
{
    string strFilePath = "../../Dat/Room_Type.txt";

    ifstream fin(strFilePath);

    string strGetLine = "";
    //�ڵ带 �� �پ� �о�´�.
    while (getline(fin, strGetLine))
    {
        int iIndex = 0;

        while (true) {
            // , ��ġ ã��
            int pos = strGetLine.find_first_of(',', iIndex);

            // ,�� ã�� ���ϸ� ����
            if (pos == string::npos) 
                break;

            // �и��� ���ڿ� ���
            m_vecRoomType.push_back(strGetLine.substr(iIndex, pos - iIndex));
            iIndex = pos + 1;
        }
    }

    fin.close();
}

void CMapToolGui::Load_Room_Theme()
{
    string strFilePath = "../../Dat/Room_Theme.txt";

    ifstream fin(strFilePath);

    string strGetLine = "";
    //�ڵ带 �� �پ� �о�´�.
    while (getline(fin, strGetLine))
    {
        int iIndex = 0;

        while (true) {
            // , ��ġ ã��
            int pos = strGetLine.find_first_of(',', iIndex);

            // ,�� ã�� ���ϸ� ����
            if (pos == string::npos)
                break;

            // �и��� ���ڿ� ���
            m_vecRoomTheme.push_back(strGetLine.substr(iIndex, pos - iIndex));
            iIndex = pos + 1;
        }
    }

    fin.close();
}

void CMapToolGui::Save_Stage_List()
{
    string strFilePath = "../../Dat/MapLevel.dat";

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

void CMapToolGui::Change_Theme_Texture()
{
    wstring wstrTexture;
    wstrTexture.assign(m_vecRoomTheme[m_iSelectedRoomThemeIndex].begin(), m_vecRoomTheme[m_iSelectedRoomThemeIndex].end());
    m_pTargetScene->Set_Theme_Texture(wstrTexture.c_str());
}

void CMapToolGui::Set_Index_Val(const char* items)
{
    //.dat ������ �ҷ��� ��θ� �������ش�.
    string strFilePath = "../../Dat/";
    string strExtension = ".dat";
    strFilePath = strFilePath + items + strExtension;

    //�ڵ带 �� �� �о�´�. (���������� ���� ������ ���ٷ� �����ϰ� �����ױ� ������ ����)
    ifstream fin(strFilePath);
    m_strGetLine = "";
    getline(fin, m_strGetLine);
    fin.close();

    //left, right, top, bottom, theme, roomtype
    int iTypePos = m_strGetLine.find_last_of(",") + 1;
    m_strCurRoomType = m_strGetLine.substr(iTypePos);

    string strTemp = m_strGetLine.substr(0, iTypePos - 1);
    int iThemePos = strTemp.find_last_of(",") + 1;

    m_strCurRoomTheme = strTemp.substr(iThemePos);

    if (m_strGetLine != "")
    {
        m_iSelectedRoomTypeIndex = 0;

        for (int i = 0; i < m_vecRoomType.size(); ++i)
        {
            if (m_vecRoomType[i] == m_strCurRoomType)
            {
                m_iSelectedRoomTypeIndex = i;
                break;
            }
        }

        m_iSelectedRoomThemeIndex = 0;

        for (int i = 0; i < m_strCurRoomTheme.size(); ++i)
        {
            if (m_vecRoomTheme[i] == m_strCurRoomTheme)
            {
                m_iSelectedRoomThemeIndex = i;
                break;
            }
        }
    }
}

void CMapToolGui::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
