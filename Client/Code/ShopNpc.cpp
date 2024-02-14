#include "stdafx.h"
#include "ShopNpc.h"

#include "Export_System.h"
#include "Export_Utility.h"

CShopNpc::CShopNpc(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CShopNpc::CShopNpc(const CShopNpc& rhs)
	: CMapObj(rhs)
{
}

CShopNpc::~CShopNpc()
{
}

HRESULT CShopNpc::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };

	m_iPicNum = 2;
	m_fFrameSpeed = 1.f;

	m_fCallLimit = 3.f;

	m_bGood = false;
	m_bFrameFix = false;

	m_ePreState = NPC_END;

	return S_OK;
}

_int CShopNpc::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead)
	{
		m_eCurState = NPC_DEAD;
	}
	else
	{
		if (m_bGood)
		{
			m_eCurState = NPC_GOOD;

			if (Check_Time(fTimeDelta))
			{
				m_bGood = false;
			}
		}
		else
			m_eCurState = NPC_IDLE;
	}

	if (!m_bFrameFix)
	{
		m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

		if (m_iPicNum < m_fFrame)
		{
			if(NPC_IDLE == m_eCurState)
				m_fFrame = 0.f;

			else if(NPC_GOOD == m_eCurState || NPC_DEAD == m_eCurState)
			{
				m_fFrame = 1.9f;
				m_bFrameFix = true;
			}
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);



	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CShopNpc::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CShopNpc::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CShopNpc::Add_Component()
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

void CShopNpc::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CShopNpc::NPC_IDLE:
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Shop", L"Proto_ShopNpcTexture"));
			break;

		case CShopNpc::NPC_GOOD:
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Shop", L"Proto_ShopNpcThumbsTexture"));
			break;

		case CShopNpc::NPC_DEAD:
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Shop", L"Proto_ShopNpcDeadTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CShopNpc::Hit()
{
	// 폭탄에 맞았을 때만 충돌 처리 되도록
	m_eCurState = NPC_DEAD;
}

CShopNpc* CShopNpc::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShopNpc* pInstance = new CShopNpc(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("ShopNpc Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShopNpc::Free()
{
	__super::Free();
}

