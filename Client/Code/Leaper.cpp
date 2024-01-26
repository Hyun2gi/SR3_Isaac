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

	return S_OK;
}

_int CLeaper::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 8.f * fTimeDelta * 0.5f;

	if (8.f < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (Check_Time(fTimeDelta) && !m_bMove)
	{
		Change_Dir(fTimeDelta);
		m_bMove = true;
	}
	
	if (m_bMove)
	{
		MoveTo_Random(fTimeDelta);
	}

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CLeaper::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CLeaper::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CLeaper::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_LeaperTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_LeaperTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CLeaper::Change_Dir(const _float& fTimeDelta)
{
	m_iRandNum = rand() % 180;
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));
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
	// 큰 점프를 해 플레이어 위치로 내려찍기
	// 사라졌다가 해당 위치 위로 텔레포트하기?

	
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
