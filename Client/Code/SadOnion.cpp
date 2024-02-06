#include "stdafx.h"
#include "SadOnion.h"
#include "Export_Utility.h"
#include "Player.h"

CSadOnion::CSadOnion(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CSadOnion::CSadOnion(const CSadOnion& rhs)
	: CItem(rhs)
{
}

CSadOnion::~CSadOnion()
{
}

HRESULT CSadOnion::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_bDead = false;
	m_fFrame = 0;
	m_iCoin = 2;
	m_iUpTimer = 0;

	m_pTransformCom->m_vScale = { 0.4,0.4,0.4 };

	return S_OK;
}

_int CSadOnion::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead == true)
	{
		m_iUpTimer++;
		CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

		_vec3		playerPos;

		playerInfo->Get_Info(INFO_POS, &playerPos);

		m_pTransformCom->Set_Pos(playerPos.x, 2.4, playerPos.z);

		m_bDead = true;
	}

	if (m_bDead == true && m_iUpTimer > 135)
	{
		// Á×À½ Ã³¸®
		return 1;
	}

	Item_Spawn_Action();

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSadOnion::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CSadOnion::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)0);

	m_pBufferCom->Render_Buffer();

}

void CSadOnion::Run_Item_Effect()
{
	if (m_eCurItemPlace != SP_SLOT && m_eCurItemPlace != SP_OBJECT)
	{
		CPlayer::GetInstance()->Set_AttackSpeed(-6);
		CPlayer::GetInstance()->Set_Item_Get_Anim();
		m_bDead = true;
	}
	
}

void CSadOnion::Item_Spawn_Action()
{
	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	_float	fHeight = m_pCalculCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

	_vec3 itemPos;
	m_pTransformCom->Get_Info(INFO_POS, &itemPos);

	if (m_eCurItemPlace == SP_SLOT)
	{

		m_pTransformCom->Set_Pos(itemPos.x + m_vLookVec.x * 0.2, itemPos.y - 0.3, itemPos.z + m_vLookVec.z * 0.2);

		_vec3 temp;
		m_pTransformCom->Get_Info(INFO_POS, &temp);

		if (temp.y <= fHeight + 1)
		{
			m_eCurItemPlace = SP_END;
			m_pTransformCom->Set_Pos(temp.x, fHeight + 1, temp.z);
		}


	}
	else if (m_eCurItemPlace == SP_OBJECT)
	{
		if (m_iTimer < 1)
		{
			//m_fMoveSpeed += 0.01;
			m_pTransformCom->Set_Pos(itemPos.x, itemPos.y + m_fMoveSpeed, itemPos.z);
		}
		else
		{
			_vec3 temp = itemPos - _vec3(0, 0.1, 0);

			if (temp.y <= fHeight + 1)
			{
				m_eCurItemPlace = SP_END;
				m_pTransformCom->Set_Pos(itemPos.x, fHeight + 1, itemPos.z);
			}
			else
			{
				m_pTransformCom->Set_Pos(itemPos.x, itemPos.y - 0.1, itemPos.z);
			}
		}
	}

	m_iTimer++;
}

HRESULT CSadOnion::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_SadOnion"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_SadOnion", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	m_pTransformCom->Set_Pos(m_vSpawnPos);

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CSadOnion::Motion_Change()
{
}

CSadOnion* CSadOnion::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
	CSadOnion* pInstance = new CSadOnion(pGraphicDev);
	srand((unsigned)time(NULL));

	if (spawnspot == 1)
	{
		_vec3 postemp = pos + _vec3(0, 3.f, 0);
		pInstance->Set_SpawnPos(postemp);

		float speed = rand() % 4 + 2;
		speed *= 0.1;
		pInstance->Set_MoveSpeed(speed);
	}
	else
	{
		pInstance->Set_SpawnPos(pos);
	}

	float fAngle = (float)(rand() % 100 - 50);
	_matrix mat;
	_vec3 templook = look;
	D3DXMatrixRotationY(&mat, fAngle);
	D3DXVec3TransformCoord(&templook, &templook, &mat);
	pInstance->Set_LookDir(templook);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SadOnion Create Failed");
		return nullptr;
	}
	pInstance->Set_Item_SpawnSpot(spawnspot);

	return pInstance;
}

void CSadOnion::Free()
{
	__super::Free();
}
