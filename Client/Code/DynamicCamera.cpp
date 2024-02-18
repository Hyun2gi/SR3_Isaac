#include "stdafx.h"
#include "..\Header\DynamicCamera.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Player.h"

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
	m_ePreState = C_END;
	m_eCurState = C_PLAYERCHASE;

	m_bShakeCamera = false;
	m_bShakeCamera_Sub = false;
	m_bCollisionWall = false;
	m_bPreCollisionWall = false;
	m_bCollisionWall = false;
	m_bFirstPerson = false;
	m_bPreFirstPerson = false;
	m_bChaseInit = true;
	m_bMouseCameraStart = false;
	m_bResetChaseInit = false;


	m_bStopShakingWorld = false;
	m_bStart = false;

	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;

	m_bEpic = false;
	m_bEpicCall = false;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	return S_OK;
}

Engine::_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pTarget == nullptr)
	{
		m_pTarget = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));
		CPlayer::GetInstance()->Set_Camera(this);
	}

	ShakeByPosition(fTimeDelta);
	//ShakeByRotation(fTimeDelta);
	ShakeByPosition_Sub(fTimeDelta);

	// 해당위치로 이동
	MoveToTarget(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);


	if (m_eCurState == C_DOPLE)
	{
		m_vEye = _vec3(VTXCNTX / 2, 29, VTXCNTZ / 2-1);

		m_vAt = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);

		return iExit;
	}

	Key_Input(fTimeDelta);

	if (m_eCurState == C_EPIC)
	{
		// Epic 일때 위치 세팅
		CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

		_vec3		playerPos;

		playerInfo->Get_Info(INFO_POS, &playerPos);
		m_vEye = _vec3(playerPos.x, 20, playerPos.z);
	}

	


	if (false == m_bFix)
	{
		if (m_bMouseCameraStart)
		{
			if (Get_ShakingCamera() == false)
			{
				Mouse_Move();
			}
			
		}
			
		Chase_Character(fTimeDelta);
		//마우스 움직임

		if (m_eCurState != C_EPIC)
		{
			// EPIC 일때는 마우스 움직일 수 있게하기
			// EPIC 아닐때는 마우스 움직임 중간으로
			Mouse_Fix();
		}

	}

	if (true == m_bFix && (m_bShakeCamera_Sub == true || m_bShakeCamera_Rot == true))
	{
		Chase_Character(fTimeDelta);
	}


	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject()
{
	Engine::CCamera::LateUpdate_GameObject();
}


void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	if (Engine::Get_DIKeyState(DIK_N) & 0x80)
	{
		if (m_bShakeCamera_Sub == false)
		{
			//OnShakeCameraPos(float shakeTime, float shakeIntensity)
			//OnShakeCameraPos(1.0, 1);
			//OnShakeCameraPos_Sub(1.0, 0.7);
			if (m_bShakeCamera_Rot == false)
			{
				OnShakeCameraRot(1.0, 0.1);
			}
			
		}
	}

	if (Engine::Get_DIKeyState(DIK_Y) & 0x80)
	{
		if (m_bShakeCamera == false)
		{
			Cinemachine_01_TotalLand();
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

void CDynamicCamera::Chase_Character(const _float& fTimeDelta)
{

	if (m_eCurState == C_PLAYERCHASE)
	{
		CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

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
			
			m_bFirstPerson = false;

			m_fTotalDistanceWithPlayer = D3DXVec3Length(&(m_vEye - m_vAt));
			m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;

			if (m_bMouseCameraStart == false)
			{
				// 시네머신 끝나고나 m_bMouseCameraStart처리가 안되었을경우
				m_bMouseCameraStart = true;
			}

			//*** 이미 방문한 방에 방문할 경우를 위해 처리***
			// 방문한 경우 큐브연출x, 바로 들어갔던 방문 앞에서 스폰되도록(1인칭 되게끔)
			// 벽에 부딪혔을때
			if ((m_vEye.x > VTXCNTX - 3 || m_vEye.z > VTXCNTX - 3 || m_vEye.x < 3 || m_vEye.z < 3))
			{
				// 밖으로 나갔을때
				m_bCollisionWall = true;
				m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;
			}

			if (m_bCollisionWall == true)
			{
				// 방 이동때 바로 튀어나오게끔
				if (!CPlayer::GetInstance()->Get_SafeCamera_Area_For_ChangeStage())
				{
					m_bFirstPerson = true;
					m_vGoalPosition = playerPos + _vec3(0, 2, 0);
					// 1인칭 시점으로 전환
					D3DXVECTOR3 _movevec;
					//m_vCameraPosDir = -(playerDir);
					m_vGoalPosition = playerPos + _vec3(0, 2, 0);
					D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
					m_vEye = m_vGoalPosition;
					m_vAt = playerPos + playerDir * 4 + _vec3(0, 1, 0);
					//m_vCameraPosDir = m_vEye - m_vAt;
				}

			}

		}
		else
		{
			if (m_bShakeCamera_Rot)
			{
				// 플레이어 카메라 쉐이킹
				ShakeByRotation(fTimeDelta);
			}


			// 회전한만큼 길이와 방향이 벡터에 저장되어 있어서
			// 플레이어에서 해당방향만큼 계산
			_vec3 vPos = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;

			m_bPreCollisionWall = m_bCollisionWall;
			m_bPreFirstPerson = m_bFirstPerson;

			if (m_bPreFirstPerson != m_bFirstPerson)
			{
				m_fAngleY = 0;
			}

			// 벽에 부딪혔을때
			if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
			{
				// 밖으로 나갔을때
				m_bCollisionWall = true;
				m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;
			}
			else
			{
				m_bCollisionWall = false;
				m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;
			}

			if (m_bPreCollisionWall != m_bCollisionWall)
			{
				// 각도 초기화를 위한 비교
				m_fAngleY = 0;
			}


			if (m_bCollisionWall == true)
			{
				if (CPlayer::GetInstance()->Get_SafeCamera_Area())
				{
					// 안전지대에 있을때는 그냥 카메라가 따라오게끔
					// 카메라와 벽 충돌발생
					int cnt = 0;
					while (CheckCollisionWall(m_fFlexibleDistanceWithPlayer))
					{
						m_fFlexibleDistanceWithPlayer -= 0.01;
						cnt++;

						if (cnt > 5000)
						{
							break;
						}
					}
					m_vGoalPosition = m_vAt + m_vCameraPosDir * m_fFlexibleDistanceWithPlayer;

					/*D3DXVECTOR3 _movevec;
					D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);*/

					m_vEye = m_vAt + m_vCameraPosDir * m_fFlexibleDistanceWithPlayer;

					if (D3DXVec3Length(&(m_vEye - m_vAt)) < 5)
					{
						// 1인칭처럼 바꾸기 (총알 방향 조절을 위해 해당 변수 바꾸기)
						m_bFirstPerson = true;
					}
					else
					{
						m_bFirstPerson = false;
					}
				}
				else
				{
					m_bFirstPerson = true;
					m_vGoalPosition = playerPos + _vec3(0, 2, 0);
					// 1인칭 시점으로 전환
					D3DXVECTOR3 _movevec;
					//m_vCameraPosDir = -(playerDir);
					m_vGoalPosition = playerPos + _vec3(0, 2, 0);
					D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
					m_vEye = m_vGoalPosition;
					m_vAt = playerPos + playerDir * 4 + _vec3(0, 1, 0);
					//m_vCameraPosDir = m_vEye - m_vAt;
				}
				
			}
			else if(m_bCollisionWall == false)
			{
				// 카메라와 벽 충돌x
				m_vEye = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;
				m_bFirstPerson = false;
			}

#pragma region 안쓰는거
			// 벽에 부딪히고 safe_area에 있으면
			//if (m_bCollisionWall == true && CPlayer::GetInstance()->Get_SafeCamera_Area())
			//{
			//	// 플레이어가 안전지대에 들어옴
			//	// 다시 멀어지게끔
			//	D3DXVECTOR3 _movevec;
			//	m_vCameraPosDir = -(playerDir);
			//	m_vGoalPosition = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;

			//	if ((m_vGoalPosition.x <= VTXCNTX - 1 && m_vGoalPosition.z <= VTXCNTX - 1 && m_vGoalPosition.x >= 1 && m_vGoalPosition.z >= 1))
			//	{
			//		D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
			//		m_vEye = _movevec;
			//		//m_vCameraPosDir = m_vEye - m_vAt;
			//		//1인칭 시점 아님
			//		m_bFirstPerson = false;

			//		_vec3 distance = m_vEye - m_vGoalPosition;

			//		// 내가 원하는 카메라 위치와 지금 위치가 차이가 별로 안나면
			//		if (D3DXVec3Length(&distance) < 30)
			//		{
			//			m_bCollisionWall = false;
			//			m_vEye = m_vGoalPosition;
			//			m_fAngleY = 0;
			//			m_vAt = playerPos + playerDir * 2;
			//			//m_vCameraPosDir = m_vEye - m_vAt;
			//		}
			//	}
			//}
			//else if (m_bCollisionWall == true && !CPlayer::GetInstance()->Get_SafeCamera_Area() && m_bFirstPerson == false)
			//{
			//	// 밖으로 나간 상태
			//	// 카메라와 가까워지게
			//	D3DXVECTOR3 _movevec;
			//	m_vCameraPosDir = -(playerDir);
			//	m_vGoalPosition = playerPos + _vec3(0, 3, 0);
			//	D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 5);
			//	m_vEye = _movevec;
			//	m_vAt = playerPos + playerDir * 2;
			//	m_bFirstPerson = false;
			//	_vec3 distance = m_vEye - m_vAt;

			//	if (D3DXVec3Length(&distance) < 4)
			//	{
			//		m_bFirstPerson = true;
			//		m_fAngleY = 0;
			//		m_vEye = playerPos + _vec3(0, 2, 0);
			//	}
			//	m_vCameraPosDir = m_vEye - m_vAt;
			//}
			//else if (m_bCollisionWall == true && !CPlayer::GetInstance()->Get_SafeCamera_Area() && m_bFirstPerson == true)
			//{
			//	// 1인칭 시점으로 전환
			//	D3DXVECTOR3 _movevec;
			//	m_vCameraPosDir = -(playerDir);
			//	m_vGoalPosition = playerPos + _vec3(0, 2, 0);
			//	D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
			//	m_vEye = m_vGoalPosition;
			//	m_vAt = playerPos + playerDir * 4 + _vec3(0, 1, 0);
			//	m_vCameraPosDir = m_vEye - m_vAt;
			//}
			//else if (m_bCollisionWall == false)
			//{
			//	// 안정권
			//	m_vEye = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;
			//	m_bFirstPerson = false;
			//}
#pragma endregion
		}

		if (m_bShakeCamera_Sub)
		{
			m_vEye += m_vShakeBoundary;
		}

		
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

		// 회전 막기
		if (m_fAngleY - (dwMouseMoveY / 10.f) > -20 && m_fAngleY - (dwMouseMoveY / 10.f) < 38)
		{
			m_fAngleY -= (dwMouseMoveY / 10.f);
			D3DXQuaternionRotationAxis(&qRot, &vCross, -D3DXToRadian(dwMouseMoveY / 10.f));
			D3DXMatrixRotationQuaternion(&matRotY, &qRot);

			matTotalRot = matRotX * matRotY;

			CPlayer::GetInstance()->Set_Bool_MouseYRotation(true);
			CPlayer::GetInstance()->Set_MouseRotation(D3DXToRadian(dwMouseMoveX / 10.f), -D3DXToRadian(dwMouseMoveY / 10.f));
		}
		else
		{
			CPlayer::GetInstance()->Set_Bool_MouseYRotation(false);
			matTotalRot = matRotX;
			CPlayer::GetInstance()->Set_MouseRotation(D3DXToRadian(dwMouseMoveX / 10.f), 0);
		}

		D3DXVec3TransformCoord(&vLook, &vLook, &matTotalRot);
		m_vCameraPosDir = vLook;
		D3DXVec3Normalize(&m_vCameraPosDir, &m_vCameraPosDir);
	}
}

// 전체 맵 쉐이킹
void CDynamicCamera::ShakeByPosition(const _float& fTimeDelta)
{
	if (m_bShakeCamera == true)
	{
		//잠깐 맞았을때 살짝 흔들리는거

		if (m_fShakeTime > 0.0f)
		{
			// 쉐이킹 멈추기
			if (m_bStopShakingWorld == false)
			{
				if (m_vGoalPosition == m_vEye)
				{
					float FLOAT_MAX = 1;
					float FLOAT_MIN = 0;

					_matrix			matCamWorld;
					D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);
					_vec3 camright, camup;
					camright = _vec3(matCamWorld._11, matCamWorld._12, matCamWorld._13);
					camup = _vec3(matCamWorld._21, matCamWorld._22, matCamWorld._23);



					if (m_bChangeShakeDir == false)
					{
						//moveDir *= -1;
						camright *= -1;
						m_bChangeShakeDir = true;
					}
					else
					{
						m_bChangeShakeDir = false;
					}

					camright *= m_fShakeIntensity;

					// 목표위치
					// moveDir과 곱해주는 값은 작아야함!!
					//m_vGoalPosition = m_vStartEyePosition + moveDir*0.2;
					// m_vGoalPosition = m_vStartEyePosition + moveDir * 0.2;


					m_vGoalPosition = m_vStartEyePosition + camright * 0.3;
				}
				else
				{
					//D3DXVECTOR3 _movevec;
					//// fTimeDelta에 50 곱하면 느림..
					//D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, 1);

					//m_vEye = _movevec;
					//m_vAt = m_vEye + m_vWorldAtDir;
					////m_vAt = _movevec;
				}
			}
			

			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			// 큐브연출 후
			//if (m_eAfterState == C_MOVE_TO_TARGET)
			//{
			//	//원상복구
			//	m_fShakeTime = 0;
			//	m_eAfterState = C_END;
			//	m_bFix = false; //잠금 풀어주기
			//	m_eCurState = C_MOVE_TO_TARGET;
			//	m_bChaseInit = true;
			//	m_bShakeCamera = false;
			//	Cinemachine_02_GoToIsaac();
			//	return;
			//}

			m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;
			m_bStopShakingWorld = false;
			//다시 플레이어 향하게
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bShakeCamera = false;
			m_bFix = false; //잠금 풀어주기

			// 플레이어 잠금 풀기
			CPlayer::GetInstance()->Set_KeyBlock(false);
		}

	}
}

void CDynamicCamera::ShakeByPosition_Sub(const _float& fTimeDelta)
{
	if (m_bShakeCamera_Sub == true)
	{
		//잠깐 맞았을때 살짝 흔들리는거

		if (m_fShakeTime > 0.0f)
		{
			
			if (D3DXVec3Length(&(m_vShakeBoundary - m_vGoalPosition)) <0.1)
			{
				/*DWORD dwSeed = time(NULL) % 1000;
				srand(dwSeed);
				float fMax = 32767;
				float fX = rand() / fMax;
				float fY = rand() / fMax;
				float fZ = rand() / fMax;
				float fZ = 0;
				float fY = 0;
				m_vGoalPosition = _vec3(fX, fY, fZ);
				D3DXVec3Normalize(&m_vGoalPosition, &m_vGoalPosition);*/

				//if (m_iShakeNum % 2 == 0)
				//{
				//	//moveDir *= -1;
				//	m_vGoalPosition *= -1;
				//}
				//m_iShakeNum++;

				//m_vGoalPosition = m_vGoalPosition * m_fShakeIntensity;

				/*CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

				_vec3		playerPos;
				_vec3		playerRightDir, playerUpDir;
				_vec3		cameraDir;
				_vec3		cameraPos;

				playerInfo->Get_Info(INFO_POS, &playerPos);
				playerInfo->Get_Info(INFO_RIGHT, &playerRightDir);
				playerInfo->Get_Info(INFO_UP, &playerUpDir);
				D3DXVec3Normalize(&playerRightDir, &playerRightDir);*/

				_matrix			matCamWorld;
				D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);
				_vec3 camright, camup;
				camright = _vec3(matCamWorld._11, matCamWorld._12, matCamWorld._13);
				camup = _vec3(matCamWorld._21, matCamWorld._22, matCamWorld._23);


				DWORD dwSeed = time(NULL) % 1000;
				std::srand(dwSeed);
				float fMax = 32767;
				float fY = rand() / fMax;
				m_vGoalPosition = camright;
				
				//m_vGoalPosition =  _vec3(0,fY,0)*10;

				if (m_bChangeShakeDir == false)
				{
					//moveDir *= -1;
					m_vGoalPosition *= -1;
					m_bChangeShakeDir = true;
				}
				else
				{
					m_bChangeShakeDir = false;
				}
				//D3DXVec3Normalize(&m_vGoalPosition, &m_vGoalPosition);
				

				m_vGoalPosition = m_vGoalPosition * m_fShakeIntensity;

			}
			else
			{
				D3DXVECTOR3 _movevec;
				// fTimeDelta에 50 곱하면 느림..
				D3DXVec3Lerp(&m_vShakeBoundary, &m_vShakeBoundary, &m_vGoalPosition, 0.9);
			}
			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			//m_vStartEyePosition = m_vCameraPosDir; -> 이렇게 보관했음
			m_vCameraPosDir = m_vStartEyePosition;
			m_fAngleY = 0;
			//m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;
			m_bShakeCamera_Sub = false;
			//다시 플레이어 향하게
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bFix = false; //잠금 풀어주기

			// 플레이어 잠금 풀기
			CPlayer::GetInstance()->Set_KeyBlock(false);
			
		}

	}
}

// rotation으로 움직임
void CDynamicCamera::ShakeByRotation(const _float& fTimeDelta)
{
	if (m_bShakeCamera_Rot)
	{
		//잠깐 맞았을때 살짝 흔들리는거

		if (m_fShakeTime > 0.0f)
		{
			//m_vEye = m_vStartPosition;
			std::srand((unsigned)time(NULL));

			_matrix			matCamWorld;
			D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);
			_vec3 camright, camup;
			camright = _vec3(matCamWorld._11, matCamWorld._12, matCamWorld._13);
			camup = _vec3(matCamWorld._21, matCamWorld._22, matCamWorld._23);

			if (abs(m_fTotalShakingAngle) >= 2)
			{
				if (m_bChangeShakeDir)
				{
					//음수
					m_bChangeShakeDir = false;
					m_fTotalShakingAngle++;
				}
				else
				{
					//음수
					m_bChangeShakeDir = true;
					m_fTotalShakingAngle--;
				}
			}
		
			// m_bChangeShakeDir = true; 초기화값
			if (m_bChangeShakeDir)
			{
				// true 면 음수
				m_fTotalShakingAngle--;
				m_fAngleY = -1;
			}
			else
			{
				// false면 더하기
				m_fTotalShakingAngle++;
				m_fAngleY = 1;
			}

			D3DXQUATERNION qRot;

			_matrix		matRotX, matRotY, matRotZ, matTotalRot;


			D3DXQuaternionRotationYawPitchRoll(&qRot, D3DXToRadian(m_fAngleY), 0, 0);
			D3DXMatrixRotationQuaternion(&matRotY, &qRot);

			
			matTotalRot = matRotY;

			D3DXVec3TransformCoord(&m_vCameraPosDir, &m_vCameraPosDir, &matTotalRot);

			//m_iShakeNum++;
			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			m_fTotalShakingAngle = 0;
			m_fAngleX = 0;
			m_fAngleY = 0;
			m_bShakeCamera_Rot = false;
			// 원상태로
			m_vCameraPosDir = m_vStartEyePosition;
			//m_vEye = m_vStartEyePosition;
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bFix = false; //잠금 풀어주기
			CPlayer::GetInstance()->Set_KeyBlock(false);
		}
	}
}


// 지정된 target에 부드럽게 이동
void CDynamicCamera::MoveToTarget(const _float& fTimeDelta)
{
	if (m_eCurState == C_MOVE_TO_TARGET)
	{
		CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

		_vec3		playerPos;
		_vec3		playerDir;
		_vec3		targetpos;

		playerInfo->Get_Info(INFO_POS, &playerPos);
		playerInfo->Get_Info(INFO_LOOK, &playerDir);


		if (m_fMoveTime > 0.0f)
		{
			if (m_vEye != m_vGoalPosition)
			{
				D3DXVECTOR3 _movevec;
				D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * m_fMoveSpeed);
				m_vEye = _movevec;
			}
			
			if (m_eAfterState == C_CINEMACHINE_02)
			{
				m_vAt = _vec3(VTXCNTX / 2, 10, VTXCNTZ / 2);
				// 시네머신하면 고정돼야한다
				//m_vAt = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);
			}
			else if(m_bFixAtPos == false)
			{
				m_vAt = playerPos + playerDir * 2;
			}
			
		}
		else
		{
			m_bFixAtPos = false;
			//m_fAngleY = 0;

			if (m_bStart)
			{
				m_bStart = false;
				//Engine::StopSound(SOUND_BGM);
			}


			// 에픽에투스일때 위로 고정
			if (m_bEpic == true)
			{
				//에픽에투스 상태임
				m_eCurState = C_EPIC;
				// 키보드 사용할 수 있게끔
				CPlayer::GetInstance()->Set_KeyBlock(false);
				// 타겟 설정할때는 카메라 마우스 이동 막기
				m_bFix = true;
				return;
			}

			if (m_bEpicCall == true)
			{
				// 다시 제자리로 돌아간 후 떨어지기 시작
				CPlayer::GetInstance()->Set_EpicFall();
				m_bEpicCall = false;
				m_eCurState = C_PLAYERCHASE;
				m_bChaseInit = false;
				m_bFix = false;
				m_vEye = m_vGoalPosition;
				m_vCameraPosDir = m_vEye - m_vAt;
				D3DXVec3Normalize(&m_vCameraPosDir, &m_vCameraPosDir);
				CPlayer::GetInstance()->Set_KeyBlock(false);
				return;
			}

			if (m_eAfterState == C_DOPLE)
			{
				m_eCurState = C_DOPLE;
				// 마우스 움직임 막기
				m_bFix = true;
				//플레이어 잠금풀기
				CPlayer::GetInstance()->Set_KeyBlock(false);
			}


			m_bFix = false; //잠금 풀어주기

			// m_bFixedPos == true 그자리에 고정
			if (m_eAfterState == C_PLAYERCHASE && m_bFixedPos == true)
			{
				// 도착지점에서 바뀌지 않을경우에는 bChaseInit = false를 해줘야 새로운 자리를 잡지 않음
				m_eCurState = C_PLAYERCHASE;
				m_bChaseInit = false;
				m_bFix = false;
				D3DXVec3Normalize(&m_vCameraPosDir, &(m_vEye- m_vAt));
			}
			else if (m_eAfterState == C_MOVE_TO_TARGET)
			{
				m_eCurState = C_MOVE_TO_TARGET;
				m_bFix = true;
			}
			
			if (m_bResetChaseInit == true)
			{
				// chaseinit 해주기
				m_bChaseInit = true;
				m_bResetChaseInit = false;
			}
			


			// goalposition에 고정안하고 원래 position으로 돌아가는 경우
			if (m_bFixedPos == false)
			{
				m_vEye = m_vStartEyePosition;
			}
			CPlayer::GetInstance()->Set_KeyBlock(false);
		}

		m_fMoveTime -= fTimeDelta;
	}
}

bool CDynamicCamera::CheckCollisionWall(float distance)
{
	D3DXVec3Normalize(&m_vCameraPosDir, &m_vCameraPosDir);
	_vec3 vPos = m_vAt + m_vCameraPosDir * distance;

	// 벽에 부딪혔을때
	// if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
	if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
	{
		// 밖으로 나갔을때
		return true;
	}
	else
	{
		return false;
	}
}


void CDynamicCamera::OnShakeCameraPos(float shakeTime, float shakeIntensity)
{
	// 카메라 흔들림 설정
	// public이어서 object 받아서 사용할까 생각중
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_bChangeShakeDir = false;

	//m_eCurState = C_SHAKING_POS;

	m_bShakeCamera = true;
	m_bFix = true; // 사용자 움직임 잠금 

	m_vStartEyePosition = m_vEye;
	m_vGoalPosition = m_vEye;

	// 움직이지 못하게
	CPlayer::GetInstance()->Set_KeyBlock(true);
}

void CDynamicCamera::OnShakeCameraPos_Sub(float shakeTime, float shakeIntensity)
{
	
	m_vGoalPosition = _vec3(0, 0, 0);
	m_vShakeBoundary = m_vGoalPosition;
	// 카메라 흔들림 설정
	// public이어서 object 받아서 사용할까 생각중
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_bChangeShakeDir = false;

	m_bShakeCamera_Sub = true;
	m_bFix = true; // 사용자 움직임 잠금 (카메라쪽)

	// cameraposdir 원본 보관
	m_vStartEyePosition = m_vCameraPosDir;
	//m_vGoalPosition = m_vEye;

	// 움직이지 못하게
	//CPlayer::GetInstance()->Set_KeyBlock(true);
}

void CDynamicCamera::OnShakeCameraRot(float shakeTime, float shakeIntensity)
{
	// 카메라 흔들림 설정
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	//m_iShakeNum = 0;
	
	//m_eCurState = C_SHAKING_ROT;

	/*m_vStartEyePosition = m_vEye;
	m_vStartAtPosition = m_vAt;*/

	m_vStartEyePosition = m_vCameraPosDir;
	
	// 쉐이킹 전 초기화
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;
	m_fTotalShakingAngle = 0;

	m_bChangeShakeDir = true; // 방향 바꿀떄 사용
	m_bShakeCamera_Rot = true;
	m_bFix = true; // 사용자 움직임 잠금 
}

void CDynamicCamera::OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate)
{
	CPlayer::GetInstance()->Set_KeyBlock(true);

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

	switch (afterstate)
	{
	case 0:
		m_eAfterState = C_PLAYERCHASE;
		break;
	case 1:
		m_eAfterState = C_WHOLELAND;
		break;
	case 2:
		m_eAfterState = C_SHAKING_POS;
		break;
	case 3:
		m_eAfterState = C_SHAKING_ROT;
		break;
	case 4:
		m_eAfterState = C_MOVE_TO_TARGET;
		break;
	case 5:
		m_eAfterState = C_CINEMACHINE_02;
		break;
	case 6:
		m_eAfterState = C_DOPLE;
		break;
	}
}


void CDynamicCamera::OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate)
{
	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vOriginAtPosition = m_vAt;
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

	switch (afterstate)
	{
	case 0:
		m_eAfterState = C_PLAYERCHASE;
		break;
	case 1:
		m_eAfterState = C_WHOLELAND;
		break;
	case 2:
		m_eAfterState = C_SHAKING_POS;
		break;
	case 3:
		m_eAfterState = C_SHAKING_ROT;
		break;
	case 4:
		m_eAfterState = C_MOVE_TO_TARGET;
		break;
	case 5:
		m_eAfterState = C_CINEMACHINE_02;
		break;
	case 6:
		m_eAfterState = C_DOPLE;
		break;
	}
}

void CDynamicCamera::OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, _vec3 startpos, bool fixedPosition, int afterstate)
{
	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vOriginAtPosition = m_vAt;
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

	m_vEye = startpos;


	switch (afterstate)
	{
	case 0:
		m_eAfterState = C_PLAYERCHASE;
		break;
	case 1:
		m_eAfterState = C_WHOLELAND;
		break;
	case 2:
		m_eAfterState = C_SHAKING_POS;
		break;
	case 3:
		m_eAfterState = C_SHAKING_ROT;
		break;
	case 4:
		m_eAfterState = C_MOVE_TO_TARGET;
		break;
	case 5:
		m_eAfterState = C_CINEMACHINE_02;
		break;
	case 6:
		m_eAfterState = C_DOPLE;
		break;
	}
}

void CDynamicCamera::OnMoveToPlayerFront()
{
	CPlayer::GetInstance()->Set_KeyBlock(true);
	m_eCurState = C_MOVE_TO_TARGET;
	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		targetpos;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	m_vOriginAtPosition = m_vAt;
	m_vStartEyePosition = m_vEye;

	m_bFix = true;
	D3DXVec3Normalize(&playerDir, &playerDir);
	playerDir *= -4;
	// 바라보는 대상은 플레이어
	targetpos = playerPos + playerDir;

	OnMoveTargetCamera(5, 5, targetpos, true, 4);
}


void CDynamicCamera::OnMoveToOriginPos()
{
	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		cameraDir;
	_vec3		cameraPos;


	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);

	// 바라보는 대상은 플레이어
	m_vAt = m_vOriginAtPosition;

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vCameraPosDir = -(playerDir);

	m_eAfterState = C_PLAYERCHASE;
	_vec3	moveCamPos;

	/*if (m_bCollisionWall == false)
	{
		moveCamPos = playerPos + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
	}
	else
	{
		moveCamPos = playerPos + m_vCameraPosDir * m_fCameraShortDistance + _vec3(0, m_fCameraShortHeight, 0);
	}*/

	m_eCurState = C_MOVE_TO_TARGET;
	//m_vCameraPosDir = m_vCameraPosDir * m_fCameraShortDistance + _vec3(0, m_fCameraShortHeight, 0);

	OnMoveTargetCamera(1.5f, 7.f, m_vStartEyePosition, true, 0);
}

void CDynamicCamera::Set_EpicBullet()
{
	m_bEpic = true;
	m_vStartAtPosition = m_vAt;
	m_vStartEyePosition = m_vEye;
	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	m_fAngleY = 0;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	_vec3 target = _vec3(playerPos.x, 20, playerPos.z);
	OnMoveTargetCamera(1.f, 7.f, target, false, 0);
}

void CDynamicCamera::Set_Shoot_End_Epic()
{
	// 에픽에투스 완료
	m_bEpic = false;
	// 에픽에투스 떨어지는거 부르기
	m_bEpicCall = true;
	m_eAfterState = C_PLAYERCHASE;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		cameraDir;
	_vec3		cameraPos;


	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);

	// 바라보는 대상은 플레이어
	m_vAt = playerPos + playerDir * 2;

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vCameraPosDir = -(playerDir);

	_vec3	moveCamPos;

	
	moveCamPos = playerPos + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
	

	//OnMoveTargetCamera(1.f, 7.f, moveCamPos, false, 0);
	// 1인칭일때를 위해 원래 자리로 돌아가게끔
	OnMoveTargetCamera(1.f, 7.f, m_vStartEyePosition, false, 0);
}

void CDynamicCamera::Cinemachine_00_Start()
{
	m_eAfterState = C_PLAYERCHASE;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;

	// 사운드위해
	m_bStart = true;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		cameraDir;
	_vec3		cameraPos;


	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);

	// 현재 플레이어 위치 받아오는게 제대로 안돼서 플레이어 스폰 지역인 중간 지점에서 작동되게끔함
	m_vAt = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) + (playerDir) * 2;

	m_vCameraPosDir = -(playerDir);
	
	// false로 하면 m_vEye 상태를 m_vStartpos로 저장해 돌아가게끔 하기 때문에 3인칭 되는 지점을 m_vEye로 초기화
	m_vEye = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos,startPos;


	goalPos = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) - (playerDir) * 3;
	startPos = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) - (playerDir) * 3 + _vec3(6, 6, -3);

	m_bChaseInit = true;
	// false하면 chaseInit이 true
	OnMoveTargetCamera(m_vAt, 30.f, 0.5f, goalPos, startPos, false, 0);
}

void CDynamicCamera::Cinemachine_01_TotalLand()
{
	m_eAfterState = C_MOVE_TO_TARGET;
	m_bChaseInit = true;
	//m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;
	m_bShakeCamera = true;

	m_bFix = true;

	// 플레이어 키 막기
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	goalPos = _vec3(-5, 25, -5);

	startpos = m_vEye;

	m_vEye = _vec3(-5, 25, -5);
	m_vAt = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);
	// 5 : CINEMACHINE_01
	//OnMoveTargetCamera(_vec3(VTXCNTX / 2, 0, VTXCNTZ / 2), 8.f, 1.f, goalPos, startpos, true, 5);

	m_bStopShakingWorld = false;

	m_vWorldAtDir = m_vAt - m_vEye;
	OnShakeCameraPos(19, 1);
}

void CDynamicCamera::Cinemachine_02_MiddleTotalLand()
{
	m_bShakeCamera = false;
	m_eAfterState = C_CINEMACHINE_02;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;

	m_bMouseCameraStart = false;
	m_bFix = true;

	// 플레이어 키 막기
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// 바라보는 대상은 플레이어
	

	goalPos = _vec3(3, 10, 3);
	m_vAt = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2);

	startpos = _vec3(-5, 25, -5);

	// 5 : cinemachine2
	OnMoveTargetCamera(m_vAt, 10.f, 7.f, goalPos, false, 5);
}

void CDynamicCamera::Cinemachine_03_GoToIsaac()
{
	m_bShakeCamera = false;
	m_eAfterState = C_PLAYERCHASE;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;
	m_bResetChaseInit = true;

	m_bMouseCameraStart = false;
	m_bFix = true;

	// 플레이어 키 막기
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// 바라보는 대상은 플레이어
	m_vAt = playerPos + playerDir * 2;

	goalPos = m_vAt + -(playerDir)* m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);

	startpos = m_vEye;

	//m_vEye = goalPos;

	// 0 : playerchase
	OnMoveTargetCamera(m_vAt, 2.f, 3.f, goalPos, startpos, true, 0);
}

void CDynamicCamera::Cinemachine_04_Dople_GoToIsaac()
{
	m_bShakeCamera = false;
	m_eAfterState = C_DOPLE;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;
	m_bResetChaseInit = true;

	m_bMouseCameraStart = false;
	m_bFix = true;

	// 플레이어 키 막기
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	

	m_bFixAtPos = true;
	
	goalPos = _vec3(VTXCNTX / 2, 29, VTXCNTZ / 2 - 1);

	m_vAt = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);

	// case 6: dople
	OnMoveTargetCamera(m_vAt, 2.f, 3.f, goalPos, m_vEye, true, 6);
}


void CDynamicCamera::Set_OnSlotCamera(CTransform* slotTransform)
{
	m_eAfterState = C_PLAYERCHASE;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;
	m_bFix = true;

	// 임의로 각도 조정
	m_vStartEyePosition = m_vCameraPosDir;

	// atpos 직접 정해준 곳에 나둬야함
	m_bFixAtPos = true;

	_vec3 slotPos, slotDir, camPos, atPos;
	slotTransform->Get_Info(INFO_POS, &slotPos);
	slotTransform->Get_Info(INFO_LOOK, &slotDir);

	camPos = slotPos + (-slotDir * 4.5) + _vec3(0, 0.4f, 0);
	atPos = slotPos + _vec3(0, 0.4f, 0);

	OnMoveTargetCamera(atPos, 11.f, 3.f, camPos, m_vEye, true, 0);
}

void CDynamicCamera::Set_OffSlotCamera()
{
	// 임의로 각도 조정
	m_vCameraPosDir = m_vStartEyePosition;

	// atpos 직접 정해준 고셍 나둬야함
	m_bFixAtPos = true;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// 바라보는 대상은 플레이어
	//m_vAt = playerPos + playerDir * 2;

	//m_vCameraPosDir = -(playerDir);

	_vec3 goalPos;
	m_vAt = playerPos + playerDir * 2;

	//goalPos = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);

	m_fAngleY = 0;
	goalPos = m_vAt + m_vCameraPosDir * m_fFlexibleDistanceWithPlayer;


	OnMoveTargetCamera(m_vAt, 2.f, 5.f, goalPos, m_vEye, true, 0);
}

void CDynamicCamera::Set_OnDople()
{
	m_eCurState = C_DOPLE;
}

void CDynamicCamera::Set_OffDople()
{
	//m_eCurState = C_PLAYERCHASE;
	m_fAngleY = 0;
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
