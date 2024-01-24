#include "../Include/stdafx.h"
#include "..\Header\Effect.h"

#include "Export_System.h"
#include "Export_Utility.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CEffect::CEffect(const CEffect& rhs)
	: Engine::CGameObject(rhs)
{

}

CEffect::~CEffect()
{
}

HRESULT CEffect::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//srand(unsigned(time(NULL)));

	m_pTransformCom->Set_Pos(_float(rand() % 20), 0.f, _float(rand() % 20));

	return S_OK;
}

Engine::_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 90.f * fTimeDelta;

	if (90.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	_matrix	matWorld, matView, matBill;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	m_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

	Engine::Add_RenderGroup(RENDER_ALPHA, this);


	return 0;
}

void CEffect::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	__super::Compute_ViewZ(&vPos);

	//Height_OnTerrain();
}

void CEffect::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

HRESULT CEffect::Add_Component()
{
	CComponent*		pComponent = nullptr;
	
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_EffectTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_EffectTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
		
	return S_OK;
}

CEffect * CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect *	pInstance = new CEffect(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Effect Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CEffect::Free()
{	
	__super::Free();
}


void CEffect::Height_OnTerrain()
{
	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

	m_pTransformCom->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}


