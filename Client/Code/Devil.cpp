#include "stdafx.h"
#include "Devil.h"

#include "Export_System.h"
#include "Export_Utility.h"

CDevil::CDevil(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CDevil::CDevil(const CDevil& rhs)
	: CMapObj(rhs)
{
}

CDevil::~CDevil()
{
}

HRESULT CDevil::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, 2.5f, 0.f);
	m_pTransformCom->m_vScale = { 2.5f, 2.5f, 2.5f };

	m_eObjType = DEVIL;

	return S_OK;
}

_int CDevil::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bCreate)
	{
		Create_Start_Particle(m_iRandNum % 5 + 2);

		m_bCreate = true;
	}

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;

}

void CDevil::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CDevil::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CDevil::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DevilTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DevilTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

CDevil* CDevil::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDevil* pInstance = new CDevil(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Devil Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDevil::Free()
{
	__super::Free();
}
