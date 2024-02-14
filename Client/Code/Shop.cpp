#include "stdafx.h"
#include "Shop.h"

#include "Export_Utility.h"

CShop::CShop(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pShopNpc(nullptr),
	m_pPill(nullptr), m_pHeart(nullptr), m_pSadOnion(nullptr)
{
}

CShop::CShop(const CShop& rhs)
	: CMapObj(rhs),
	m_pShopNpc(rhs.m_pShopNpc),
	m_pPill(rhs.m_pPill), m_pHeart(rhs.m_pHeart), m_pSadOnion(rhs.m_pSadOnion)
{
}

CShop::~CShop()
{
}

void CShop::Set_Item_ToStage(CLayer* pLayer)
{
	if (!m_bIsLayerInsert)
	{
		if (m_pPill != nullptr && m_pSadOnion != nullptr && m_pHeart != nullptr)
		{
			pLayer->Add_GameObject(L"Pill", m_pPill);
			pLayer->Add_GameObject(L"Epic", m_pSadOnion);
			pLayer->Add_GameObject(L"Heart", m_pHeart);

			m_bIsLayerInsert = true;
		}
	}
}

HRESULT CShop::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 1.f, 25.f);

	m_bIsLayerInsert = false;

	return S_OK;
}

_int CShop::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Create_Obj();

	Check_Sell();

	if (m_pShopNpc != nullptr)
		m_pShopNpc->Update_GameObject(fTimeDelta);

	if (!m_vecCoinFont.empty())
	{
		for (auto& iter : m_vecCoinFont)
		{
			iter->Update_GameObject(fTimeDelta);
		}
	}

	return 0;
}

void CShop::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pShopNpc != nullptr)
		m_pShopNpc->LateUpdate_GameObject();

	if (!m_vecCoinFont.empty())
	{
		for (auto& iter : m_vecCoinFont)
		{
			iter->LateUpdate_GameObject();
		}
	}
}

void CShop::Render_GameObject()
{
	if (m_pShopNpc != nullptr)
		m_pShopNpc->Render_GameObject();

	if (!m_vecCoinFont.empty())
	{
		for (auto& iter : m_vecCoinFont)
		{
			iter->Render_GameObject();
		}
	}
}

HRESULT CShop::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region NPC Texture

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

#pragma endregion NPC Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CShop::Create_Obj()
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	if (m_pShopNpc == nullptr)
	{
		m_pShopNpc = CShopNpc::Create(m_pGraphicDev);
		m_pShopNpc->Set_MyLayer(m_vecMyLayer[0]);
		m_pShopNpc->Get_TransformCom()->Set_Pos(vPos);
	}

	if (m_pPill == nullptr)
	{
		_vec3 vPillPos = { vPos.x - INTERVALX, vPos.y - 0.5f, vPos.z - INTERVALZ };
		m_pPill = CPill::Create(m_pGraphicDev, 3, vPillPos, vDir);
		m_pPill->Set_MyLayer(L"GameItem");

		// Coin font
		m_vecCoinFont.push_back(CCoinFont::Create(m_pGraphicDev));
		m_vecCoinFont.front()->Get_TransformCom()->Set_Pos(vPillPos.x, vPillPos.y + INTERVALY, vPillPos.z);
		dynamic_cast<CCoinFont*>(m_vecCoinFont.front())->Set_State(1);
	}

	

	if (m_pSadOnion == nullptr)
	{
		_vec3 vEpicPos = { vPos.x, vPos.y - 0.5f, vPos.z - INTERVALZ };
		m_pSadOnion = CSadOnion::Create(m_pGraphicDev, 3, vEpicPos, vDir);
		m_pSadOnion->Set_MyLayer(L"GameItem");

		// Coin font
		m_vecCoinFont.push_back(CCoinFont::Create(m_pGraphicDev));
		m_vecCoinFont.back()->Get_TransformCom()->Set_Pos(vEpicPos.x, vEpicPos.y + INTERVALY, vEpicPos.z);
		dynamic_cast<CCoinFont*>(m_vecCoinFont.back())->Set_State(2);
	}

	if (m_pHeart == nullptr)
	{
		_vec3 vHeartPos = { vPos.x + INTERVALX, vPos.y - 0.5f, vPos.z - INTERVALZ }; // INTERVALZ
		m_pHeart = CHeart::Create(m_pGraphicDev, 3, vHeartPos, vDir, 0);
		m_pHeart->Set_MyLayer(L"GameItem");

		// Coin font
		m_vecCoinFont.push_back(CCoinFont::Create(m_pGraphicDev));
		m_vecCoinFont.back()->Get_TransformCom()->Set_Pos(vHeartPos.x, vHeartPos.y + INTERVALY, vHeartPos.z);
		dynamic_cast<CCoinFont*>(m_vecCoinFont.back())->Set_State(0);
	}
}

void CShop::Check_Sell()
{
	// 해당 아이템 Dead 시 CoinFont 사라지게
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

	Safe_Release<CPill*>(m_pPill);
	m_pPill = nullptr;

	Safe_Release<CSadOnion*>(m_pSadOnion);
	m_pSadOnion = nullptr;

	Safe_Release<CHeart*>(m_pHeart);
	m_pHeart = nullptr;

	for_each(m_vecCoinFont.begin(), m_vecCoinFont.end(), CDeleteObj());

	__super::Free();
}
