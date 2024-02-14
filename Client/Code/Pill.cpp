#include "stdafx.h"
#include "Pill.h"
#include "Export_Utility.h"
#include "Player.h"

CPill::CPill(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CPill::CPill(const CPill& rhs)
	: CItem(rhs)
{
}

CPill::~CPill()
{
}

HRESULT CPill::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 0.4,0.4,0.4 };
	m_bDead = false;

	// 랜덤으로 설정
	srand((unsigned)time(NULL));
	m_iPicNum = rand() % 5;

	m_iCoin = 5;
	// 약 효과
	switch (m_iPicNum)
	{
	case 0:
		m_eCurState = PILL_0;
		break;
	case 1:
		m_eCurState = PILL_1;
		break;
	case 2:
		m_eCurState = PILL_2;
		break;
	case 3:
		m_eCurState = PILL_3;
		break;
	case 4:
		m_eCurState = PILL_4;
		break;
	}
	m_iUpTimer = 0;
	m_bBadItem = false;

	m_eItemType = PILL;

	return S_OK;
}

_int CPill::Update_GameObject(const _float& fTimeDelta)
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

	if (m_bBadItem)
	{
		if (m_bDead == true && m_iUpTimer > 80)
		{
			// 죽음 처리
			return 1;
		}
	}
	else
	{
		if (m_bDead == true && m_iUpTimer > 135)
		{
			// 죽음 처리
			return 1;
		}
	}

	Item_Spawn_Action();

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CPill::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPill::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_iPicNum);

	m_pBufferCom->Render_Buffer();
}

bool CPill::Run_Item_Effect()
{
	if (m_eCurItemPlace == SP_SHOP)
	{
		// 구매해야할 경우
		if (CPlayer::GetInstance()->Get_Coin() >= m_iCoin)
		{
			CPlayer::GetInstance()->Set_Coin(-m_iCoin);
			srand((unsigned)time(NULL));
			int iEffectNum = rand() % 5;

			// 약 효과
			switch (iEffectNum)
			{
			case 0:
				// 최대피
				CPlayer::GetInstance()->Set_To_MaxHp();
				CPlayer::GetInstance()->Set_Item_Get_Anim();
				break;
			case 1:
				CPlayer::GetInstance()->Set_Hp(-1);
				m_bBadItem = true;
				CPlayer::GetInstance()->Set_Item_Get_Anim_Bad();
				break;
			case 2:
				// hp+1
				CPlayer::GetInstance()->Set_Hp(1);
				CPlayer::GetInstance()->Set_Item_Get_Anim();
				break;
			case 3:
				CPlayer::GetInstance()->Set_MoveSpeed(1);
				CPlayer::GetInstance()->Set_Item_Get_Anim();
				break;
			case 4:
				CPlayer::GetInstance()->Set_MoveSpeed(-2);
				m_bBadItem = true;
				CPlayer::GetInstance()->Set_Item_Get_Anim_Bad();
				break;
			}

			m_bDead = true;
			return true;
		}
	}
	else if (m_eCurItemPlace != SP_SLOT && m_eCurItemPlace != SP_OBJECT)
	{
		srand((unsigned)time(NULL));
		int iEffectNum = rand() % 5;

		// 약 효과
		switch (iEffectNum)
		{
		case 0:
			// 최대피
			CPlayer::GetInstance()->Set_To_MaxHp();
			CPlayer::GetInstance()->Set_Item_Get_Anim();
			break;
		case 1:
			CPlayer::GetInstance()->Set_Hp(-1);
			m_bBadItem = true;
			CPlayer::GetInstance()->Set_Item_Get_Anim_Bad();
			break;
		case 2:
			// 무적 상태
			m_bBadItem = true;
			CPlayer::GetInstance()->Set_Item_Get_Anim_Bad();
			break;
		case 3:
			CPlayer::GetInstance()->Set_MoveSpeed(4);
			CPlayer::GetInstance()->Set_Item_Get_Anim();
			break;
		case 4:
			CPlayer::GetInstance()->Set_MoveSpeed(-2);
			m_bBadItem = true;
			CPlayer::GetInstance()->Set_Item_Get_Anim_Bad();
			break;
		}

		m_bDead = true;
		return true;
	}

	return false;
}

void CPill::Item_Spawn_Action()
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

HRESULT CPill::Add_Component()
{
	CComponent * pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_Pill"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_Pill", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	m_pTransformCom->Set_Pos(m_vSpawnPos);

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPill::Motion_Change()
{
}

CPill* CPill::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
	CPill* pInstance = new CPill(pGraphicDev);
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
		MSG_BOX("Pill Create Failed");
		return nullptr;
	}
	pInstance->Set_Item_SpawnSpot(spawnspot);

	return pInstance;
}

void CPill::Free()
{
	__super::Free();
}
