#include "stdafx.h"
#include "..\Header\PlayerLeg.h"

#include "Export_System.h"
#include "Export_Utility.h"

CPlayerLeg::CPlayerLeg(LPDIRECT3DDEVICE9 pGraphicDev)
	:Engine::CGameObject(pGraphicDev)
{
}

CPlayerLeg::CPlayerLeg(const CPlayerLeg& rhs)
	: Engine::CGameObject(rhs)
{
}

CPlayerLeg::~CPlayerLeg()
{
}

HRESULT CPlayerLeg::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fFrame = 0.f;
	m_iPicNum = 0; // 이미지 개수
	m_fSpriteSpeed = 0.9f;
	m_iPicNum = 0;

	return S_OK;
}

_int CPlayerLeg::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CPlayerLeg::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPlayerLeg::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_iPicNum);

	m_pBufferCom->Render_Buffer();
}

HRESULT CPlayerLeg::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_Pill"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_Pill", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });


	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPlayerLeg::Motion_Change()
{
	if (m_ePreLegState != m_eCurLegState)
	{
		m_fFrame = 0.f;
		switch (m_eCurLegState)
		{
		case P_LEG_IDLE:
			m_iPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_CoinTexture_IDLE"));
			break;
		case P_LEG_FRONT:
			m_iPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_CoinTexture_IDLE"));
			break;
		case P_LEG_LEFT:
			m_iPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_CoinTexture_IDLE"));
			break;
		case P_LEG_RIGHT:
			m_iPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_CoinTexture_IDLE"));
			break;
		}

		m_ePreLegState = m_eCurLegState;
	}
}

CPlayerLeg* CPlayerLeg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerLeg* pInstance = new CPlayerLeg(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("PlayerLeg Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlayerLeg::Free()
{
	__super::Free();
}
