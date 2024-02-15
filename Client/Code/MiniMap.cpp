#include "stdafx.h"
#include "MiniMap.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "StageLoadMgr.h"
#include "MapIcon.h"


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

CMapParts* CMiniMap::Get_MapParts(_int iIndex)
{
	vector<CMapParts*>::iterator iter = m_vecRoomParts.begin();

	for (int i = 0; i < iIndex; ++i)
	{
		++iter;
	}

	return (*iter); // 해당 인덱스의 MapParts 반환
}

HRESULT CMiniMap::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bRender = true; // 필요할 지? 연출 끝나면 나오는 식으로 (추후 false로)

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

	// 현재 방 정보 string strType = CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_strType;
	// 를 받아온 후 값에 따라서 현재 방 설정

	if (!m_vecRoomParts.empty())
	{
		Setting_NowRoom();
		// 씬 전환을 하고 나서도 해당 객체의 bool 값들이 그대로여야 함
		Setting_CheckRoom();

		// 플레이어의 Look벡터를 받아와서 해당 벡터를 길게 늘렸을 때 벽의 값을 받아오기?

		for (auto& iter : m_vecRoomParts)
			iter->Update_GameObject(fTimeDelta);

	}
	else
		Create_RoomParts();

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
		// 방 순서대로
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

void CMiniMap::Setting_NowRoom()
{
	// 현재 있는 방의 텍스쳐를 밝게 변화시키기
	for (auto& iter : m_vecRoomParts)
	{
		if (m_iNowRoomNum == iter->Get_RoomNumber())
		{
			iter->Set_NowRoom(true);
		}
	}
}

void CMiniMap::Setting_CheckRoom()
{
	//CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(m_iCurStageKey).m_bClear = true;

	int iStageKey(1);

	if (!CStageLoadMgr::GetInstance()->Get_StageInfo_Map().empty())
	{

		for (auto& iter : m_vecRoomParts)
		{
			if (CStageLoadMgr::GetInstance()->Get_StageInfo_Map().at(iStageKey).m_bClear)
			{
				iter->Set_CheckRoom();
				if(iter->Get_Icon() != nullptr)
					iter->Get_Icon()->Set_Render();
			}
			++iStageKey;
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
