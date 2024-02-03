#include "stdafx.h"
#include "SadOnion.h"
#include "Export_Utility.h"
#include "Player.h"

CSadOnion::CSadOnion(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CSadOnion::CSadOnion(const CSadOnion& rhs)
	: CItem(rhs)
{
}

CSadOnion::~CSadOnion()
{
}

HRESULT CSadOnion::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(12.f, 1.f, 12.f);

	m_bDead = false;
	m_fFrame = 0;
	m_iCoin = 2;

	return S_OK;
}

_int CSadOnion::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	if (m_bDead == true)
	{
		// Á×À½ Ã³¸®
		return 1;
	}


	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CSadOnion::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CSadOnion::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CSadOnion::Run_Item_Effect()
{
	CPlayer::GetInstance()->Set_AttackSpeed(-6);
	CPlayer::GetInstance()->Set_Item_Get_Anim();
	m_bDead = true;
}

HRESULT CSadOnion::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_SadOnion"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_SadOnion", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CSadOnion::Motion_Change()
{
}

CSadOnion* CSadOnion::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot)
{
	CSadOnion* pInstance = new CSadOnion(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("SadOnion Create Failed");
		return nullptr;
	}
	pInstance->Set_Item_SpawnSpot(spawnspot);

	return pInstance;
}

void CSadOnion::Free()
{
	__super::Free();
}
