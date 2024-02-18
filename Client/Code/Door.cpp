#include "stdafx.h"
#include "Door.h"

#include "Export_System.h"
#include "Export_Utility.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev), m_iStageNumKey(0)
{
}

CDoor::CDoor(const CDoor& rhs)
	: CMapObj(rhs), m_iStageNumKey(0)
{
}

CDoor::~CDoor()
{
}

void CDoor::Set_Theme(int iID)
{
	switch (iID)
	{
	case 0:
		m_eTheme = THEME_NORMAL;
		break;
	case 1:
		m_eTheme = THEME_BOSS;
		break;
	case 2:
		m_eTheme = THEME_ARCADE;
		break;
	case 3:
		m_eTheme = THEME_GOLD;
		break;
	case 4:
		m_eTheme = THEME_DEVIL;
		break;
	default:
		break;
	}
}

void CDoor::Set_Open()
{
	if(m_bOpen == false)
	{
		m_bOpen = true; // 방 문 열리는 사운드
		Engine::PlayEffect(L"DoorOpen.wav", SOUND_EFFECT_ETC_ALLPLAY, 1.f);

		Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 3.f, 20);
	}
}

HRESULT CDoor::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 2.f, 5.f); // 
	m_pTransformCom->m_vScale = { 3.f, 3.f, 3.f };

	m_bOpen = false;
	m_bCollision = false;

	m_fCallLimit = 1.f;

	m_bScaleChange = false;
	m_bScaleReduce = true;
	m_iScaleCount = 0;

	m_ePreState = DOOR_END;

	return S_OK;
}

_int CDoor::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bCreate)
	{
		Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 3.f, 20);

		m_bCreate = true;
	}

	if (m_bOpen)
	{
		m_eCurState = DOOR_OPEN;
	}
	else
		m_eCurState = DOOR_CLOSE;

	if (m_bScaleChange)
		Animation_Change();

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CDoor::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CDoor::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CDoor::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// STAGE _ OPEN
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_NormalDoorOpenTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_NormalDoorOpenTexture", pComponent });

	// STAGE _ CLOSE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_NormalDoorCloseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_NormalDoorCloseTexture", pComponent });

	// STAGE _ OPENING
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_NormalDoorOpneingTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_NormalDoorOpneingTexture", pComponent });

	// BOSS _ OPEN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BossDoorOpenTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_BossDoorOpenTexture", pComponent });

	// BOSS _ CLOSE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BossDoorCloseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_BossDoorCloseTexture", pComponent });

	// ARCADE _ OPEN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ArcadeDoorOpenTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ArcadeDoorOpenTexture", pComponent });

	// ARCADE _ CLOSE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ArcadeDoorCloseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ArcadeDoorCloseTexture", pComponent });

	// DEVIL _ OPEN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DevilDoorOpenTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DevilDoorOpenTexture", pComponent });

	// DEVIL _ CLOSE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DevilDoorCloseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DevilDoorCloseTexture", pComponent });

	// TREASURE _ OPEN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TreasureDoorOpenTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TreasureDoorOpenTexture", pComponent });

	// TREASURE _ CLOSE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TreasureDoorCloseTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_TreasureDoorCloseTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CDoor::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		wstring wstrProto = L"Proto_";
		wstring wstrTexture, wstrTheme;

		wstrTheme.assign(m_strStageKey.begin(), m_strStageKey.end());
		//Proto_BaseDoorOpenTexture

		switch (m_eCurState)
		{
		case CDoor::DOOR_OPEN:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			wstrTexture = wstrProto + wstrTheme + L"DoorOpenTexture";
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Door", wstrTexture.c_str()));
			m_bScaleChange = true; // 애니메이션 적용
			break;

		case CDoor::DOOR_CLOSE:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			wstrTexture = wstrProto + wstrTheme + L"DoorCloseTexture";
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Door", wstrTexture.c_str()));
			break;

		case CDoor::DOOR_OPENING: // 리소스 작업할 시간이 남는다면 구현
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CDoor::Animation_Change()
{
	if (2 == m_iScaleCount)
	{
		m_bScaleChange = false;
		m_pTransformCom->m_vScale = { ORIGIN_SCALE, ORIGIN_SCALE, ORIGIN_SCALE };
		m_iScaleCount = 0;
	}

	_vec3 vScale;
	vScale = m_pTransformCom->m_vScale;

	if (m_bScaleReduce)
	{
		vScale.x -= 0.1f;

		if (vScale.x <= ORIGIN_SCALE - 0.5f)
		{
			m_bScaleReduce = false;
			vScale.x = ORIGIN_SCALE - 0.5f;
		}
	}
	else
	{
		vScale.x += 0.1f;

		if (vScale.x >= ORIGIN_SCALE + 0.5f)
		{
			m_bScaleReduce = true;
			vScale.x = ORIGIN_SCALE + 0.5f;
			++m_iScaleCount;
		}
	}
	m_pTransformCom->m_vScale = vScale;

}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDoor* pInstance = new CDoor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Door Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();
}
