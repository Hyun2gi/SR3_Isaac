#include "stdafx.h"
#include "Machine.h"

#include "Export_Utility.h"

CMachine::CMachine(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CMachine::CMachine(const CMachine& rhs)
	: CMapObj(rhs)
{
}

CMachine::~CMachine()
{
}

HRESULT CMachine::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };

	m_bGame = false;
	m_bBroken = false;

	m_ePreState = MC_END;

	return S_OK;
}

_int CMachine::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
		m_eCurState = MC_BROKEN;

	// 충돌 했을 때 bool 값 true 만들면서 Motion Change() 호출
	if (m_bGame)
	{
		m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

		if (m_iPicNum < m_fFrame)
		{
			m_fFrame = 0.f;
			m_bGame = false;
		}
	}
	else
	{
		m_fFrame = 0.f;
	}

	if (m_bBroken)
	{
		m_eCurState = MC_BROKEN;

	}
	else
		m_eCurState = MC_IDLE;

	CGameObject::Update_GameObject(fTimeDelta);

	//m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CMachine::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMachine::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMachine::Add_Component()
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

void CMachine::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CMachine::MC_IDLE:
			m_iPicNum = 3;
			m_fFrameSpeed = 2.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_SlotMCTexture"));
			break;

		case CMachine::MC_BROKEN:
			m_iPicNum = 1;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"SlotMC", L"Proto_BrokenSlotMCTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CMachine* CMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMachine* pInstance = new CMachine(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Machine Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMachine::Free()
{
	__super::Free();
}
