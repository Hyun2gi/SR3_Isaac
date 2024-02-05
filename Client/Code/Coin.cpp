#include "stdafx.h"
#include "Coin.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Player.h"

CCoin::CCoin(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CCoin::CCoin(const CCoin& rhs)
	:CItem(rhs)
{
}

CCoin::~CCoin()
{
}

HRESULT CCoin::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	
	m_pTransformCom->m_vScale = { 0.5,0.5,0.5 };
	m_ePreState = COIN_END;
	m_eCurState = COIN_IDLE;
	m_fFrame = 0;
	m_fSpriteSpeed = 2;
	m_iDelay = 0;
	m_iTimer = 0;
	m_fMoveSpeed = 0.9;

	m_bDead = false;

	return S_OK;
}

_int CCoin::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eCurState == COIN_IDLE)
	{
		if (m_iDelay == 0)
		{
			m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;
		}
		else
		{
			m_fFrame = 0.f;
			m_iDelay--;
		}

		if (m_fPicNum < m_fFrame)
		{
			m_fFrame = 0.f;
			m_iDelay = 70;
		}
	}
	else if(m_eCurState == COIN_GET)
	{
		m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;

		if (m_fPicNum < m_fFrame)
		{
			// 없애기
			m_bDead = true;
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Item_Spawn_Action();
	
	if (m_bDead == true)
	{
		// 죽음 처리
		return 1;
	}

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);
	return 0;
}

void CCoin::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CCoin::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

void CCoin::Run_Item_Effect()
{
	// 튀어나올때 먹지 않도록
	if (m_eCurItemPlace != SP_SLOT && m_eCurItemPlace != SP_OBJECT)
	{
		m_eCurState = COIN_GET;
		// 첫 이미지부터 시작
		m_fFrame = 0;

		CPlayer::GetInstance()->Set_Coin(1);
	}
	
}

void CCoin::Item_Spawn_Action()
{
	// SP_IDLE, // 그냥 있음
	//	SP_SLOT, // 슬롯 머신에서 나옴 (많이 튀어나옴)
	//	SP_OBJECT, // OBJECT에서 나옴 (조금 튀어나옴)
	//	SP_SHOP, // 샵에서 구매

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	_float	fHeight = m_pCalculCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

	_vec3 itemPos;
	m_pTransformCom->Get_Info(INFO_POS, &itemPos);

	if (m_eCurItemPlace == SP_SLOT)
	{
		
		m_pTransformCom->Set_Pos(itemPos.x + m_vLookVec.x*0.2, itemPos.y - 0.3, itemPos.z + m_vLookVec.z * 0.2);

		_vec3 temp;
		m_pTransformCom->Get_Info(INFO_POS, &temp);

		if (temp.y <= fHeight + 1)
		{
			m_eCurItemPlace = SP_END;
			m_pTransformCom->Set_Pos(temp.x, fHeight + 1, temp.z);
		}

		
	}
	else if(m_eCurItemPlace == SP_OBJECT)
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

HRESULT CCoin::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// COIN 그냥상태
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoinTexture_IDLE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CoinTexture_IDLE", pComponent });

	// COIN 먹고 EFFECT
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoinTexture_EFFECT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CoinTexture_EFFECT", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	m_pTransformCom->Set_Pos(m_vSpawnPos);

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CCoin::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;
		switch (m_eCurState)
		{
		case COIN_IDLE:
			m_fPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Coin", L"Proto_CoinTexture_IDLE"));
			break;
		case COIN_GET:
			m_fPicNum = 5;
			m_fSpriteSpeed = 3.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Coin", L"Proto_CoinTexture_EFFECT"));
			break;
		}

		m_ePreState = m_eCurState;
	}
}

CCoin* CCoin::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
	CCoin* pInstance = new CCoin(pGraphicDev);
	//정확한 위치 설정

	srand((unsigned)time(NULL));
	
	if (spawnspot == 1)
	{
		_vec3 postemp = pos + _vec3(0, 3.f, 0);
		pInstance->Set_SpawnPos(postemp);

		float speed = rand() % 4 +2;
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
		MSG_BOX("COIN Create Failed");
		return nullptr;
	}
	pInstance->Set_Item_SpawnSpot(spawnspot);

	return pInstance;
}

void CCoin::Free()
{
	__super::Free();
}
