#include "stdafx.h"
#include "ItemFontUI.h"

#include "Export_System.h"
#include "Export_Utility.h"

CItemFontUI::CItemFontUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CUI(pGraphicDev)
{
}

CItemFontUI::CItemFontUI(const CItemFontUI& rhs)
    : Engine::CUI(rhs)
{
}

CItemFontUI::~CItemFontUI()
{
}

HRESULT CItemFontUI::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = false; // 아이템 습득 시에만?

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	__super::Ready_GameObject();
	//Compute_ProjectionMatrix();

	return S_OK;
}

_int CItemFontUI::Update_GameObject(const _float& fTimeDelta)
{
	m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);

	return 0;
}

void CItemFontUI::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CItemFontUI::Render_GameObject()
{

	_tchar	m_szLoading[128];
	ZeroMemory(m_szLoading, sizeof(m_szLoading));
	lstrcpy(m_szLoading, L"Test");
	Engine::Render_Font(L"Font_Default", m_szLoading, &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if (m_bRender)
		m_pBufferCom->Render_Buffer(); // 고민 해볼 부분
}

HRESULT CItemFontUI::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MenuTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MenuTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

CItemFontUI* CItemFontUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CItemFontUI* pInstance = new CItemFontUI(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("ItemFontUI Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CItemFontUI::Free()
{
	__super::Free();
}
