#include "stdafx.h"
#include "MiniMap.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CMiniMap::CMiniMap(const CMiniMap& rhs)
	: Engine::CUI(rhs)
{
}

CMiniMap::~CMiniMap()
{
}

HRESULT CMiniMap::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = true; // �ʿ��� ��? ���� ������ ������ ������ (���� false��)

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	m_iNowRoomNum = 0;

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CMiniMap::Update_GameObject(const _float& fTimeDelta)
{		
	m_fCurFrame = 0.f;

	CUI::Update_GameObject(fTimeDelta);

	// ���� �� ���� string strType = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_strType;
	// �� �޾ƿ� �� ���� ���� ���� �� ����
	Setting_RoomType();

	if (!m_vecRoomParts.empty())
	{
		for (auto& iter : m_vecRoomParts)
			iter->Update_GameObject(fTimeDelta);

		//Setting_RoomType();
		Setting_NowRoom();
	}
	else
	{
		Create_RoomParts();
	}

	return 0;
}

void CMiniMap::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (!m_vecRoomParts.empty())
	{
		for (auto& iter : m_vecRoomParts)
			iter->LateUpdate_GameObject();
	}
}

void CMiniMap::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if (m_bRender)
		m_pBufferCom->Render_Buffer();

	if (!m_vecRoomParts.empty())
	{
		for (auto& iter : m_vecRoomParts)
			iter->Render_GameObject();
	}

}

HRESULT CMiniMap::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

//#pragma region Texture
//
//	// Icon
//	// Gold
//	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Arcade
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Mom
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Monstro
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Devil
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//
//	// Heart
//	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FontBackGroundTexture"));
//	NULL_CHECK_RETURN(pComponent, E_FAIL);
//	m_mapComponent[ID_STATIC].insert({ L"Proto_FontBackGroundTexture", pComponent });
//	// �̰� ������ ���� ������
//
//#pragma endregion Texture


	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniMapToolTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MiniMapToolTexture", pComponent });


	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

void CMiniMap::Create_RoomParts()
{
	if (m_vecRoomParts.empty())
	{
		// �� �������
		CMapParts* pParts_First = CMapParts::Create(m_pGraphicDev, 40, 40, 317.f, 200.f, 1, 1);
		pParts_First->Set_RoomNumber(1);
		m_vecRoomParts.push_back(pParts_First);

		CMapParts* pParts_Sec = CMapParts::Create(m_pGraphicDev, 40, 40, 317.f, 220.f, 1, 1);
		pParts_Sec->Set_RoomNumber(2);
		m_vecRoomParts.push_back(pParts_Sec);

		CMapParts* pParts_Third = CMapParts::Create(m_pGraphicDev, 40, 40, 317.f, 240.f, 1, 1);
		pParts_Third->Set_RoomNumber(3);
		m_vecRoomParts.push_back(pParts_Third);

		CMapParts* pParts_Fourth = CMapParts::Create(m_pGraphicDev, 40, 40, 294.f, 220.f, 1, 1);
		pParts_Fourth->Set_RoomNumber(4);
		m_vecRoomParts.push_back(pParts_Fourth);

		CMapParts* pParts_Fifth = CMapParts::Create(m_pGraphicDev, 40, 40, 317.f, 260.f, 1, 1);
		pParts_Fifth->Set_RoomNumber(5);
		m_vecRoomParts.push_back(pParts_Fifth);

		CMapParts* pParts_Sixth = CMapParts::Create(m_pGraphicDev, 40, 40, 340.f, 200.f, 1, 1);
		pParts_Sixth->Set_RoomNumber(6);
		m_vecRoomParts.push_back(pParts_Sixth);

		CMapParts* pParts_Seventh = CMapParts::Create(m_pGraphicDev, 40, 40, 340.f, 220.f, 1, 1);
		pParts_Seventh->Set_RoomNumber(7);
		m_vecRoomParts.push_back(pParts_Seventh);

		CMapParts* pParts_Eighth = CMapParts::Create(m_pGraphicDev, 40, 40, 363.f, 220.f, 1, 1);
		pParts_Eighth->Set_RoomNumber(8);
		m_vecRoomParts.push_back(pParts_Eighth);

		CMapParts* pParts_Ninth = CMapParts::Create(m_pGraphicDev, 40, 40, 363.f, 240.f, 1, 1);
		pParts_Ninth->Set_RoomNumber(9);
		m_vecRoomParts.push_back(pParts_Ninth);

		CMapParts* pParts_Tenth = CMapParts::Create(m_pGraphicDev, 40, 40, 363.f, 260.f, 1, 1);
		pParts_Tenth->Set_RoomNumber(10);
		m_vecRoomParts.push_back(pParts_Tenth);

	}
}

void CMiniMap::Setting_RoomType()
{
	// m_strRoomTypeNow : �÷��̾ ��ġ�� ��
	if (m_strRoomTypeNow == "Normal")
	{
		m_iNowRoomNum = 1; // �ӽ÷�
	}
	else if (m_strRoomTypeNow == "Boss")
	{
		m_iNowRoomNum = 10;
	}
	else if (m_strRoomTypeNow == "Arcade")
	{
		m_iNowRoomNum = 3;
	}
	else if (m_strRoomTypeNow == "Devil")
	{
		m_iNowRoomNum = 9;
	}
	else if (m_strRoomTypeNow == "Basement") // Treasure
	{
		m_iNowRoomNum = 5;
	}
	else if (m_strRoomTypeNow == "Challenge")
	{

	}
}

void CMiniMap::Setting_NowRoom()
{
	// ���� �ִ� ���� �ؽ��ĸ� ��� ��ȭ��Ű��
	for (auto& iter : m_vecRoomParts)
	{
		if (m_iNowRoomNum == iter->Get_RoomNumber())
		{
			iter->Set_NowRoom(true);
		}
		else
		{
			iter->Set_NowRoom(false);
		}
	}
}

CMiniMap* CMiniMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CMiniMap* pInstance = new CMiniMap(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MiniMap Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMiniMap::Free()
{
	__super::Free();
}
