#include "stdafx.h"
#include "Dip.h"

#include "Export_System.h"
#include "Export_Utility.h"

CDip::CDip(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 180;
}

CDip::CDip(const CDip& rhs)
	: CMonster(rhs)
{
}

CDip::~CDip()
{
}

HRESULT CDip::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));

	m_iHp = 3;

	m_fCallLimit = m_iRandNum % 5 + 2;
	m_fSpeed = 10.f;

	m_bSliding = false;
	m_fAccel = 1.f;

	m_ePreState = DIP_END;

	m_eMstType = DIP;

	return S_OK;
}

_int CDip::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

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

		if (0 >= m_iHp)
		{
			_vec3 vPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			//Engine::Create_Explosion(m_pGraphicDev, vPos);
			m_bDead = true;
		}
	}

	if (!m_bDead)
	{
		Face_Camera();

		if (Check_Time(m_fSlowDelta))
		{
			Change_Dir();
			m_bSliding = true;
		}

		if (m_bSliding)
		{
			m_eCurState = DIP_SLIDE;
			Sliding(m_fSlowDelta);
		}
		else
			m_eCurState = DIP_IDLE;
	}

	CGameObject::Update_GameObject(m_fSlowDelta);

	Fix_Y();

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CDip::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CDip::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CDip::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DipTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DipTexture", pComponent });

	// JUMP
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DipSlideTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DipSlideTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CDip::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CDip::DIP_IDLE:
			m_iPicNum = 3;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dip", L"Proto_DipTexture"));
			break;

		case CDip::DIP_SLIDE:
			m_iPicNum = 2;
			m_fFrameSpeed = 2.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Dip", L"Proto_DipSlideTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CDip::Change_Dir()
{
	m_iRandNum = rand() % 180; // 180
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));

	m_pTransformCom->Get_Info(INFO_POS, &m_vMoveLook);
}

void CDip::Sliding(const _float& fTimeDelta)
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (vPos.x < VTXCNTX - m_pTransformCom->m_vScale.x &&
		vPos.z < VTXCNTZ - m_pTransformCom->m_vScale.z &&
		vPos.x > m_pTransformCom->m_vScale.x &&
		vPos.z > m_pTransformCom->m_vScale.z)
	{
		m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed * m_fAccel, fTimeDelta);
	}
	else
	{
		m_vMoveLook *= -1.f;
		m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed * m_fAccel, fTimeDelta);
	}

	m_fAccel -= 0.01;

	if (m_fAccel <= 0.f)
	{
		m_bSliding = false;
		m_fAccel = 1.f;
	}
}

void CDip::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;

}

CDip* CDip::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CDip* pInstance = new CDip(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Dip Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDip::Free()
{
	__super::Free();
}