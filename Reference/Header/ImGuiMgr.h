#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL CImGuiMgr :
    public CBase
{
    DECLARE_SINGLETON(CImGuiMgr)


private:
    explicit CImGuiMgr();
    virtual ~CImGuiMgr();

public:
    HRESULT Ready_ImGuiMgr(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev);
    void Update_ImGuiMgr();
    void Render_ImGuiMgr();

private:
    LPDIRECT3DDEVICE9 m_pGraphicDev;

private:
    virtual void Free();
};
END
