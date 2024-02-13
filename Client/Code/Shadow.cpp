#include "stdafx.h"
#include "Shadow.h"

#include "Export_Utility.h"

CShadow::CShadow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CShadow::CShadow(const CShadow& rhs)
	: CMapObj(rhs)
{
}

CShadow::~CShadow()
{
}

HRESULT CShadow::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(0.f, 0.1f, 0.f);
	m_pTransformCom->m_vAngle.x = 70.f;

	return S_OK;
}

_int CShadow::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CShadow::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CShadow::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CShadow::Add_Component()
{
	return S_OK;
}

CShadow* CShadow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShadow* pInstance = new CShadow(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Spike Create Failed");
		return nullptr;
	}

	return pInstance;

}

void CShadow::Free()
{
	__super::Free();
}
