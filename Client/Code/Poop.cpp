#include "stdafx.h"
#include "Poop.h"

#include "Export_System.h"
#include "Export_Utility.h"

CPoop::CPoop(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CPoop::CPoop(const CPoop& rhs)
	: CMapObj(rhs)
{
}

CPoop::~CPoop()
{
}

HRESULT CPoop::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 1.f, 10.f);

	m_iLimitHit = 4;

	m_bAni = false;
	m_bReduce = true;

	m_eDropItem = COIN;
	m_eObjType = POOP;

	return S_OK;
}

_int CPoop::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (m_bHit)
		Hit();

	if (m_bAni)
		Change_Scale();

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CPoop::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPoop::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

void CPoop::Change_Scale()
{
	float fScaleY = m_pTransformCom->m_vScale.y;

	if (m_bReduce)
	{
		if (0.9f >= fScaleY)
			m_bReduce = false;

		fScaleY -= 0.1f;
	}
	else
	{
		if (1.f <= fScaleY)
		{
			m_bReduce = true;
			m_bAni = false;
		}

		fScaleY += 0.1f;
	}
	m_pTransformCom->m_vScale = { m_pTransformCom->m_vScale.x,
	fScaleY, m_pTransformCom->m_vScale.z };
}

HRESULT CPoop::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PoopTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PoopTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPoop::Motion_Change()
{
}

void CPoop::Hit()
{
	if (m_iHitCount < m_iLimitHit)
	{
		m_iHitCount += 1;
		m_fFrame += 1.f;
		m_bAni = true;
	}
	else
	{
		m_bDead = true;
		Set_Item_Value();
		Setting_ItemTag();
	}
}

void CPoop::Set_Item_Value()
{
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);
	
	int iResult = rand() % 6;

	switch (iResult)
	{
	case 0:
		m_eDropItem =  COIN;
	case 1:
		m_eDropItem = HEART;
	case 2:
		m_eDropItem = HEART_HALF;
	case 3:
		m_eDropItem = SAD_ONION;
	case 4:
		m_eDropItem = TRINKET;
	case 5:
		m_eDropItem = PILL;
	}
}

CPoop* CPoop::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPoop* pInstance = new CPoop(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Poop Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPoop::Free()
{
	__super::Free();
}
