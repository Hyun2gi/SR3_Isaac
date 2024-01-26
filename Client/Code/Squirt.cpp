#include "stdafx.h"
#include "Squirt.h"

#include "Export_Utility.h"

CSquirt::CSquirt(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
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
	m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));
	//m_pTransformCom->Set_Pos(3.f, 1.f, 0.f);

	m_iHp = 6;

	m_fCallLimit = 3;
	m_fSpeed = 1.f;

	m_bSliding = false;
	m_fAccel = 10.f;

	return S_OK;
}

_int CSquirt::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 2.f * fTimeDelta;

	if (2.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta))
	{
		//Setting_Direction();
		Check_TargetPos();
		m_bSliding = true;
	}

	if (m_bSliding)
		Sliding(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CSquirt::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CSquirt::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSquirt::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SquirtTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_SquirtTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CSquirt::Sliding(const _float& fTimeDelta)
{
	_vec3	vDir = m_vTargetPos - m_pTransformCom->m_vInfo[INFO_POS];
	_vec3	vPos;

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed * m_fAccel, fTimeDelta);

	m_fAccel -= 0.1;

	if (m_fAccel <= 0.f)
	{
		m_bSliding = false;
		m_fAccel = 1.f;
	}
}

void CSquirt::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);
}

CSquirt* CSquirt::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSquirt* pInstance = new CSquirt(pGraphicDev);

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
