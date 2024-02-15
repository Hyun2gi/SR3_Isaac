#include "stdafx.h"
#include "BrimStone.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "Player.h"

CBrimStone::CBrimStone(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CBrimStone::CBrimStone(const CBrimStone& rhs)
	: CItem(rhs)
{
}

CBrimStone::~CBrimStone()
{
}

HRESULT CBrimStone::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 0.7,0.7,0.7 };

	m_bDead = false;
	m_fFrame = 0;
	m_iCoin = 4;
	m_iUpTimer = 0;

	m_eItemType = BRIM;

	return S_OK;
}

_int CBrimStone::Update_GameObject(const _float& fTimeDelta)
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

void CBrimStone::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CBrimStone::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CBrimStone::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_BrimstoneItem"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_BrimstoneItem", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	m_pTransformCom->Set_Pos(m_vSpawnPos);

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CBrimStone::Motion_Change()
{
}

bool CBrimStone::Run_Item_Effect()
{
	if (m_eCurItemPlace != SP_SLOT && m_eCurItemPlace != SP_OBJECT)
	{
		CPlayer::GetInstance()->Set_BulletType(2);
		CPlayer::GetInstance()->Set_Item_Get_Anim();
		Engine::PlayEffect(L"Get_Brimstone.wav", SOUND_EFFECT_ITEM_STOPSUDDEN, 1.f);
		m_bDead = true;

		return true;
	}
	return false;
}

void CBrimStone::Item_Spawn_Action()
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

CBrimStone* CBrimStone::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
	CBrimStone* pInstance = new CBrimStone(pGraphicDev);
	//srand((unsigned)time(NULL));

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
		MSG_BOX("BrimStone Create Failed");
		return nullptr;
	}
	pInstance->Set_Item_SpawnSpot(spawnspot);

	return pInstance;
}

void CBrimStone::Free()
{
	__super::Free();
}
