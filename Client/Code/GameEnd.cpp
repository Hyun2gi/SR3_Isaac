#include "stdafx.h"
#include "..\Header\GameEnd.h"

#include "Export_Utility.h"

CGameEnd::CGameEnd(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CGameEnd::CGameEnd(const CGameEnd& rhs)
	: Engine::CUI(rhs)
{
}

CGameEnd::~CGameEnd()
{
}

HRESULT CGameEnd::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.98f));

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

Engine::_int CGameEnd::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bVisible) return 0;

	//점점 보이게
	if (m_bOn && 1 > m_fAlpha)
	{
		m_fAlpha += 0.5f * fTimeDelta;
		
	}
	//점점 안보이게
	else if (!m_bOn && 0 < m_fAlpha)
	{
		m_fAlpha -= 0.5f * fTimeDelta;

		if (0 >= m_fAlpha)
		{
			m_bVisible = false;
		}
	}

	// 알파가 1보다 크거나 같으면 대기 타이머를 시작한다
	if (1 <= m_fAlpha)	m_fWaitTimer -= fTimeDelta;

	if (0 >= m_fWaitTimer)	Set_Off();

	m_fCurFrame += m_iEndFrame * fTimeDelta * m_fAnimSpeed;

	if (m_fCurFrame > m_iEndFrame)
		m_fCurFrame = m_iStartFrame;

	m_iStartFrame = 0;
	m_iEndFrame = m_iAnimFrameCount;

	CUI::Update_GameObject(fTimeDelta);

	return 0;
}

void CGameEnd::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	//CGameObject::LateUpdate_GameObject();
	__super::LateUpdate_GameObject();
}

void CGameEnd::Render_GameObject()
{	
	if (!m_bVisible) return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_pShaderCom, "g_Texture", (_int)m_fCurFrame);

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

HRESULT CGameEnd::Add_Component()
{
	CComponent*		pComponent = nullptr;
		
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GameEndTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_GameEndTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pShaderCom = dynamic_cast<CShader*>(Engine::Clone_Proto(L"Proto_Shader_Rect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Shader_Rect", pComponent });

	return S_OK;
}

CGameEnd * CGameEnd::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	_float fSizeX, _float fSizeY, 
	_float fPosX, _float fPosY, 
	_int iAnimFrameCount, _int iMaxFrameCount,
	_float fWinCX, _float fWinCY)
{
	CGameEnd *	pInstance = new CGameEnd(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("GameEnd Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CGameEnd::Free()
{	
	__super::Free();
}
