#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CImGuiTools :
    public CBase
{
public:
    CImGuiTools();
    CImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CImGuiTools();

public:
    HRESULT Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    void Update_ImGuiTools();
    void Render_ImGuiTools();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;

private:
    virtual void Free();
};
END
