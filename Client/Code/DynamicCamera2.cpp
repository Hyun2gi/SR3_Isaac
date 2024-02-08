#include "stdafx.h"
#include "..\Header\DynamicCamera2.h"

#include "Export_Utility.h"
#include "Export_System.h"

CTestCamera::CTestCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{
}

CTestCamera::~CTestCamera()
{
}

HRESULT CTestCamera::Ready_GameObject(const _vec3* pEye, 
	const _vec3* pAt, const _vec3* pUp, 
	const _float& fFov, const _float& fAspect, 
	const _float& fNear, const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_vFornt = _vec3(0.f, 0.f, 1.f);

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	//Set_Camera_Pos();
	
	return S_OK;
}

Engine::_int CTestCamera::Update_GameObject(const _float& fTimeDelta)
{	
	m_vLook = m_vAt - m_vEye;

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}

	return iExit;
}

void CTestCamera::LateUpdate_GameObject()
{
	Engine::CCamera::LateUpdate_GameObject();
	//Set_Camera_Pos();
}

void CTestCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt  -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt  += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;
		else
			m_bFix = true;
	}
	else
		m_bCheck = false;

	if (false == m_bFix)
		return;
	


}

void CTestCamera::Mouse_Fix()
{
	POINT	pt{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);


}

void CTestCamera::Mouse_Move()
{
	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	_long	dwMouseMove(0);

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;
		vLook *= -1;

		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vEye = m_vAt + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3		vUp = { 0.f, 1.f, 0.f };

		_vec3	vLook = m_vAt - m_vEye;
		vLook *= -1;

		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vEye = m_vAt + vLook;
	}
}

void CTestCamera::Set_Target(CComponent* pTarget)
{
	m_pTarget = dynamic_cast<CTransform*>(pTarget);
}

void CTestCamera::Set_Camera_Pos()
{
	m_vAt = _vec3(m_pTarget->m_vInfo[INFO_POS].x,
		m_pTarget->m_vInfo[INFO_POS].y + 2.f,
		m_pTarget->m_vInfo[INFO_POS].z);

	//m_vEye = 

	//m_vEye = _vec3(m_pTarget->m_vInfo[INFO_POS].x, 
	//	m_pTarget->m_vInfo[INFO_POS].y + 3.f, 
	//	m_pTarget->m_vInfo[INFO_POS].z - 8.f);
}

void CTestCamera::Free()
{
	Engine::CCamera::Free();
}

CTestCamera* CTestCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CTestCamera*	pInstance = new CTestCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		MSG_BOX("dynamiccamera Create Failed");
		return nullptr;
	}

	return pInstance;
}
