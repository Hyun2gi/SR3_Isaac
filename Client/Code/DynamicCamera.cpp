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
	m_fCameraDistance = 4.f;
	m_pTarget = nullptr;
	m_ePreState = C_END;
	m_eCurState = C_PLAYERCHASE;

	m_bShake = false;
	m_bMove = false;


	m_fAngleY = 0;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	return S_OK;
}

Engine::_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{

	if (m_pTarget == nullptr)
	{
		m_pTarget = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));
	}

	ShakeByPosition(fTimeDelta);
	ShakeByRotation(fTimeDelta);

	// 해당위치로 이동
	MoveToTarget(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Chase_Character();
		Whole_Land_Show();
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

	if (Engine::Get_DIKeyState(DIK_N) & 0x80)
	{
		if (m_bShake == false)
		{
			//OnShakeCameraPos(float shakeTime, float shakeIntensity)
			OnShakeCameraPos(0.5, 2);
			//OnShakeCameraRot(2, 2);
		}
	}

	if (Engine::Get_DIKeyState(DIK_M) & 0x80)
	{
		if (m_bMove == false)
		{
			m_bMove = true;
			CTransform* playerInfo = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"));

			_vec3		playerPos;
			_vec3		playerDir;
			_vec3		targetpos;

			playerInfo->Get_Info(INFO_POS, &playerPos);
			playerInfo->Get_Info(INFO_LOOK, &playerDir);

			D3DXVec3Normalize(&playerDir, &playerDir);
			playerDir *= -3;
			// 바라보는 대상은 플레이어
			targetpos = playerPos + playerDir;
			OnMoveTargetCamera(3,7, targetpos, false);
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
	if (m_eCurState == C_PLAYERCHASE)
	{
		CTransform* playerInfo = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

		_vec3		playerPos;
		_vec3		playerDir;
		_vec3		cameraDir;
		_vec3		cameraPos;


		playerInfo->Get_Info(INFO_POS, &playerPos);
		playerInfo->Get_Info(INFO_LOOK, &playerDir);

		D3DXVec3Normalize(&playerDir, &playerDir);
		// 바라보는 대상은 플레이어
		m_vAt = playerPos + playerDir * 2;

		// 새로운 카메라 위치
		if (m_bChaseInit == true)
		{
			// 카메라 처음 초기화
			// 플레이어 뒤로 위치하게끔 하고 높이 조절
			m_vCameraPosDir = -(playerDir);
			m_vEye = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
		}
		else
		{
			// 회전한만큼 길이와 방향이 벡터에 저장되어 있어서
			// 플레이어에서 해당방향만큼 계산
			m_vEye = m_vAt + m_vCameraPosDir;
		}
	}
	

}

void CDynamicCamera::Whole_Land_Show()
{
	if (m_eCurState == C_WHOLELAND)
	{
		m_vEye = _vec3(0,50.f,0);
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

		// 플레이어 - 마우스 X축 이동했을때 회전
		//dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"))->Rotation(ROT_Y, D3DXToRadian(dwMouseMoveX / 10.f));
		// 플레이어 - 마우스 Y축 이동했을때 회전
		//dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"))->Rotation(ROT_Y, D3DXToRadian(dwMouseMoveX / 30.f));

		// 마우스 Y축 이동 처리
		_vec3		vCross;

		D3DXQuaternionRotationMatrix(&qRot, &matRotY);
		D3DXVec3Cross(&vCross, &m_vUp, &vLook);
		
		if (m_fAngleY - (dwMouseMoveY / 10.f) > -20)
		{
			m_fAngleY -= (dwMouseMoveY / 10.f);
			D3DXQuaternionRotationAxis(&qRot, &vCross, -D3DXToRadian(dwMouseMoveY / 10.f));
			D3DXMatrixRotationQuaternion(&matRotY, &qRot);

			matTotalRot = matRotX * matRotY;
		}
		else
		{
			matTotalRot = matRotX;
		}
		

		D3DXVec3TransformCoord(&vLook, &vLook, &matTotalRot);
		m_vCameraPosDir = vLook;

		// 90도 이상 회전 못하게 하는
		_vec3		newCameraDir = vLook;

		D3DXVec3Normalize(&playerDir, &playerDir);
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

// position 변화로 움직이기..
// 흔들림 짧게 줄때 쓰기 적당하지만 계속적인 쉐이킹에는 사용x
void CDynamicCamera::ShakeByPosition(const _float& fTimeDelta)
{
	if (m_eCurState == C_SHAKING_POS)
	{
		//잠깐 맞았을때 살짝 흔들리는거
		
		if (m_fShakeTime > 0.0f)
		{
			
			if (m_vGoalPosition == m_vEye)
			{
				float FLOAT_MAX = 1;
				float FLOAT_MIN = 0;

				srand((unsigned)time(NULL));

				// -1~1 사이의 난수 생성

				/*float randx = (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN)));
				float randy = (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN)));
				float randz = (float)(rand()) / ((float)(RAND_MAX / (FLOAT_MAX - FLOAT_MIN)));

				float randx = (float)(rand() % 2 - 1);
				float randy = (float)(rand() % 2 - 1);
				float randz = (float)(rand() % 2 - 1);
				_vec3 randompos = _vec3(randx, randy, randz);

				D3DXVec3Normalize(&randompos, &randompos);*/

				_vec3 templook;
				templook = _vec3(m_vAt.x, 0, m_vAt.z) - _vec3(m_vStartAtPosition.x, 0, m_vStartAtPosition.x);
				templook = m_vAt - m_vStartAtPosition;

				D3DXVec3Normalize(&templook, &templook);


				_vec3 moveDir;
				D3DXVec3Cross(&moveDir, &(_vec3(0, 1, 0)), &templook);
				
				//테스트추가
				//moveDir = _vec3(moveDir.x, 0, moveDir.z);
				// x축으로 직선일때는 양옆으로
				// moveDir = _vec3(1, 0, 0);
				//moveDir = _vec3(0.5, 0, 0.5);

				D3DXVec3Normalize(&moveDir, &moveDir);

				

				if (m_iShakeNum % 2 == 0)
				{
					moveDir *= -1;
				}
				m_iShakeNum++;
				
				// 목표위치
				// moveDir과 곱해주는 값은 작아야함!!
				//m_vGoalPosition = m_vStartEyePosition + moveDir*0.2;
				m_vGoalPosition = m_vStartEyePosition + moveDir * 0.2;
			}
			else
			{
				D3DXVECTOR3 _movevec;
				// fTimeDelta에 50 곱하면 느림..
				D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta*60);

				m_vEye = _movevec;
			}
			
			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;

			//다시 플레이어 향하게
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bShake = false;
			m_bFix = false; //잠금 풀어주기
		}

	}
}

// rotation으로 움직임
void CDynamicCamera::ShakeByRotation(const _float& fTimeDelta)
{
	if (m_eCurState == C_SHAKING_ROT)
	{
		//잠깐 맞았을때 살짝 흔들리는거

		if (m_fShakeTime > 0.0f)
		{
			//m_vEye = m_vStartPosition;
			srand((unsigned)time(NULL));

			//랜덤각도 움직이고 싶을때
	
			float randx = (float)(rand() % 30);
			//float randy = (float)(rand() % 30);
			float randz = (float)(rand() % 30);

			// 같은 회전으로 움직일때 사용
			float randy = 5;
			
			if (m_iShakeNum % 2 == 1)
			{

				randy *= -1;

			}			
			D3DXQUATERNION qRot;

			_matrix		matRotX, matRotY, matRotZ, matTotalRot;
			D3DXQuaternionRotationMatrix(&qRot, &matRotX);

			// yaw, pitch, roll 순서
			// yaw : y축, pitch : x축, roll : z축
			D3DXQuaternionRotationYawPitchRoll(&qRot, 0, D3DXToRadian(randx), 0);
			D3DXMatrixRotationQuaternion(&matRotX, &qRot);

			D3DXQuaternionRotationYawPitchRoll(&qRot, D3DXToRadian(randy), 0, 0);
			D3DXMatrixRotationQuaternion(&matRotY, &qRot);

			D3DXQuaternionRotationYawPitchRoll(&qRot, 0, 0, D3DXToRadian(randz));
			D3DXMatrixRotationQuaternion(&matRotZ, &qRot);

			//matTotalRot = matRotX * matRotY * matRotZ;
			matTotalRot = matRotY;

			D3DXVec3TransformCoord(&m_vEye, &m_vEye, &matTotalRot);

			m_iShakeNum++;
			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			m_vEye = m_vStartEyePosition;
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bShake = false;
			m_bFix = false; //잠금 풀어주기
		}
	}
}


// 지정된 target에 부드럽게 이동
void CDynamicCamera::MoveToTarget(const _float& fTimeDelta)
{
	if (m_eCurState == C_MOVE_TO_TARGET)
	{
		if (m_fMoveTime > 0.0f)
		{
			D3DXVECTOR3 _movevec;
			D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * m_fMoveSpeed);

			m_vEye = _movevec;
		}
		else
		{
			m_bMove = false;
			m_bFix = false; //잠금 풀어주기
			m_eCurState = C_PLAYERCHASE;

			// goalposition에 고정안하고 원래 position으로 돌아가는 경우
			if (m_bFixedPos == false)
			{
				m_vEye = m_vStartEyePosition;
			}
		}

		m_fMoveTime -= fTimeDelta;
	}
}


void CDynamicCamera::OnShakeCameraPos(float shakeTime, float shakeIntensity)
{
	// 카메라 흔들림 설정
	// public이어서 object 받아서 사용할까 생각중
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_iShakeNum = 0;

	m_eCurState = C_SHAKING_POS;

	m_vStartEyePosition = m_vEye;

	m_bShake = true;
	m_bFix = true; // 사용자 움직임 잠금 

	m_vGoalPosition = m_vEye;
}

void CDynamicCamera::OnShakeCameraRot(float shakeTime, float shakeIntensity)
{
	// 카메라 흔들림 설정
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_iShakeNum = 0;

	m_eCurState = C_SHAKING_ROT;

	m_vStartEyePosition = m_vEye;
	m_vStartAtPosition = m_vAt;

	// 쉐이킹 전 초기화
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;

	m_bShake = true;
	m_bFix = true; // 사용자 움직임 잠금 
}

void CDynamicCamera::OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition)
{
	m_eCurState = C_MOVE_TO_TARGET;

	m_bFix = true; // 사용자 카메라 움직임 잠금 

	m_fMoveTime = moveTime;
	m_fMoveSpeed = moveSpeed;
	m_vGoalPosition = target;

	m_bFixedPos = fixedPosition;

	if (m_bFixedPos == false)
	{
		// 다시 돌아가야할 경우 첫 시작점 저장
		m_vStartEyePosition = m_vEye;
	}
}

void CDynamicCamera::OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition)
{
	m_vAt = atPos;

	m_eCurState = C_MOVE_TO_TARGET;

	m_bFix = true; // 사용자 카메라 움직임 잠금 

	m_fMoveTime = moveTime;
	m_fMoveSpeed = moveSpeed;
	m_vGoalPosition = target;

	m_bFixedPos = fixedPosition;

	if (m_bFixedPos == false)
	{
		// 다시 돌아가야할 경우 첫 시작점 저장
		m_vStartEyePosition = m_vEye;
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
