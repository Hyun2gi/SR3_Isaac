#pragma once
#include "ImGuiTools.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "Export_System.h"


CImGuiTools::CImGuiTools()
{
}

CImGuiTools::CImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    Ready_ImGuiTools(hWnd, pGraphicDev);
}

CImGuiTools::~CImGuiTools()
{
    Free();
}

HRESULT CImGuiTools::Ready_ImGuiTools(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);

	return S_OK;
}

void CImGuiTools::Update_ImGuiTools()
{
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //매 업데이트마다 돌아감
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("test");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

   // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

}

void CImGuiTools::Render_ImGuiTools()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    //m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    //m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    //D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(255, 255, 255, 255);
    //m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    //if (m_pGraphicDev->BeginScene() >= 0)
    //{
    //    ImGui::Render();
    //    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    //    m_pGraphicDev->EndScene();
    //}

    //HRESULT result = m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);

}

void CImGuiTools::Free()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
