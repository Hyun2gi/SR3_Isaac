#include "stdafx.h"
#include "Fly.h"

#include "Export_System.h"
#include "Export_Utility.h"

CFly::CFly(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 180;
}

CFly::CFly(const CFly& rhs)
	: CMonster(rhs)
{
}

CFly::~CFly()
{
}

HRESULT CFly::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(_float(rand() % 20), 5.f, _float(rand() % 20));
	m_pTransformCom->m_vScale = { 0.6f, 0.6f, 0.6f };

	m_iHp = 3;

	m_fCallLimit = m_iRandNum % 5 + 2;
	m_fSpeed = 2.f;

	m_ePreState = FLY_END;
	m_bEpicTime = false;
	
	m_eMstType = FLY;

	return S_OK;
}

_int CFly::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	if (m_bEpicTime)
		Engine::Set_TimeDeltaScale(L"Timer_Second", 0.2f);
	else
		Engine::Set_TimeDeltaScale(L"Timer_Second", 1.f);

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
	{
		if (m_bDead)
			return 1;
		else
			m_fFrame = 0.f;
	}

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);

		m_bHit = false;
		m_bHitColor = true;

		if (0 >= m_iHp)
		{
			m_eCurState = FLY_DEAD;
			m_bDead = true;
		}
	}

	if (m_bHitColor)
		Change_Color(m_fSlowDelta);

	// Epic
	if (CPlayer::GetInstance()->Get_EpicLieTiming() && CPlayer::GetInstance()->Get_EpicTargetRun())
		m_bEpicTime = true;

	if (m_bEpicTime)
		Epic_Time();
	else
	{
		m_vOriginScale = m_pTransformCom->m_vAngle;
		Face_Camera();
	}

	if (!m_bDead)
	{
		Face_Camera();

		if (Check_Time(m_fSlowDelta))
			Change_Dir(m_fSlowDelta);

		Move(m_fSlowDelta);
	}

	CGameObject::Update_GameObject(m_fSlowDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CFly::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CFly::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CFly::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FlyTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FlyTexture", pComponent });

	// DEAD
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FlyDeadTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FlyDeadTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CFly::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CFly::FLY_IDLE:
			m_iPicNum = 2;
			m_fFrameSpeed = 3.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_FlyTexture"));
			break;

		case CFly::FLY_DEAD:
			m_iPicNum = 11;
			m_fFrameSpeed = 2.f;
			m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_FlyDeadTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CFly::Change_Dir(const _float& fTimeDelta)
{
	m_iRandNum = rand() % 180;
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));

	m_pTransformCom->Get_Info(INFO_POS, &m_vMoveLook);
}

void CFly::Move(const _float& fTimeDelta)
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
	

	// LOOK 벡터로 이동 X 임의의 벡터를 사용
	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (vPos.x < VTXCNTX - m_pTransformCom->m_vScale.x &&
		vPos.z < VTXCNTZ - m_pTransformCom->m_vScale.z &&
		vPos.x > m_pTransformCom->m_vScale.x &&
		vPos.z > m_pTransformCom->m_vScale.z &&
		vPos.y > 0.f + m_pTransformCom->m_vScale.y &&
		vPos.y < 30.f)
	{
		m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed, fTimeDelta); // 값 조정 필요
	}
	/*D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);*/

	m_eCurState = FLY_IDLE;
}

void CFly::Epic_Time()
{
	m_pTransformCom->m_vAngle = { 0.f, 0.f, 90.f };

	if (!CPlayer::GetInstance()->Get_EpicLieTiming())
	{
		m_pTransformCom->m_vAngle = m_vOriginAngle;
		m_bEpicTime = false;
	}
}

void CFly::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

CFly* CFly::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CFly* pInstance = new CFly(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Fly Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFly::Free()
{
	__super::Free();
}
