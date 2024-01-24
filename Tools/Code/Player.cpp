#include "../Include/stdafx.h"
#include "..\Header\Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

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

	//m_pTransformCom->m_vScale = { 2.f, 1.f, 1.f };


	return S_OK;
}

Engine::_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{

	Key_Input(fTimeDelta);
	
	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CPlayer::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	Height_OnTerrain();
}

void CPlayer::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

}

HRESULT CPlayer::Add_Component()
{
	CComponent*		pComponent = nullptr;
	
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture", pComponent });

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

void CPlayer::Free()
{	
	__super::Free();
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	_vec3		vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	if (GetAsyncKeyState(VK_UP))
	{
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransformCom->Move_Pos(&vDir, -10.f, fTimeDelta);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
	}	

	if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	{
		_vec3	vPickPos = Picking_OnTerrain();

		m_pTransformCom->Move_Terrain(&vPickPos, fTimeDelta, 5.f);
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
