#include "Camera.h"
#include "Pipeline.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCamera::~CCamera()
{
}

void Engine::CCamera::Free()
{
	CGameObject::Free();
}

HRESULT Engine::CCamera::Ready_GameObject()
{	
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	//CPipeline::MakeViewMatrix(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	//CPipeline::MakeProjMatrix(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	
	return S_OK;
}

Engine::_int Engine::CCamera::Update_GameObject(const _float& fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void Engine::CCamera::LateUpdate_GameObject()
{
	CGameObject::LateUpdate_GameObject();
}


