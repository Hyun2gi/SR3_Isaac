#include "stdafx.h"
#include "Mom.h"

#include "Export_Utility.h"

CMom::CMom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);
}

CMom::CMom(const CMom& rhs)
	: CMonster(rhs)
{
}

CMom::~CMom()
{
}

HRESULT CMom::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 10.f, 10.f, 0.f };
	//m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));
	m_pTransformCom->Set_Pos(10.f, 50.f, 10.f);

	m_iHp = 645; // 조정 필요 할지도

	m_iRandNum = 0;
	m_iPicNum = 1;

	m_fCallLimit = 3;
	m_fSpeed = 3.f;

	m_bReduce = true;
	
	m_eState = MOM_IDLE;

	return S_OK;
}

_int CMom::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	// 상태에 따라 callLimit을 바꾸는 것은?
	switch (m_eState)
	{
	case CMom::MOM_IDLE:
		m_fCallLimit = (_float)(rand() % 10) + 3.f;
		break;

	case CMom::MOM_WAIT:
		m_fCallLimit = 3.f;
		break;
	}

	if (MOM_IDLE == m_eState)
	{
		if (Check_Time(fTimeDelta))
		{
			int iX, iZ;
			iX = rand() % 30;
			iZ = rand() % 30;

			m_pTransformCom->Set_Pos(iX, 50.f, iZ);

			m_eState = MOM_ATTACK;
		}
	}
	else if (MOM_WAIT == m_eState)
	{
		if (Check_Time(fTimeDelta))
		{
			m_eState = MOM_UP;
		}
	}
	else if (MOM_ATTACK == m_eState || MOM_UP == m_eState) // 
		Attack(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CMom::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMom::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMom::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomTexture", pComponent });


	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CMom::Motion_Change()
{
}

void CMom::Attack(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (MOM_ATTACK == m_eState)
	{
		if (vPos.y <= 9.f)
		{
			vPos.y = 9.f;
			m_eState = MOM_WAIT;
		}
		else
			vPos.y -= m_fSpeed;
	}
	else if(MOM_UP)
	{
		Scale_Change();

		if (vPos.y >= 50.f)
		{
			m_eState = MOM_IDLE;
			vPos.y = 50.f;
		}
		else
			vPos.y += m_fSpeed;
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CMom::Scale_Change()
{
	
	if (m_bReduce)
	{
		if (m_pTransformCom->m_vScale.y <= 9.f)
		{
			m_bReduce = false;
		}
		else
			m_pTransformCom->m_vScale.y -= 0.1f;
	}
	else
	{
		if (m_pTransformCom->m_vScale.y >= 10.f)
		{
			m_bReduce = true;
		}
		else
			m_pTransformCom->m_vScale.y += 0.1f;
	}

	m_pTransformCom->m_vScale = { m_pTransformCom->m_vScale.x,
	m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z };
}

CMom* CMom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMom* pInstance = new CMom(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Mom Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMom::Free()
{
	__super::Free();
}
