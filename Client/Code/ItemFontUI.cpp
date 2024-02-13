#include "stdafx.h"
#include "ItemFontUI.h"

#include "Export_System.h"
#include "Export_Utility.h"

CItemFontUI::CItemFontUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CUI(pGraphicDev)
{
	ZeroMemory(m_szItem, sizeof(m_szItem));
	ZeroMemory(m_szItemDetail, sizeof(m_szItemDetail));
}

CItemFontUI::CItemFontUI(const CItemFontUI& rhs)
    : Engine::CUI(rhs)
{
}

CItemFontUI::~CItemFontUI()
{
}

void CItemFontUI::Set_PillState(int iState)
{
	switch (iState)
	{
	case 0:
		m_ePillState = PILL_0;
		break;
	case 1:
		m_ePillState = PILL_1;
		break;
	case 2:
		m_ePillState = PILL_2;
		break;
	case 3:
		m_ePillState = PILL_3;
		break;
	case 4:
		m_ePillState = PILL_4;
		break;
	default:
		break;
	}
}

HRESULT CItemFontUI::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = false; // ¾ÆÀÌÅÛ ½Àµæ ½Ã¿¡¸¸?

	m_fAccTimeDelta = 0.f;
	m_fCallLimit = 2.f;
	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_eItemType = ITEM_NONE; // ITEM_NONE
	m_ePillState = PILL_NONE; // PILL_NONE

	__super::Ready_GameObject();

	return S_OK;
}

_int CItemFontUI::Update_GameObject(const _float& fTimeDelta)
{
	m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta))
	{
		m_bRender = false;
	}

	if (m_bRender)
	{
		Change_Font();
	}

	if (PILL == m_eItemType)
		m_vecFontPos.y = PILL_FONT_Y; // PILL_FONT_Y
	else
		m_vecFontPos.y = FONT_Y;
	
	m_vecDetailPos.y = DETAIL_Y;

	//m_bRender = true;

	return 0;
}

void CItemFontUI::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CItemFontUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if (m_bRender)
	{
		m_pBufferCom->Render_Buffer(); // µÞ¹è°æ
		Engine::Render_Font(L"NeoµÕ±Ù¸ð Code", m_szItem, &m_vecFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		Engine::Render_Font(L"NeoµÕ±Ù¸ð", m_szItemDetail, &m_vecDetailPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CItemFontUI::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

bool CItemFontUI::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		//m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

void CItemFontUI::Change_Font()
{
	switch (m_eItemType)
	{
	case Engine::BRIM:
		lstrcpy(m_szItem, L"BRIMSTONE");
		lstrcpy(m_szItemDetail, L"Blood Laser Barrage");
		m_vecFontPos.x = 265.f;
		m_vecDetailPos.x = 250.f;
		break;
	case Engine::EPIC:
		lstrcpy(m_szItem, L"EPIC FETUS");
		lstrcpy(m_szItemDetail, L"On-demand air strike");
		m_vecFontPos.x = 245.f;
		m_vecDetailPos.x = 240.f;
		break;
	case Engine::SAD_ONION:
		lstrcpy(m_szItem, L"THE SAD ONION");
		lstrcpy(m_szItemDetail, L"Tears up");
		m_vecFontPos.x = 210.f;
		m_vecDetailPos.x = 330.f; // 320
		break;
	case Engine::TRINKET:
		lstrcpy(m_szItem, L"WHIP WORM");
		lstrcpy(m_szItemDetail, L"Wooosh!");
		m_vecFontPos.x = 270.f;
		m_vecDetailPos.x = 350.f;
		break;
	default:
		break;
	}

	switch (m_ePillState)
	{
	case CItemFontUI::PILL_0:
		lstrcpy(m_szItem, L"MAX HP");
		lstrcpy(m_szItemDetail, L"");
		m_vecFontPos.x = 300.f;
		break;
	case CItemFontUI::PILL_1:
		lstrcpy(m_szItem, L"HP DOWN");
		lstrcpy(m_szItemDetail, L"");
		m_vecFontPos.x = 300.f;
		break;
	case CItemFontUI::PILL_2:
		lstrcpy(m_szItem, L"POWER PILL!");
		lstrcpy(m_szItemDetail, L"");
		m_vecFontPos.x = 230.f;
		break;
	case CItemFontUI::PILL_3:
		lstrcpy(m_szItem, L"SPEED UP");
		lstrcpy(m_szItemDetail, L"");
		m_vecFontPos.x = 280.f;
		break;
	case CItemFontUI::PILL_4:
		lstrcpy(m_szItem, L"SPEED DOWN");
		lstrcpy(m_szItemDetail, L"");
		m_vecFontPos.x = 250.f;
		break;
	default:
		break;
	}
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
