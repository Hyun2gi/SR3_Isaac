#include "stdafx.h"
#include "Fire.h"

#include "Export_System.h"
#include "Export_Utility.h"

CFire::CFire(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CFire::CFire(const CFire& rhs)
	: CMapObj(rhs)
{
}

CFire::~CFire()
{
}

HRESULT CFire::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iLimitHit = 4;

	m_iPicNum = 6;
	m_fFrameSpeed = 1.f;

	m_bHit = false;

	m_eObjID = MOBJID_FIRE;

	return S_OK;
}

_int CFire::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	CGameObject::Update_GameObject(fTimeDelta);

	if (m_bHit)
	{
		Hit();
		m_bHit = false;
	}

	m_pCalculator->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CFire::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CFire::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

void CFire::Change_Scale()
{
	m_pTransformCom->m_vScale =
	{
		m_pTransformCom->m_vScale.x - 0.2f,
		m_pTransformCom->m_vScale.y - 0.2f,
		m_pTransformCom->m_vScale.z - 0.2f
	};

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	
	m_pTransformCom->Set_Pos(vPos.x, vPos.y - 0.09f, vPos.z);
}

HRESULT CFire::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FireTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_FireTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CFire::Hit()
{
	if (m_iHitCount < m_iLimitHit)
	{
		m_iHitCount += 1;
		Change_Scale();
	}
	else
	{
		// 아이템 드랍
		m_bDead = true;
		Set_Item_Value();
		Setting_ItemTag();
	}
}

void CFire::Set_Item_Value()
{
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);

	int iResult = rand() % 6;

	switch (iResult)
	{
	case 0:
		m_eDropItem = COIN;
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

CFire* CFire::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFire* pInstance = new CFire(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Fire Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFire::Free()
{
	__super::Free();
}
