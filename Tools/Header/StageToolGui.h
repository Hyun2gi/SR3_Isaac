#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include <filesystem>

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

    void	Set_Picking_Pos(_vec3 vPos) { m_vecPickingPos = vPos; }

    void    Popup_Object_Gui();

    void    Load_Folder(string strFilePath, int& iCount);

private:
    void Load_Stage_Object(const char* items);

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;
    bool m_bIsOpend;
    int m_iSelectedStageIndex;
    int m_iObjFileCount;

    map<int, string> m_mapStage;

    _vec3	m_vecPickingPos;

private:
    virtual void Free();
};
