#include "stdafx.h"
#include "ShellGame.h"

#include "Export_System.h"
#include "Export_Utility.h"

CShellGame::CShellGame(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CShellGame::CShellGame(const CShellGame& rhs)
	: CMapObj(rhs)
{
}

CShellGame::~CShellGame()
{
}

HRESULT CShellGame::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(5.f, 2.f, 0.f);

	m_bGame = false;

	return S_OK;
}

_int CShellGame::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);


	return 0;
}

void CShellGame::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CShellGame::Render_GameObject()
{
}

HRESULT CShellGame::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// NPC _ IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellNpcTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellNpcTexture", pComponent });

	// NPC _ GAMING
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellNpcGameTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellNpcGameTexture", pComponent });

	// SHELL
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CShellGame::Create_NPC()
{
}

void CShellGame::Create_Shell()
{
}

CShellGame* CShellGame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShellGame* pInstance = new CShellGame(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("ShellGame Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShellGame::Free()
{

	__super::Free();
}
