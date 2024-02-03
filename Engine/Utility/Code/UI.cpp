#include "UI.h"

#include "Export_Utility.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CUI::~CUI()
{
}

void Engine::CUI::Free()
{
	CGameObject::Free();
}

HRESULT Engine::CUI::Ready_GameObject()
{	
	D3DXMatrixIdentity(&m_matView);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return S_OK;
}

Engine::_int Engine::CUI::Update_GameObject(const _float& fTimeDelta)
{
	Engine::Add_RenderGroup(RENDER_UI, this);

	return CGameObject::Update_GameObject(fTimeDelta);
}

void Engine::CUI::LateUpdate_GameObject()
{
	CGameObject::LateUpdate_GameObject();
}

void CUI::Compute_ProjectionMatrix()
{
	D3DXMatrixOrthoLH(&m_matProj, m_fWinCX, m_fWinCY, 0.0f, 1.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

}


