#include "stdafx.h"
#include "ShellGame.h"

#include "Export_System.h"
#include "Export_Utility.h"

CShellGame::CShellGame(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pShellNpc(nullptr)
{
}

CShellGame::CShellGame(const CShellGame& rhs)
	: CMapObj(rhs),
	m_pShellNpc(rhs.m_pShellNpc)
{
}

CShellGame::~CShellGame()
{
}

HRESULT CShellGame::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, 3.f, 0.f);

	m_bGame = false;

	return S_OK;
}

_int CShellGame::Update_GameObject(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (m_pShellNpc == nullptr)
		Create_NPC();
	else
		m_pShellNpc->Update_GameObject(fTimeDelta);

	if (m_vecShell.empty())
		Create_Shell();
	else
	{
		for (auto& iter : m_vecShell)
		{
			iter->Update_GameObject(fTimeDelta);
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CShellGame::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pShellNpc != nullptr)
		m_pShellNpc->LateUpdate_GameObject();

	if (!m_vecShell.empty())
	{
		for (auto& iter : m_vecShell)
		{
			iter->LateUpdate_GameObject();
		}
	}
}

void CShellGame::Render_GameObject()
{
	if (m_pShellNpc != nullptr)
		m_pShellNpc->Render_GameObject();

	if (!m_vecShell.empty())
	{
		for (auto& iter : m_vecShell)
		{
			iter->Render_GameObject();
		}
	}
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
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pShellNpc = CShellNpc::Create(m_pGraphicDev);
	m_pShellNpc->Set_MyLayer(m_vecMyLayer[0]);
	m_pShellNpc->Get_TransformCom()->Set_Pos(vPos);
	m_pShellNpc->Set_Game_False();

	if (m_pShellNpc == nullptr)
		return;
}

void CShellGame::Create_Shell()
{
	// Shell 세 개 생성
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fScalar = SCALAR_X;

	for (int i = 0; i < 3; ++i)
	{
		CShell* pShell = CShell::Create(m_pGraphicDev);
		pShell->Set_MyLayer(m_vecMyLayer[0]);
		pShell->Get_TransformCom()->Set_Pos(vPos.x + fScalar, vPos.y, vPos.z);
		m_vecShell.push_back(pShell);
		fScalar += 1.f;
	}
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
	Safe_Release<CShellNpc*>(m_pShellNpc);
	m_pShellNpc = nullptr;

	__super::Free();
}
