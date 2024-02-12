#include "stdafx.h"
#include "Mom.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMom::CMom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);
}

CMom::CMom(const CMom& rhs)
	: CMonster(rhs)
{
}

CMom::~CMom()
{
}

HRESULT CMom::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { ORIGIN_SCALE, ORIGIN_SCALE, ORIGIN_SCALE };
	m_pTransformCom->Set_Pos(10.f, 50.f, 10.f);

	m_iHp = 1; // 645

	m_iRandNum = 0;
	m_iPicNum = 1;

	m_fCallLimit = 3;
	m_fSpeed = 3.f;

	m_bScaleChange = false;
	m_bScaleReduceX = true;
	m_bScaleReduceY = false;
	m_iScaleCountX = 0;
	m_iScaleCountY = 0;

	m_bReduce = true;
	
	m_eState = MOM_IDLE;

	m_bBoss = true;
	m_eBossType = MOM;

	return S_OK;
}

_int CMom::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	else
		m_fSlowDelta = 0.f;

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	Face_Camera();

	CGameObject::Update_GameObject(m_fSlowDelta);

	if(!m_bDead)
		Mom_Default();
	else
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		vPos.y = 5.f;
		m_pTransformCom->Set_Pos(vPos);

		Engine::Create_Splash_Left(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), L"../Bin/Resource/Texture/Particle/BloodExp_Left/BloodExp_%d.png", 2, 2.f, 25);
		Engine::Create_Splash_Right(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()),L"../Bin/Resource/Texture/Particle/BloodExp_Right/BloodExp_%d.png", 2, 2.f, 25);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 10.f, 30);
		
		return 1;
	}

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CMom::LateUpdate_GameObject()
{
	if (m_bHit)
	{
		m_iHp -= 1;

		m_bHit = false;
		m_bHitColor = true;

		if (0 >= m_iHp)
		{
			// 아이작을 부르는 소리를 지르며 아예 사라짐
			m_bDead = true;
		}
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMom::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMom::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomTexture", pComponent });


	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CMom::Motion_Change()
{
}

void CMom::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CMom::Attack(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (MOM_ATTACK == m_eState)
	{
		if (vPos.y <= LIMIT_Y)
		{
			vPos.y = LIMIT_Y;
			m_eState = MOM_WAIT;
			m_bScaleChange = true; // 애니메이션 조정
		}
		else
			vPos.y -= m_fSpeed;
	}
	else if(MOM_UP)
	{
		Scale_Change();

		if (vPos.y >= 50.f)
		{
			m_eState = MOM_IDLE;
			vPos.y = 50.f;
		}
		else
			vPos.y += m_fSpeed;
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CMom::Mom_Default()
{
	// State에 따른 Check_Time 기준 변화
	switch (m_eState)
	{
	case CMom::MOM_IDLE:
		m_fCallLimit = (_float)(rand() % 10) + 3.f;
		break;

	case CMom::MOM_WAIT:
		m_fCallLimit = 3.f;
		break;
	}

	// 공격 시 애니메이션
	if (m_bScaleChange)
		Animation_Attack();

	if (MOM_IDLE == m_eState)
	{
		if (Check_Time(m_fSlowDelta))
		{
			int iX, iZ;
			iX = (rand() % 20) + 5;
			iZ = (rand() % 20) + 5;

			m_pTransformCom->Set_Pos(iX, 50.f, iZ);

			m_eState = MOM_ATTACK;
		}
	}
	else if (MOM_WAIT == m_eState)
	{
		if (Check_Time(m_fSlowDelta))
		{
			m_eState = MOM_UP;
		}
	}
	else if (MOM_ATTACK == m_eState || MOM_UP == m_eState) // 
		Attack(m_fSlowDelta);

}

void CMom::Animation_Attack()
{
	// 총 두 번 말랑말랑
	if (2 == m_iScaleCountX || 2 == m_iScaleCountY)
	{
		m_bScaleChange = false;
		m_pTransformCom->m_vScale = { ORIGIN_SCALE, ORIGIN_SCALE, ORIGIN_SCALE };

		m_iScaleCountX = 0;
		m_iScaleCountY = 0;

		_vec3 vOriginPos;
		m_pTransformCom->Get_Info(INFO_POS, &vOriginPos);
		vOriginPos.y = LIMIT_Y;
		m_pTransformCom->Set_Pos(vOriginPos);
	}

	_vec3 vScale, vPos;
	vScale = m_pTransformCom->m_vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

#pragma region X 변경

	if (m_bScaleReduceX)
	{
		vScale.x -= 1.f;

		if (vScale.x <= ORIGIN_SCALE - 3.f + m_iScaleCountX)
		{
			m_bScaleReduceX = false;
			vScale.x = ORIGIN_SCALE - 3.f + m_iScaleCountX;
		}
	}
	else
	{
		vScale.x += 1.f;

		if (vScale.x >= ORIGIN_SCALE + 3.f - m_iScaleCountX)
		{
			m_bScaleReduceX = true;
			vScale.x = ORIGIN_SCALE + 3.f - m_iScaleCountX;
			++m_iScaleCountX;
		}
	}

#pragma endregion X 변경

#pragma region Y 변경

	if (m_bScaleReduceY)
	{
		vScale.y -= 1.f;

		if (vScale.y <= ORIGIN_SCALE - 3.f + m_iScaleCountY)
		{
			m_bScaleReduceY = false;
			vScale.y = ORIGIN_SCALE - 3.f + m_iScaleCountY;
		}
		else
			vPos.y -= 0.5f;
	}
	else
	{
		vScale.y += 1.f;

		if (vScale.y >= ORIGIN_SCALE + 3.f - m_iScaleCountY)
		{
			m_bScaleReduceY = true;
			vScale.y = ORIGIN_SCALE + 3.f - m_iScaleCountY;
			++m_iScaleCountY;
		}
		else
			vPos.y += 0.5f;
	}

#pragma endregion Y 변경

	m_pTransformCom->m_vScale = vScale;
	m_pTransformCom->Set_Pos(vPos);
}

void CMom::Scale_Change()
{
	
	if (m_bReduce)
	{
		if (m_pTransformCom->m_vScale.y <= 9.f)
		{
			m_bReduce = false;
		}
		else
			m_pTransformCom->m_vScale.y -= 0.1f;
	}
	else
	{
		if (m_pTransformCom->m_vScale.y >= 10.f)
		{
			m_bReduce = true;
		}
		else
			m_pTransformCom->m_vScale.y += 0.1f;
	}

	m_pTransformCom->m_vScale = { m_pTransformCom->m_vScale.x,
	m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z };
}

CMom* CMom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMom* pInstance = new CMom(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Mom Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMom::Free()
{
	__super::Free();
}
