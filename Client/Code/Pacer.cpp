#include "stdafx.h"
#include "Pacer.h"

#include "Export_System.h"
#include "Export_Utility.h"

CPacer::CPacer(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	int iSeed = iID * 5;
	DWORD dwSeed = (iSeed << 16) | (time(NULL) % 1000);
	srand(dwSeed);
}

CPacer::CPacer(const CPacer& rhs)
	: CMonster(rhs)
{
}

CPacer::~CPacer()
{
}

HRESULT CPacer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 0.8f, 0.8f, 0.8f };
	m_pTransformCom->Set_Pos(0.f, HEIGHT_Y, 0.f);

	m_iHp = 3;

	m_fCallLimit = 0.1f;
	m_fSpeed = 1.f;

	m_iPicNum = 10.f;
	m_bEpicTime = false;

	m_eMstType = PACER;

	return S_OK;
}

_int CPacer::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bCreate)
	{
		if (Check_Time(fTimeDelta))
			Create_Start_Particle((rand() % 7) + 5);
	}

	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
	{
		if (m_bEpicTime)
			Engine::Set_TimeDeltaScale(L"Timer_Second", 0.2f);
		else
			Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
	}
	else
		m_fSlowDelta = 0.f;

	m_fFrame += m_iPicNum * m_fSlowDelta;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	if (m_bDead)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.1f, 20);

		int soundrand = rand() % 4;
		if (soundrand == 0)
		{
			Engine::PlayEffect(L"blood fire 3.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 1)
		{
			Engine::PlayEffect(L"blood fire 2.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 2)
		{
			Engine::PlayEffect(L"blood fire 1.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 3)
		{
			Engine::PlayEffect(L"blood fire.wav", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		return 1;
	}

	Check_Outof_Map();

	// Epic
	if (CPlayer::GetInstance()->Get_EpicLieTiming() && CPlayer::GetInstance()->Get_EpicTargetRun())
		m_bEpicTime = true;

	if (m_bEpicTime)
		Epic_Time();
	else
	{
		m_vOriginAngle = m_pTransformCom->m_vAngle;
		Face_Camera();
	}

	if (Check_Time(m_fSlowDelta))
		Change_Dir();

	Move(m_fSlowDelta); // 기본적으로 계속 걸음

	CGameObject::Update_GameObject(m_fSlowDelta);

	Fix_Y(HEIGHT_Y);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CPacer::LateUpdate_GameObject()
{
	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.05f, 5);

		m_bHit = false;
		m_bHitColor = true;

		if (0 >= m_iHp)
		{
			m_bDead = true;
		}
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPacer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CPacer::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PacerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PacerTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPacer::Motion_Change()
{
}

void CPacer::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CPacer::Change_Dir()
{
	m_iRandNum = rand() % 180; // (m_iIndex + 1)
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));

	m_pTransformCom->Get_Info(INFO_POS, &m_vMoveLook);
}

void CPacer::Move(const _float& fTimeDelta)
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	// 멤버 변수 Look 벡터를 사용
	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (vPos.x >= VTXCNTX - m_pTransformCom->m_vScale.x - INTERVAL ||
		vPos.z >= VTXCNTZ - m_pTransformCom->m_vScale.z - INTERVAL ||
		vPos.x <= m_pTransformCom->m_vScale.x + INTERVAL ||
		vPos.z <= m_pTransformCom->m_vScale.z + INTERVAL)
	{
		m_vMoveLook *= -1;
	}

	m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed, fTimeDelta);
}

void CPacer::Epic_Time()
{
	Rotation_Epic();

	if (!CPlayer::GetInstance()->Get_EpicLieTiming())
	{
		m_pTransformCom->m_vAngle = m_vOriginAngle;
		m_bEpicTime = false;
	}
}

CPacer* CPacer::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CPacer* pInstance = new CPacer(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Pacer Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPacer::Free()
{
	__super::Free();
}
