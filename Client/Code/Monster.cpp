#include "stdafx.h"
#include "..\Header\Monster.h"

#include "Export_Utility.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
	: Engine::CGameObject(rhs)
{

}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Engine::_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Engine::CTransform*	pPlayerTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Environment", L"Player", L"Proto_Transform"));
	NULL_CHECK_RETURN(pPlayerTransformCom, -1);

	_vec3	vPlayerPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	m_pTransformCom->Chase_Target(&vPlayerPos, 3.f, fTimeDelta);



	return 0;
}

void CMonster::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMonster::Add_Component()
{
	CComponent*		pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CTriCol*>(Engine::Clone_Proto(L"Proto_TriCol"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TriCol", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Transform", pComponent });
		
	return S_OK;
}

CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster *	pInstance = new CMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Monster Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMonster::Free()
{	
	__super::Free();
}

