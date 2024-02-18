#include "stdafx.h"
#include "EndingBlackBack.h"

#include "Export_Utility.h"

CEndingBlackBack::CEndingBlackBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CEndingBlackBack::CEndingBlackBack(const CEndingBlackBack& rhs)
	: Engine::CUI(rhs)
{
}

CEndingBlackBack::~CEndingBlackBack()
{
}

HRESULT CEndingBlackBack::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_iRenderIndex = 1;

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CEndingBlackBack::Update_GameObject(const _float& fTimeDelta)
{
	//알파값 변경된 것이 쉐이더에 잘 적용되는지 테스트용으로 작성해둠

	//Off면 밝아지는 상태
	if (!m_bOff && 0 < m_fAlpha)
	{
		m_fAlpha -= 0.5f * fTimeDelta;
	}
	//On이면 어두워지는 상태
	else if (m_bOff && 1 > m_fAlpha)
	{
		m_fAlpha += 0.5f * fTimeDelta;
	}
		
	
	CUI::Update_GameObject(fTimeDelta);

	return 0;
}

void CEndingBlackBack::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CEndingBlackBack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_pShaderCom, "g_Texture", 0);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix())))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_matView)))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_matProj)))
		return;
	if (FAILED(m_pShaderCom->Bind_Float("g_fAlpahScale", m_fAlpha)))
		return;

	if (FAILED(m_pShaderCom->Begin_Shader(SHADER_ALPHA))) // SHADER_WHITE SHADER_ALPHA
		return;

	m_pBufferCom->Render_Buffer();

	if (FAILED(m_pShaderCom->End_Shader()))
		return;
}

HRESULT CEndingBlackBack::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BlackBackTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_BlackBackTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Proto(L"Proto_Shader_Rect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Shader_Rect", pComponent });

	return S_OK;
}

CEndingBlackBack* CEndingBlackBack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _bool bOff, _float fWinCX, _float fWinCY)
{
	CEndingBlackBack* pInstance = new CEndingBlackBack(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_bOff = bOff;
	if (bOff)
	{
		pInstance->m_fAlpha = 1.f;
	}

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("EndingBlackBack Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CEndingBlackBack::Free()
{
	__super::Free();
}
