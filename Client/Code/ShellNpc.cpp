#include "stdafx.h"
#include "ShellNpc.h"

#include "Export_Utility.h"

CShellNpc::CShellNpc(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CShellNpc::CShellNpc(const CShellNpc& rhs)
	: CMapObj(rhs)
{
}

CShellNpc::~CShellNpc()
{
}

HRESULT CShellNpc::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 1.5f, 1.5f, 1.5f };

	m_iPicNum = 1;
	m_fFrameSpeed = 1.f;

	m_bGame = false;

	m_ePreState = NPC_END;

	return S_OK;
}

_int CShellNpc::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (m_bGame)
	{
		m_eCurState = NPC_GAMING;
	}
	else
		m_eCurState = NPC_IDLE;

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CShellNpc::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CShellNpc::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CShellNpc::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CShellNpc::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CShellNpc::NPC_IDLE:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"ShellGame", L"Proto_ShellNpcTexture"));
			break;

		case CShellNpc::NPC_GAMING:
			m_iPicNum = 6;
			m_fFrameSpeed = 0.7f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"ShellGame", L"Proto_ShellNpcGameTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CShellNpc* CShellNpc::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShellNpc* pInstance = new CShellNpc(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("ShellNpc Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShellNpc::Free()
{
	__super::Free();
}
