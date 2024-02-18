#include "stdafx.h"
#include "..\Header\PlayerLeg.h"


#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"

CPlayerLeg::CPlayerLeg(LPDIRECT3DDEVICE9 pGraphicDev)
	:Engine::CGameObject(pGraphicDev)
{
}

CPlayerLeg::CPlayerLeg(const CPlayerLeg& rhs)
	: Engine::CGameObject(rhs)
{
}

CPlayerLeg::~CPlayerLeg()
{
}

HRESULT CPlayerLeg::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fFrame = 0.f;
	m_iPicNum = 0; // 이미지 개수
	m_fSpriteSpeed = 0.9f;
	m_iPicNum = 0;

	m_ePreLegState = P_LEG_END;
	m_eCurLegState = P_LEG_FRONT_IDLE;

	m_fMoveSpeed = 10;

	return S_OK;
}

_int CPlayerLeg::Update_GameObject(const _float& fTimeDelta)
{

	m_fFrame += m_iPicNum * fTimeDelta * m_fSpriteSpeed;

	if (m_iPicNum < m_fFrame)
	{
		m_fFrame = 0.f;
	}

	// 플레이어에게 다리 상태 받기
	int legstate = CPlayer::GetInstance()->Get_Azazel_Anim();
	switch (legstate)
	{
	case 0 :
		m_eCurLegState = P_LEG_FRONT_IDLE;
		break;
	case 1 :
		m_eCurLegState = P_LEG_FRONT;
		break;
	case 2:
		m_eCurLegState = P_LEG_BACK;
		break;
	case 3:
		m_eCurLegState = P_LEG_LEFT;
		break;
	case 4:
		m_eCurLegState = P_LEG_RIGHT;
		break;
	case 5:
		m_eCurLegState = P_LEG_NO_SHOW;
		break;
	}


	// 플레이어 다리 회전과 위치 받기
	_vec3 position, rotation;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerLookDir, playerRightDir;
	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerLookDir);
	playerInfo->Get_Info(INFO_LOOK, &playerRightDir);
	
	_vec3		legRightDir;
	m_pTransformCom->Get_Info(INFO_RIGHT, &legRightDir);

	if (m_eCurLegState == P_LEG_FRONT)
	{
		playerPos += (playerLookDir *0.3);
	}
	else if (m_eCurLegState == P_LEG_BACK)
	{
		playerPos -= (playerLookDir*0.1);
	}
	else if (m_eCurLegState == P_LEG_LEFT) 
	{
		playerPos += (playerLookDir * 0.04);
		playerPos += (legRightDir *0.03);
		//playerPos += (-playerRightDir * 0.2);
	}
	else if (m_eCurLegState == P_LEG_RIGHT)
	{
		/*playerPos += (playerLookDir * 0.01);
		playerPos += (playerRightDir * 0.1);*/
		playerPos += (playerLookDir * 0.04);
		playerPos += (legRightDir * 0.03);
	}
	else if (m_eCurLegState == P_LEG_FRONT_IDLE)
	{
		playerPos += (playerLookDir * 0.3);
	}
	else if (m_eCurLegState == P_LEG_NO_SHOW)
	{
		// ATTACKED
		_vec3 vDir, vPos;
		vDir = _vec3(playerLookDir.x, 0, playerLookDir.z);
		vDir *= -1;
		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformCom->Get_Info(INFO_POS, &vPos);

		m_pTransformCom->Move_Pos(&vDir, 3, fTimeDelta * 3);

	}
	


	m_pTransformCom->Set_Pos(playerPos);
	m_pTransformCom->m_vAngle = playerInfo->m_vAngle;


	CGameObject::Update_GameObject(fTimeDelta);

	if (m_eCurLegState != P_LEG_NO_SHOW)
	{
		// noshow면 보여지지 않음
		Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);
	}
	
	return 0;
}

void CPlayerLeg::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPlayerLeg::Render_GameObject()
{
	if (m_eCurLegState != P_LEG_NO_SHOW)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pTextureCom->Set_Texture((_uint)m_fFrame);

		m_pBufferCom->Render_Buffer();
	}
}

HRESULT CPlayerLeg::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_LEG_LEFT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_LEG_LEFT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_LEG_RIGHT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_LEG_RIGHT", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_FRONTLEG"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_FRONTLEG", pComponent });


	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });


	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPlayerLeg::Motion_Change()
{
	if (m_ePreLegState != m_eCurLegState)
	{
		m_fFrame = 0.f;
		switch (m_eCurLegState)
		{
		case P_LEG_FRONT_IDLE:
			m_iPicNum = 1;
			m_fSpriteSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_PlayerTexture_AZAZEL_FRONTLEG"));
			break;
		case P_LEG_FRONT:
			m_iPicNum =10;
			m_fSpriteSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_PlayerTexture_AZAZEL_FRONTLEG"));
			break;
		case P_LEG_BACK:
			m_iPicNum = 10;
			m_fSpriteSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_PlayerTexture_AZAZEL_FRONTLEG"));
			break;
		case P_LEG_LEFT:
			m_iPicNum = 10;
			m_fSpriteSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_PlayerTexture_AZAZEL_LEG_LEFT"));
			break;
		case P_LEG_RIGHT:
			m_iPicNum = 10;
			m_fSpriteSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_PlayerTexture_AZAZEL_LEG_RIGHT"));
			break;
		}

		m_ePreLegState = m_eCurLegState;
	}
}

CPlayerLeg* CPlayerLeg::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerLeg* pInstance = new CPlayerLeg(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("PlayerLeg Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlayerLeg::Free()
{
	__super::Free();
}
