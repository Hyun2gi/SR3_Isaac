#include "stdafx.h"
#include "MapParts.h"

#include "Export_Utility.h"

CMapParts::CMapParts(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev), m_pMapIcon(nullptr)
{
}

CMapParts::CMapParts(const CMapParts& rhs)
	: Engine::CUI(rhs), m_pMapIcon(rhs.m_pMapIcon)
{
}

CMapParts::~CMapParts()
{
}

HRESULT CMapParts::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_bFrontRoom = false; // 눈앞의 방인지 여부
	m_bNowRoom = false;
	m_bCheckRoom = false;

	m_iRoomNumber = 0; // 방 번호

	m_eRoomState = ROOM_END;

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CMapParts::Update_GameObject(const _float& fTimeDelta)
{
	//m_fCurFrame = 2.3f; // 방 상태에 따라 다르게 표기

	CUI::Update_GameObject(fTimeDelta);

	if (m_bFrontRoom)
	{
		m_fCurFrame += 3 * fTimeDelta * 2.f;

		if (3.f < m_fCurFrame)
		{
			m_fCurFrame = 0.f;
		}
	}
	else
	{
		if (m_bNowRoom)
			m_fCurFrame = 2.f;
		else // 현재 방이 아닐 때
		{
			if (m_bCheckRoom)
				m_fCurFrame = 1.f;
			else
				m_fCurFrame = 0.f;
		}
	}


	if (m_pMapIcon != nullptr)
	{
		Setting_Icon();
		m_pMapIcon->Update_GameObject(fTimeDelta);
	}
	else
		Create_Icon();

	return 0;
}

void CMapParts::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pMapIcon != nullptr)
		m_pMapIcon->LateUpdate_GameObject();
}

void CMapParts::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	m_pBufferCom->Render_Buffer();

	if (m_pMapIcon != nullptr)
		m_pMapIcon->Render_GameObject();
}

HRESULT CMapParts::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniMapPartsTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MiniMapPartsTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

void CMapParts::Setting_Icon()
{
	// gold : 5 / arcade : 3 / devil : 9 / monstro : 4 / mom : 10

	if (3 == m_iRoomNumber) // Arcade
		m_pMapIcon->Set_IconType(1);
	else if (4 == m_iRoomNumber) // Monstro
		m_pMapIcon->Set_IconType(4);
	else if (5 == m_iRoomNumber) // Gold
		m_pMapIcon->Set_IconType(0);
	else if (9 == m_iRoomNumber) // Devil
		m_pMapIcon->Set_IconType(2);
	else if (10 == m_iRoomNumber) // Mom
		m_pMapIcon->Set_IconType(3);
	else
		m_pMapIcon->Set_IconType(99);
}

void CMapParts::Create_Icon()
{

	m_pMapIcon = CMapIcon::Create(m_pGraphicDev, 30, 30, m_fPosX, m_fPosY, 1, 1);
}

CMapParts* CMapParts::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CMapParts* pInstance = new CMapParts(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MapParts Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMapParts::Free()
{
	__super::Free();

	Safe_Release<CMapIcon*>(m_pMapIcon);
	m_pMapIcon = nullptr;
}
