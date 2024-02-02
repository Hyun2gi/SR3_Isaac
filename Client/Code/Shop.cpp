#include "stdafx.h"
#include "Shop.h"

#include "Export_Utility.h"

CShop::CShop(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pShopNpc(nullptr)
{
}

CShop::CShop(const CShop& rhs)
	: CMapObj(rhs),
	m_pShopNpc(rhs.m_pShopNpc)
{
}

CShop::~CShop()
{
}

HRESULT CShop::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 2.f, 10.f);

	return S_OK;
}

_int CShop::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (m_pShopNpc == nullptr)
		Create_NPC();

	if (m_pShopNpc != nullptr)
		m_pShopNpc->Update_GameObject(fTimeDelta);

	return 0;
}

void CShop::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pShopNpc != nullptr)
		m_pShopNpc->LateUpdate_GameObject();
}

void CShop::Render_GameObject()
{
	if (m_pShopNpc != nullptr)
		m_pShopNpc->Render_GameObject();
}

HRESULT CShop::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShopNpcTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShopNpcTexture", pComponent });

	// GOOD
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShopNpcThumbsTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShopNpcThumbsTexture", pComponent });

	// DEAD
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShopNpcDeadTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShopNpcDeadTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CShop::Create_NPC()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pShopNpc = CShopNpc::Create(m_pGraphicDev);
	m_pShopNpc->Set_MyLayer(m_vecMyLayer[0]);
	m_pShopNpc->Get_TransformCom()->Set_Pos(vPos);

	if (m_pShopNpc == nullptr)
		return;
}

void CShop::Create_Item()
{

}

CShop* CShop::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShop* pInstance = new CShop(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Shop Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShop::Free()
{
	Safe_Release<CShopNpc*>(m_pShopNpc);
	m_pShopNpc = nullptr;

	__super::Free();
}
