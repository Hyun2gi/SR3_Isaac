#include "stdafx.h"
#include "Leaper.h"

#include "Export_System.h"
#include "Export_Utility.h"

CLeaper::CLeaper(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev),
	m_pShadow(nullptr)
{
	int iSeed = iID * 5;
	DWORD dwSeed = (iSeed << 16) | (time(NULL) % 1000);
	srand(dwSeed);
}

CLeaper::CLeaper(const CLeaper& rhs)
	: CMonster(rhs),
	m_pShadow(rhs.m_pShadow)
{
}

CLeaper::~CLeaper()
{
}

HRESULT CLeaper::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, HEIGHT_Y, 0.f);
	m_pTransformCom->m_vScale = { 1.2f, 1.2f, 1.2f };

	m_iHp = 3;

	m_fCallLimit = 0.1f;
	m_fSpeed = 8.f;

	m_bMove = false;
	m_bJump = false;
	m_bEpicTime = false;

	m_fPower = 1.8f;
	m_fAccelTime = 0.f;

	int timerrand = rand() % 3;
	m_fSoundTimer = timerrand;

	m_ePreState = LEAPER_END;

	m_eMstType = LEAPER;

	return S_OK;
}

_int CLeaper::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bIsLoadCreatEnd)
		return 0;

	if (!m_bCreate)
	{
		if (Check_Time(fTimeDelta))
			Create_Start_Particle((rand() % 6) + 3);
	}

	m_fSoundTimer += fTimeDelta;
	if (m_fSoundTimer < 4)
	{
		m_fSoundTimer = 0;
		Engine::PlayEffect(L"pacer_walking.wav", SOUND_EFFECT_MON_STOPSUDDEN_SUB, 1.3f);
	}

	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (!m_bTimeScale)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);
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

	if (Check_Time(m_fSlowDelta))
	{
		m_eCurState = LEAPER_UP;
		m_bJump = true;
		Check_TargetPos();
		Engine::PlayEffect(L"Fetus_Jump_0.mp3", SOUND_EFFECT_MON_STOPSUDDEN_SUB, 0.8f);
	}

	if (m_bJump && !m_bEpicTime)
	{
		JumpTo_Player(m_fSlowDelta);
	}
	else
	{
		if (Check_Time(m_fSlowDelta, (rand() % 5) + 5) && !m_bMove)
		{
			Change_Dir(m_fSlowDelta);
			m_bMove = true;
		}

		if (m_bMove)
		{
			MoveTo_Random(m_fSlowDelta);
		}
		m_eCurState = LEAPER_IDLE;
	}

	if (m_pShadow == nullptr) // 아직 그림자가 없으면 생성
		Create_Shadow();
	else
	{
		_vec3 vLeaperPos;
		m_pTransformCom->Get_Info(INFO_POS, &vLeaperPos);
		m_pShadow->Get_TransformCom()->Set_Pos(vLeaperPos.x, 0.1f, vLeaperPos.z);
		m_pShadow->Update_GameObject(m_fSlowDelta);
	}


	CGameObject::Update_GameObject(m_fSlowDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.1f, 20);

		int soundrand = rand() % 4;
		if (soundrand == 0)
		{
			Engine::PlayEffect(L"Meaty_Deaths_1.mp3", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 1)
		{
			Engine::PlayEffect(L"Meaty_Deaths_2.mp3", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 2)
		{
			Engine::PlayEffect(L"Meaty_Deaths_3.mp3", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		else if (soundrand == 3)
		{
			Engine::PlayEffect(L"Meaty_Deaths_4.mp3", SOUND_EFFECT_ETC_STOPSUDDEN, 0.8f);
		}
		return 1;
	}

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CLeaper::LateUpdate_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);
		Engine::Create_Burst(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 0.07f, 10);

		m_bHit = false;
		m_bHitColor = true;

		if (0 >= m_iHp)
			m_bDead = true;
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	if (m_pShadow != nullptr)
		m_pShadow->LateUpdate_GameObject();

	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CLeaper::Render_GameObject()
{
	if (!m_bIsLoadCreatEnd)
		return;

	if (m_pShadow != nullptr)
		m_pShadow->Render_GameObject();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CLeaper::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LeaperTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LeaperTexture", pComponent });

	// UP
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LeaperUpTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LeaperUpTexture", pComponent });

	// DOWN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LeaperDownTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LeaperDownTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CLeaper::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CLeaper::LEAPER_IDLE:
			m_iPicNum = 8;
			m_fFrameSpeed = 0.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_LeaperTexture"));
			break;

		case CLeaper::LEAPER_UP:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_LeaperUpTexture"));
			break;

		case CLeaper::LEAPER_DOWN:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_LeaperDownTexture"));
			break;

		}
		m_ePreState = m_eCurState;
	}
}

void CLeaper::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CLeaper::Change_Dir(const _float& fTimeDelta)
{
	m_iRandNum = rand() % 180;
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));

	m_pTransformCom->Get_Info(INFO_POS, &m_vMoveLook);
}

void CLeaper::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);
}

void CLeaper::MoveTo_Random(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fY = vPos.y + (m_fPower * m_fAccelTime) - (9.f * m_fAccelTime * m_fAccelTime * 0.5f);
	m_fAccelTime += 0.02f;

	if (fY < HEIGHT_Y)
	{
		m_fAccelTime = 0.f;
		fY = HEIGHT_Y;
		m_bMove = false;
	}
	m_pTransformCom->Set_Pos(vPos.x, fY, vPos.z);

	_vec3 vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	D3DXVec3Normalize(&vDir, &vDir);

	if (vPos.x >= VTXCNTX - m_pTransformCom->m_vScale.x - INTERVAL ||
		vPos.z >= VTXCNTZ - m_pTransformCom->m_vScale.z - INTERVAL ||
		vPos.x <= m_pTransformCom->m_vScale.x + INTERVAL ||
		vPos.z <= m_pTransformCom->m_vScale.z + INTERVAL)
	{
		vDir *= -1;
	}
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
}

void CLeaper::JumpTo_Player(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (LEAPER_UP == m_eCurState)
	{
		if (vPos.y >= 10.f)
		{
			m_eCurState = LEAPER_DOWN;
			vPos = m_vTargetPos;
			vPos.y = 30.f;
		}
		else
		{
			//vPos.y += 1.f;
			vPos.y += m_fSpeed * fTimeDelta * 5.f;
		}
	}
	else if (LEAPER_DOWN == m_eCurState)
	{
		if (vPos.y > HEIGHT_Y)
		{
			//vPos.y -= 1.f;
			vPos.y -= m_fSpeed * fTimeDelta * 5.f;
			m_pShadow->Set_Render(true);
		}
		else
		{
			vPos.y = HEIGHT_Y;
			m_eCurState = LEAPER_IDLE;
			m_bJump = false;
			m_pShadow->Set_Render(false);
			Engine::PlayEffect(L"Fetus_Land_1.mp3", SOUND_EFFECT_MON_STOPSUDDEN_SUB, 1.3f);
		}
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CLeaper::Create_Shadow()
{
	m_pShadow = CShadow::Create(m_pGraphicDev);
	m_pShadow->Get_TransformCom()->m_vScale = { SHADOW_SCALE, SHADOW_SCALE, SHADOW_SCALE };
}

void CLeaper::Epic_Time()
{
	Rotation_Epic();

	if (!CPlayer::GetInstance()->Get_EpicLieTiming())
	{
		Engine::Set_TimeDeltaScale(L"Timer_Second", 0.01f);
		m_pTransformCom->m_vAngle = m_vOriginAngle;
		m_bEpicTime = false;
	}
}

CLeaper* CLeaper::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CLeaper* pInstance = new CLeaper(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Leaper Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLeaper::Free()
{
	__super::Free();

	Safe_Release<CShadow*>(m_pShadow);
	m_pShadow = nullptr;
}
