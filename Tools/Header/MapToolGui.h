#pragma once
#include "Base.h"
#include "Engine_Define.h"

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

private:
    void Popup_Stage_Connection(const char* items);

    void    Load_MapLevel();
    void    Load_Room_Theme();

    void    Save_Stage_List();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;
    bool m_bIsOpend;
    int m_iSelectedStageIndex;
    int m_iSelectedRommThemeIndex;

    map<int, string> m_mapStage;
    vector<string> m_vecRoomTheme;

private:
    virtual void Free();
};
