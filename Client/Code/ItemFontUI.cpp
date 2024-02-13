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

HRESULT CItemFontUI::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = false; // 아이템 습득 시에만?

	m_fAccTimeDelta = 0.f;
	m_fCallLimit = 2.f;
	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_eItemType = ITEM_NONE;

	__super::Ready_GameObject();

	return S_OK;
}

_int CItemFontUI::Update_GameObject(const _float& fTimeDelta)
{
	m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta))
	{
		m_bRender = false; // 얘를 주석치면 애초에 아예 출력이 안 되는?
	}

	if (m_bRender)
	{
		Change_Font();
		//Animation_Font();
	}
	else
	{
		//m_vecFontPos = { 10.f, m_vecEndPos.y };
	}

	// m_bRender가 true이면 좌측에서 생성되어 중앙까지 이동 후 몇 초 뒤에 사라지기
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
		m_pBufferCom->Render_Buffer(); // 뒷배경
		Engine::Render_Font(L"Font_Item", m_szItem, &m_vecEndPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		Engine::Render_Font(L"Font_Item_Detail", m_szItemDetail, &m_vecDetailPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
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
		m_vecEndPos = { 400.f, 180.f };
		m_vecDetailPos = { 200.f, 120.f };
		break;
	case Engine::EPIC:
		lstrcpy(m_szItem, L"EPIC FETUS");
		lstrcpy(m_szItemDetail, L"On-demand air strike");
		m_vecEndPos = { 400.f, 180.f };
		m_vecDetailPos = { 200.f, 120.f };
		break;
	case Engine::SAD_ONION:
		lstrcpy(m_szItem, L"THE SAD ONION");
		lstrcpy(m_szItemDetail, L"Tears up");
		m_vecEndPos = { 190.f, 100.f };
		m_vecDetailPos = { 200.f, 120.f };
		break;
	case Engine::TRINKET:
		lstrcpy(m_szItem, L"WHIP WORM");
		lstrcpy(m_szItemDetail, L"Wooosh!");
		m_vecEndPos = { 400.f, 180.f };
		m_vecDetailPos = { 200.f, 120.f };
		break;
	case Engine::PILL:
		lstrcpy(m_szItem, L"PILL"); // 추후 바꿔야함
		m_vecEndPos = { 400.f, 180.f };
		break;
	default:
		lstrcpy(m_szItem, L"Test"); // 추후 바꿔야함
		lstrcpy(m_szItemDetail, L"BRIMSTONE");
		break;
	}
}

void CItemFontUI::Animation_Font()
{
	if (m_vecFontPos.x < m_vecEndPos.x) // EndPos에 도달할 때까지
	{
		m_vecFontPos.x += 10.f;
	}
	else
		m_vecFontPos = m_vecEndPos;
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
