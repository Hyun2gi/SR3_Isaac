#include "stdafx.h"
#include "Shell.h"

#include "Export_System.h"
#include "Export_Utility.h"

CShell::CShell(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CShell::CShell(const CShell& rhs)
	: CMapObj(rhs)
{
}

CShell::~CShell()
{
}

HRESULT CShell::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };

	m_fSpeed = 0.2f;

	m_fCallLimit = 0.3f;

	m_iPicNum = 1;
	m_fFrameSpeed = 1.f;

	m_bStartUp = false;
	m_bMoveDown = false;
	m_bShaking_Ready = false;
	m_bReward = false;
	m_bLose = false;

	m_eObjID = MOBJID_SHELL;

	return S_OK;
}

_int CShell::Update_GameObject(const _float& fTimeDelta)
{
	m_eDropItem = COIN;
	Setting_ItemTag();

	CGameObject::Update_GameObject(fTimeDelta);

	if (m_bStartUp)
		Start_Up(fTimeDelta);

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CShell::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CShell::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CShell::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CShell::Hit()
{
}

void CShell::Start_Up(const _float& fTimeDelta)
{
	// 게임 시작 _ 위로 올라갔다 내려옴
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (!m_bMoveDown)
	{
		vPos.y += m_fSpeed;

		if (vPos.y >= 3.f)
		{
			vPos.y = 3.f;
			if(Check_Time(fTimeDelta))
				m_bMoveDown = true;
		}
	}
	else
	{
		vPos.y -= m_fSpeed;

		if (vPos.y <= 1.f) // 임의 값이라 조정 필요
		{
			vPos.y = 1.f;
			m_bStartUp = false;
			m_bShaking_Ready = true;
		}
	}
	m_pTransformCom->Set_Pos(vPos);
}

CShell* CShell::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShell* pInstance = new CShell(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Shell Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShell::Free()
{
	__super::Free();
}
