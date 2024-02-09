#include "stdafx.h"
#include "Leaper.h"

#include "Export_System.h"
#include "Export_Utility.h"

CLeaper::CLeaper(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	int iSeed = iID * 5;
	DWORD dwSeed = (iSeed << 16) | (time(NULL) % 1000);
	srand(dwSeed);
}

CLeaper::CLeaper(const CLeaper& rhs)
	: CMonster(rhs)
{
}

CLeaper::~CLeaper()
{
}

HRESULT CLeaper::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, 1.f, 0.f);
	m_pTransformCom->m_vScale = { 1.2f, 1.2f, 1.2f };

	m_iHp = 3;

	m_fCallLimit = (rand() % 6) + 3;
	m_fSpeed = 8.f;

	m_bMove = false;
	m_bJump = false;

	m_fPower = 1.8f;
	m_fAccelTime = 0.f;

	m_ePreState = LEAPER_END;

	m_eMstType = LEAPER;

	return S_OK;
}

_int CLeaper::Update_GameObject(const _float& fTimeDelta)
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	m_fFrame += m_iPicNum * m_fSlowDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);

		m_bHit = false;

		if (0 >= m_iHp)
		{
			m_bDead = true;
			_vec3 vPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()));
		}
	}

	if (m_bHitColor)
		Change_Color(fTimeDelta);

	Face_Camera();

	if (Check_Time(m_fSlowDelta))
	{
		m_eCurState = LEAPER_UP;
		m_bJump = true;
		Check_TargetPos();
	}

	if (m_bJump)
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

	CGameObject::Update_GameObject(m_fSlowDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead)
		return 1;

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CLeaper::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CLeaper::Render_GameObject()
{
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

	if (fY < 1.f)
	{
		m_fAccelTime = 0.f;
		fY = 1.f;
		m_bMove = false;
	}
	m_pTransformCom->Set_Pos(vPos.x, fY, vPos.z);

	_vec3 vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	D3DXVec3Normalize(&vDir, &vDir);

	/*if (vPos.x < VTXCNTX - m_pTransformCom->m_vScale.x - 5.f &&
		vPos.z < VTXCNTZ - m_pTransformCom->m_vScale.z  - 5.f &&
		vPos.x > m_pTransformCom->m_vScale.x + 5.f &&
		vPos.z > m_pTransformCom->m_vScale.z + 5.f)
	{
		
		m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
	}
	else
	{

		m_pTransformCom->Move_Pos(&-vDir, m_fSpeed, fTimeDelta);
	}*/
	//m_pTransformCom->Get_Info(INFO_POS, &vPos);

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
		if (vPos.y > 1.f)
		{
			//vPos.y -= 1.f;
			vPos.y -= m_fSpeed * fTimeDelta * 5.f;
		}
		else
		{
			vPos.y = 1.f;
			m_eCurState = LEAPER_IDLE;
			m_bJump = false;
		}
	}
	m_pTransformCom->Set_Pos(vPos);
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
}
