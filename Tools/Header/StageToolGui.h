#pragma once
#include "Base.h"
#include "Engine_Define.h"

class CStageToolGui : public CBase
{
    struct StageStruct
    {
        string  strStageName;
        int     iKey;
    };

public:
    CStageToolGui();
    CStageToolGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CStageToolGui();

public:
    HRESULT Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    void Update_ImGuiTools();
    void Render_ImGuiTools();

private:
    void Load_Stage_Object(const char* items);

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;
    bool m_bIsOpend;
    int m_iSelectedStageIndex;

    map<int, string> m_mapStage;

private:
    virtual void Free();
};
