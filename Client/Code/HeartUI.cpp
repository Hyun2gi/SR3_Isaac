#include "stdafx.h"
#include "HeartUI.h"

#include "Export_Utility.h"

CHeartUI::CHeartUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CHeartUI::CHeartUI(const CHeartUI& rhs)
	: Engine::CUI(rhs)
{
}

CHeartUI::~CHeartUI()
{
}

HRESULT CHeartUI::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 2;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CHeartUI::Update_GameObject(const _float& fTimeDelta)
{
	//m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);

	return 0;
}

void CHeartUI::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CHeartUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CHeartUI::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// Boss HP Bar
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerHeartTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerHeartTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

CHeartUI* CHeartUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CHeartUI* pInstance = new CHeartUI(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("BossHP Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CHeartUI::Free()
{
	__super::Free();
}
