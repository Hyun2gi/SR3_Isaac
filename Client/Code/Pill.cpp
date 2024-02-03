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
	m_pTransformCom->Set_Pos(7.f, 2.f, 7.f);

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

	return S_OK;
}

_int CPill::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead == true)
	{
		// 죽음 처리
		return 1;
	}

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

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

void CPill::Run_Item_Effect()
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
				break;
			case 2:
				// 무적 상태
				CPlayer::GetInstance()->Set_Item_Get_Anim();
				break;
			case 3:
				CPlayer::GetInstance()->Set_MoveSpeed(4);
				CPlayer::GetInstance()->Set_Item_Get_Anim();
				break;
			case 4:
				CPlayer::GetInstance()->Set_MoveSpeed(-2);
				break;
			}

			m_bDead = true;
			
		}
	}
	else
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
			break;
		case 2:
			// 무적 상태
			CPlayer::GetInstance()->Set_Item_Get_Anim();
			break;
		case 3:
			CPlayer::GetInstance()->Set_MoveSpeed(4);
			CPlayer::GetInstance()->Set_Item_Get_Anim();
			break;
		case 4:
			CPlayer::GetInstance()->Set_MoveSpeed(-2);
			break;
		}

		m_bDead = true;
	}
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
}

void CPill::Motion_Change()
{
}

CPill* CPill::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos)
{
	CPill* pInstance = new CPill(pGraphicDev);
	//정확한 위치 설정
	pInstance->Set_SpawnPos(pos);

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
