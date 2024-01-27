#include "stdafx.h"
#include "Monstro.h"

#include "Export_Utility.h"

CMonstro::CMonstro(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CMonstro::CMonstro(const CMonstro& rhs)
	: CMonster(rhs)
{
}

CMonstro::~CMonstro()
{
}

HRESULT CMonstro::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 3.2f, 10.f);
	m_pTransformCom->m_vScale = { 5.f, 5.f, 0.f };

	m_iHp = 5;

	m_fCallLimit = 0.f;
	m_fSpeed = 1.f;

	return S_OK;
}

_int CMonstro::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 3.f * fTimeDelta * 0.4f;

	if (3.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);


	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CMonstro::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMonstro::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonstro::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMonstro::MoveTo_Player()
{
}

void CMonstro::JumpTo_Player()
{
}

void CMonstro::AttackTo_Player()
{
}

CMonster* CMonstro::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonstro* pInstance = new CMonstro(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Monstro Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMonstro::Free()
{
	__super::Free();
}
