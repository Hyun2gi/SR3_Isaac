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
		Setting_Direction();
		m_bSliding = true;
		// 슬라이딩 전에 순간 중앙으로 이동 하는 듯함
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
	_vec3		vDir, vPos;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
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

void CSquirt::Setting_Direction()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

	_vec3 vPlayerPos;
	m_pTargetTransCom->Get_Info(INFO_POS, &vPlayerPos); // 플레이어의 현재 위치(O)

	//// this의 월드 행렬
	//_matrix matTrans = *(m_pTransformCom->Get_WorldMatrix());

	//// 플레이어 위치를 향해 바라봄
	//_matrix matRot = *(m_pTransformCom->Compute_LookAtTarget(&vPlayerPos));

	//// 플레이어를 향해 바라보는 것으로 월드 행렬 연산
	//m_pTransformCom->Set_WorldMatrix(&(matRot*matTrans));


	// Compute_LookAtTarget( ) 내부 코드 옮긴 것
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);

	_vec3 vDir = vPlayerPos - vPos;
	_vec3 vAxis = *D3DXVec3Cross(&vAxis, &vUp, &vDir);

	_vec3 vUp2;
	_matrix matRot;

	float fDot = D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir), D3DXVec3Normalize(&vUp2, &vPos));
	float fAngle = acos(fDot);

	D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);
	m_pTransformCom->Set_WorldMatrix(&matRot);
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
