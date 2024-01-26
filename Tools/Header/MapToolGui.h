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

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;
    bool m_bIsOpend;
    int m_iSelectedStageIndex;

    map<int, string> m_mapStage;

private:
    virtual void Free();
};