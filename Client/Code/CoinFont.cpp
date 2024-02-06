#include "stdafx.h"
#include "CoinFont.h"

#include "Export_System.h"
#include "Export_Utility.h"

CCoinFont::CCoinFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CCoinFont::CCoinFont(const CCoinFont& rhs)
	: CMapObj(rhs)
{
}

CCoinFont::~CCoinFont()
{
}

void CCoinFont::Set_State(_int iState)
{
	switch (iState)
	{
	case 0:
		m_eState = COIN_3;
		break;
	case 1:
		m_eState = COIN_5;
		break;
	case 2:
		m_eState = COIN_15;
		break;
	}
}

HRESULT CCoinFont::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eState = COIN_3;

	return S_OK;
}

_int CCoinFont::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CCoinFont::LateUpdate_GameObject()
{
	Change_Texture();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CCoinFont::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CCoinFont::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoinFontTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CoinFontTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CCoinFont::Change_Texture()
{
	switch (m_eState)
	{
	case CCoinFont::COIN_3:
		m_fFrame = 0.f;
		break;
	case CCoinFont::COIN_5:
		m_fFrame = 1.f;
		break;
	case CCoinFont::COIN_15:
		m_fFrame = 2.f;
		break;
	}
}

void CCoinFont::Fall_Down()
{
}

CCoinFont* CCoinFont::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCoinFont* pInstance = new CCoinFont(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CoinFont Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCoinFont::Free()
{
	__super::Free();
}
