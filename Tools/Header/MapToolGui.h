#pragma once
#include "Base.h"
#include "Engine_Define.h"

class CMapTool;

class CMapToolGui : public CBase
{
    struct StageStruct
    {
        string  strStageName;
        int     iKey;
    };

public:
    CMapToolGui();
    CMapToolGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CMapToolGui();

public:
    HRESULT Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    void Update_ImGuiTools();
    void Render_ImGuiTools();

    void    Set_Target_Scene(CMapTool* pScene) { m_pTargetScene = pScene; }

private:
    void Popup_Stage_Connection(const char* items);

    void    Load_MapLevel();
    void    Load_Room_Type();
    void    Load_Room_Theme();

    void    Save_Stage_List();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;

    CMapTool* m_pTargetScene;

    bool m_bIsOpend;
    int m_iSelectedStageIndex;

    int m_iSelectedRoomThemeIndex;
    int m_iSelectedRoomTypeIndex;

    // �������� ��ϰ� ������ �����ϴ� ��
    map<int, string> m_mapStage;

    // ������ �� �ִ� �� �׸��� ����� �����ϴ� ����
    vector<string> m_vecRoomTheme;
    // �� Ÿ���� �����ϴ� ����
    vector<string> m_vecRoomType;

    string m_strCurRoomTheme;
    string m_strCurRoomType;

private:
    virtual void Free();
};
