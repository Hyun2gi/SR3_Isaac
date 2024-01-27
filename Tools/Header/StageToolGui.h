#pragma once
#include "Base.h"
#include "Engine_Define.h"

class CStageTool;

class CStageToolGui : public CBase
{
private:
    struct PlacementObjInfo
    {
        int iType;
        int iIndex;
        float x, y, z;
    };

private:
    const float TEXTURE_SIZE = 0.15f;

public:
    CStageToolGui();
    CStageToolGui(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CStageToolGui();

public:
    HRESULT Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    void Update_ImGuiTools();
    void Render_ImGuiTools();

    void	Set_Picking_Pos(_vec3 vPos) { m_vPickingPos = vPos; }
    void    Set_Target_Scene(CStageTool* pScene) { m_pTargetScene = pScene; }
    void    Popup_Object_Gui();
    void    Create_Image_Buttons();

    void    Push_Placement_Obj_List(int iType, int iIndex, float x, float y, float z);

private:
    void	Load_Stage_Design();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;

    bool m_bIsOpend;
    int m_iSelectedStageIndex;

    _vec3	m_vPickingPos;

    CStageTool* m_pTargetScene;

    vector<int> vecObjCounts;
    map<int, string> m_mapStage;

    unordered_map<int, vector<PDIRECT3DTEXTURE9>> m_umapObjectType;

    list<PlacementObjInfo> m_PlacementObjList;

private:
    virtual void Free();
};
