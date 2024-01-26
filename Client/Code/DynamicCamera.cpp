#include "stdafx.h"
#include "..\Header\DynamicCamera.h"

#include "Export_System.h"
#include "Export_Utility.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev)
{
}

CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye,
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

	m_fCameraHeight = 5.f;
	m_fCameraDistance = 5.f;
	m_pTarget = nullptr;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	return S_OK;
}

Engine::_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{

	if (m_pTarget == nullptr)
	{
		m_pTarget = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));
	}

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Chase_Character();
		//마우스 움직임
		Mouse_Fix();
	}

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject()
{
	Engine::CCamera::LateUpdate_GameObject();
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{

	if (Engine::Get_DIKeyState(DIK_O) & 0x80)
	{
		if (m_fCameraDistance == 5)
		{
			m_bChaseInit = true;
			m_fCameraDistance = 15;

		}
		else if (m_fCameraDistance == 15)
		{
			m_bChaseInit = true;
			m_fCameraDistance = 5;
		}
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

void CDynamicCamera::Chase_Character()
{
	CTransform* playerInfo = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		cameraDir;
	_vec3		cameraPos;


	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	// 바라보는 대상은 플레이어
	m_vAt = playerPos;

	// 새로운 카메라 위치
	if (m_bChaseInit == true)
	{
		// 카메라 처음 초기화
		// 플레이어 뒤로 위치하게끔 하고 높이 조절
		m_vCameraPosDir = -(playerDir);
		m_vEye = playerPos + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
	}
	else
	{
		// 회전한만큼 길이와 방향이 벡터에 저장되어 있어서
		// 플레이어에서 해당방향만큼 계산
		m_vEye = playerPos + m_vCameraPosDir;
	}

}

void CDynamicCamera::Mouse_Fix()
{
	POINT	pt{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}


void CDynamicCamera::Mouse_Move()
{
	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	D3DXQUATERNION qRot;

	//_long	dwMouseMove(0);
	_long	dwMouseMoveX, dwMouseMoveY;

	_vec3		playerPos;
	_vec3		playerDir;

	m_pTarget->Get_Info(INFO_POS, &playerPos);
	m_pTarget->Get_Info(INFO_LOOK, &playerDir);

	if ((dwMouseMoveX = Engine::Get_DIMouseMove(DIMS_X)) || (dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y)))
	{
		dwMouseMoveY = Engine::Get_DIMouseMove(DIMS_Y);
		m_bChaseInit = false;

		// 마우스 X축 이동 처리
		_vec3		vUp = { 0.f, 1.f, 0.f };

		_vec3	vLook = m_vEye - m_vAt;

		_matrix		matRotX, matRotY, matTotalRot;
		D3DXQuaternionRotationMatrix(&qRot, &matRotX);

		D3DXQuaternionRotationYawPitchRoll(&qRot, D3DXToRadian(dwMouseMoveX / 10.f), 0, 0);

		D3DXMatrixRotationQuaternion(&matRotX, &qRot);


		// 마우스 Y축 이동 처리
		_vec3		vCross;

		D3DXQuaternionRotationMatrix(&qRot, &matRotY);
		D3DXVec3Cross(&vCross, &m_vUp, &vLook);



		D3DXQuaternionRotationAxis(&qRot, &vCross, D3DXToRadian(dwMouseMoveY / 10.f));
		D3DXMatrixRotationQuaternion(&matRotY, &qRot);

		matTotalRot = matRotX * matRotY;

		D3DXVec3TransformCoord(&vLook, &vLook, &matTotalRot);
		m_vCameraPosDir = vLook;

		// 90도 이상 회전 못하게 하는
		_vec3		newCameraDir = vLook;
		playerDir *= -1;
		D3DXVec3Normalize(&newCameraDir, &newCameraDir);

		float fangle = D3DXToDegree(acos(D3DXVec3Dot(&newCameraDir, &playerDir)));

		if (fangle <= 80 && fangle > 0)
		{
			m_vCameraPosDir = vLook;
		}
		else
		{
			matTotalRot = matRotX;
			D3DXVec3TransformCoord(&vLook, &vLook, &matTotalRot);
			m_vCameraPosDir = vLook;
		}
	}
}


void CDynamicCamera::Free()
{
	Engine::CCamera::Free();
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CDynamicCamera* pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pInstance);
		MSG_BOX("dynamiccamera Create Failed");
		return nullptr;
	}

	return pInstance;
}
