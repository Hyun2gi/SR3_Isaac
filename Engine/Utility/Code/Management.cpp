#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement(): m_pScene(nullptr)
{
}

CManagement::~CManagement()
{
	Free();
}

CComponent * CManagement::Get_Component(COMPONENTID eID, const _tchar * pLayerTag, const _tchar * pObjTag, const _tchar * pComponentTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
} 

HRESULT CManagement::Set_Scene(CScene * pScene)
{
	Safe_Release(m_pScene);

	Engine::Clear_RenderGroup();

	m_pScene = pScene;

	return S_OK;
}

_int CManagement::Update_Scene(const _float & fTimeDelta)
{
	NULL_CHECK_RETURN(m_pScene, -1)

	return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene()
{
	NULL_CHECK(m_pScene);
	m_pScene->LateUpdate_Scene();
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::Render_GameObject(pGraphicDev);

	NULL_CHECK(m_pScene);
	m_pScene->Render_Scene();
}

void CManagement::Free()
{
	Safe_Release(m_pScene);
}
