#include "stdafx.h"
#include "Dip.h"

#include "Export_Utility.h"

CDip::CDip(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 180;
}

CDip::CDip(const CDip& rhs)
	: CMonster(m_pGraphicDev)
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

	return S_OK;
}

_int CDip::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 3.f * fTimeDelta;

	if (3.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta))
	{
		Change_Dir();
		m_bSliding = true;
	}

	if(m_bSliding)
		Sliding(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CDip::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CDip::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CDip::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DipTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_DipTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CDip::Change_Dir()
{
	m_iRandNum = rand() % 180;
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));
}

void CDip::Sliding(const _float& fTimeDelta)
{
	// 해당 방향 벡터로 빠르게 이동
	// 일정 시간 동안 움직이고, 실제 슬라이딩처럼 점점 느려지게 구현?
	_vec3		vDir, vPos;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed * m_fAccel, fTimeDelta);

	m_fAccel -= 0.01;

	if (m_fAccel <= 0.f)
	{
		m_bSliding = false;
		m_fAccel = 1.f;
	}
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