#include "stdafx.h"
#include "Leaper.h"

#include "Export_Utility.h"

CLeaper::CLeaper(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 180;
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
	m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));

	m_iHp = 3;

	m_fCallLimit = 2;
	m_fSpeed = 8.f;

	m_bMove = false;
	m_bJump = false;

	m_fPower = 1.8f;
	m_fAccelTime = 0.f;

	m_ePreState = LEAPER_END;

	return S_OK;
}

_int CLeaper::Update_GameObject(const _float& fTimeDelta)
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	Face_Camera();

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(fTimeDelta);

		m_bHit = false;

		if (0 >= m_iHp)
		{
			m_bDead = true;
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta, 5.f))
	{
		m_eCurState = LEAPER_UP;
		m_bJump = true;
		Check_TargetPos();
	}

	if (m_bJump)
	{
		JumpTo_Player(fTimeDelta);
	}
	else
	{
		if (Check_Time(fTimeDelta) && !m_bMove)
		{
			Change_Dir(fTimeDelta);
			m_bMove = true;
		}

		if (m_bMove)
		{
			MoveTo_Random(fTimeDelta);
		}
		m_eCurState = LEAPER_IDLE;
	}

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

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
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Leaper", L"Proto_LeaperTexture"));
			break;

		case CLeaper::LEAPER_UP:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Leaper", L"Proto_LeaperUpTexture"));
			break;

		case CLeaper::LEAPER_DOWN:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Leaper", L"Proto_LeaperDownTexture"));
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
			vPos.y = 50.f;
		}
		else
		{
			vPos.y += 1.f;
		}
	}
	else if (LEAPER_DOWN == m_eCurState)
	{
		if (vPos.y > 1.f)
		{
			vPos.y -= 1.f;
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
