#include "stdafx.h"
#include "..\Header\Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "PlayerBullet.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: Engine::CGameObject(rhs)
{

}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_ePreState = P_END;
	//m_pTransformCom->m_vScale = { 2.f, 1.f, 1.f };

	return S_OK;
}

Engine::_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_fPicNum * fTimeDelta * 1.5f;

	if (m_fPicNum < m_fFrame)
		m_fFrame = 0.f;

	Key_Input(fTimeDelta);

	// 총알 update
	if (!m_PlayerBulletList.empty())
	{
		int		iResult = 0;
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			iResult = dynamic_cast<CPlayerBullet*>(*iter)->Update_GameObject(fTimeDelta);

			if (1 == iResult)
			{
				//Safe_Delete<CGameObject*>(*iter);
				Safe_Release<CGameObject*>(*iter);
				iter = m_PlayerBulletList.erase(iter);
			}
			else
			{
				++iter;
			}

		}
	}

	CGameObject::Update_GameObject(fTimeDelta);


	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CPlayer::LateUpdate_GameObject()
{
	Motion_Change();

	// 총알 late update
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter : m_PlayerBulletList)
		{
			dynamic_cast<CPlayerBullet*>(iter)->LateUpdate_GameObject();
		}
	}

	__super::LateUpdate_GameObject();

	Height_OnTerrain();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPlayer::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	// 총알 출력
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter : m_PlayerBulletList)
		{
			dynamic_cast<CPlayerBullet*>(iter)->LateUpdate_GameObject();
		}
	}
	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
	CComponent*		pComponent = nullptr;
	
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });


	//PLAYER 텍스처
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_BACK"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_BACK", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_BACK_SMALL"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_BACK_SMALL", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_RIGHT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_RIGHT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_LEFT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_LEFT", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_IDLE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_IDLE", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
		
	return S_OK;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer *	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Player Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	// W,A,S,D 움직임
	_vec3		vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		m_eCurState = P_BACKWALK;
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		m_eCurState = P_IDLEWALK;
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, -10.f, fTimeDelta);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);

		m_eCurState = P_LEFTWALK;
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, -10.f, fTimeDelta);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);

		m_eCurState = P_RIGHTWALK;
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
	}
	else
	{
		m_eCurState = P_IDLE;
	}

	// 총알 발사
	if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	{
		m_eCurState = P_SHOOTWALK;
		m_PlayerBulletList.push_back(CPlayerBullet::Create(m_pGraphicDev));
	}


	//마우스 회전으로 플레이어 각도 바꾸기
	_long	dwMouseMove(0);

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
	}
	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 30.f));
	}

	
}

void CPlayer::Height_OnTerrain()
{
	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

	m_pTransformCom->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

_vec3 CPlayer::Picking_OnTerrain()
{
	CTerrainTex*		pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3());

	CTransform*			pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Terrain", L"Proto_Transform"));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}

void CPlayer::Motion_Change()
{
	/*
		case START:
		m_tFrame.iFrameStart = 0;
		m_tFrame.iFrameEnd = 15;
		m_tFrame.iMotion = 0;
	*/
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;
		switch (m_eCurState)
		{
		case P_IDLE:
			m_fPicNum = 1;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_IDLE"));
			break;
		case P_IDLEWALK:
			m_fPicNum = 11;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_IDLE"));
			break;
		case P_BACKWALK:
			m_fPicNum = 11;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_BACK"));
			break;
		case P_SHOOTWALK:
			m_fPicNum = 11;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_BACK_SMALL"));
			break;
		case P_LEFTWALK:
			m_fPicNum = 8;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_LEFT"));
			break;
		case P_RIGHTWALK:
			m_fPicNum = 8;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Player", L"Proto_PlayerTexture_RIGHT"));
			break;
		}

		m_ePreState = m_eCurState;
	}
}

bool CPlayer::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

void CPlayer::Free()
{
	__super::Free();
}