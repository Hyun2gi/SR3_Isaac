#include "stdafx.h"
#include "Squirt.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "Dip.h"

CSquirt::CSquirt(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	int iSeed = iID * 5;
	DWORD dwSeed = (iSeed << 16) | time(NULL) % 1000;
	srand(dwSeed);
}

CSquirt::CSquirt(const CSquirt& rhs)
	: CMonster(rhs)
{
}

CSquirt::~CSquirt()
{
}

HRESULT CSquirt::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, HEIGHT_Y, 0.f);
	m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };

	m_iHp = 6;

	m_fCallLimit = 0.1f;
	m_fSpeed = 4.f;

	m_bSliding = false;
	m_bBounceWall = false;
	m_bEpicTime = false;
	m_fAccel = 8.f;

	m_ePreState = SQU_END;

	m_eMstType = SQUIRT;

	return S_OK;
}

_int CSquirt::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsLoadCreatEnd)
		return 0;

	if (!m_bCreate)
	{
		if (Check_Time(fTimeDelta))
			Create_Start_Particle((rand() % 3) + 3);
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


	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

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

	if (Check_Time(m_fSlowDelta) && !m_bSliding)
	{
		Check_TargetPos();
		m_bSliding = true;
		//squirt_run.wav
		Engine::PlayEffect(L"squirt_run.wav", SOUND_EFFECT_MON_STOPSUDDEN, 0.8f);
	}

	if (m_bSliding)
	{
		Sliding(m_fSlowDelta);
		m_eCurState = SQU_SLIDE;
	}
	else
		m_eCurState = SQU_IDLE;

	CGameObject::Update_GameObject(m_fSlowDelta);

	Fix_Y(HEIGHT_Y);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.7f, 30, 2.f);

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

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CSquirt::LateUpdate_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);
		Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.7f, 10, 2.f);

		m_bHit = false;
		m_bHitColor = true;

		if (0 >= m_iHp)
			m_bDead = true;
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CSquirt::Render_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CSquirt::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SquirtTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SquirtTexture", pComponent });

	// SLIDE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SquirtSlideTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SquirtSlideTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CSquirt::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CSquirt::SQU_IDLE:
			m_iPicNum = 2;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SquirtTexture"));
			break;

		case CSquirt::SQU_SLIDE:
			m_iPicNum = 3;
			m_fFrameSpeed = 0.3f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_SquirtSlideTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CSquirt::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CSquirt::Sliding(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (vPos.x >= VTXCNTX - m_pTransformCom->m_vScale.x - INTERVAL ||
		vPos.z >= VTXCNTZ - m_pTransformCom->m_vScale.z - INTERVAL ||
		vPos.x <= m_pTransformCom->m_vScale.x + INTERVAL ||
		vPos.z <= m_pTransformCom->m_vScale.z + INTERVAL)
	{
		m_vMoveLook *= -1;
	}

	m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed * m_fAccel, fTimeDelta);

	m_fAccel -= 0.1;

	if (m_fAccel <= 0.f)
	{
		m_bSliding = false;
		m_fAccel = 10.f;
	}
}

void CSquirt::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);

	m_vMoveLook = m_vTargetPos - m_pTransformCom->m_vInfo[INFO_POS];
}

void CSquirt::Epic_Time()
{
	//m_pTransformCom->m_vAngle = { 0.f, 0.f, D3DXToRadian(90) };
	Rotation_Epic();

	if (!CPlayer::GetInstance()->Get_EpicLieTiming())
	{
		Engine::Set_TimeDeltaScale(L"Timer_Second", 0.01f);
		m_pTransformCom->m_vAngle = m_vOriginAngle;
		m_bEpicTime = false;
	}
}

void CSquirt::Create_Dip(CLayer* pLayer)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	for (int i = 0; i < 2; ++i)
	{
		CDip* pDip = CDip::Create(m_pGraphicDev, i);
		dynamic_cast<CDip*>(pDip)->Get_Transform()->Set_Pos(vPos.x + (i*0.5f), vPos.y, vPos.z);
		pDip->Set_MyLayer(m_vecMyLayer[0]);
		pLayer->Add_GameObject(L"Fly", pDip);
	}
}

CSquirt* CSquirt::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CSquirt* pInstance = new CSquirt(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Squirt Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSquirt::Free()
{
	__super::Free();
}
