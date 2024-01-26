#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include <filesystem>

class CStageToolGui : public CBase
{
private:
    const int OBJECT_MAX = 3;
    const float TEXTURE_SIZE = 0.15f;

    struct StageStruct
    {
        string  strStageName;
        int     iKey;
    };

    enum OBJECT_TYPE
    {
        OBJECTS,
        MONSTERS,
        BOSSES
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

    

private:
    void    Load_Level_Data();
    void    Load_Stage_Object(const char* items);
    void    Load_Object_Counts();
    void    Load_Object_Textures();
    
    void    Popup_Object_Gui();

    void    Create_Image_Buttons();

    string Trans_Object_Type_To_String(int iValue);

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;
    
    bool m_bIsOpend;
    int m_iSelectedStageIndex;
    
    vector<int> vecObjCounts;

    map<int, string> m_mapStage;
    unordered_map<int, vector<PDIRECT3DTEXTURE9>> m_umapObjectType;

    _vec3	m_vecPickingPos;

private:
    virtual void Free();
};
