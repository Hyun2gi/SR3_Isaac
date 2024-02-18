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

	// �ش���ġ�� �̵�
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
		// Epic �϶� ��ġ ����
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
		//���콺 ������

		if (m_eCurState != C_EPIC)
		{
			// EPIC �϶��� ���콺 ������ �� �ְ��ϱ�
			// EPIC �ƴҶ��� ���콺 ������ �߰�����
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
		// �ٶ󺸴� ����� �÷��̾�
		m_vAt = playerPos + playerDir * 2;

		// ���ο� ī�޶� ��ġ
		if (m_bChaseInit == true)
		{
			// ī�޶� ó�� �ʱ�ȭ
			// �÷��̾� �ڷ� ��ġ�ϰԲ� �ϰ� ���� ����
			m_vCameraPosDir = -(playerDir);

			m_vEye = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
			
			m_bFirstPerson = false;

			m_fTotalDistanceWithPlayer = D3DXVec3Length(&(m_vEye - m_vAt));
			m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;

			if (m_bMouseCameraStart == false)
			{
				// �ó׸ӽ� ������ m_bMouseCameraStartó���� �ȵǾ������
				m_bMouseCameraStart = true;
			}

			//*** �̹� �湮�� �濡 �湮�� ��츦 ���� ó��***
			// �湮�� ��� ť�꿬��x, �ٷ� ���� �湮 �տ��� �����ǵ���(1��Ī �ǰԲ�)
			// ���� �ε�������
			if ((m_vEye.x > VTXCNTX - 3 || m_vEye.z > VTXCNTX - 3 || m_vEye.x < 3 || m_vEye.z < 3))
			{
				// ������ ��������
				m_bCollisionWall = true;
				m_fFlexibleDistanceWithPlayer = m_fTotalDistanceWithPlayer;
			}

			if (m_bCollisionWall == true)
			{
				// �� �̵��� �ٷ� Ƣ����Բ�
				if (!CPlayer::GetInstance()->Get_SafeCamera_Area_For_ChangeStage())
				{
					m_bFirstPerson = true;
					m_vGoalPosition = playerPos + _vec3(0, 2, 0);
					// 1��Ī �������� ��ȯ
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
				// �÷��̾� ī�޶� ����ŷ
				ShakeByRotation(fTimeDelta);
			}


			// ȸ���Ѹ�ŭ ���̿� ������ ���Ϳ� ����Ǿ� �־
			// �÷��̾�� �ش���⸸ŭ ���
			_vec3 vPos = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;

			m_bPreCollisionWall = m_bCollisionWall;
			m_bPreFirstPerson = m_bFirstPerson;

			if (m_bPreFirstPerson != m_bFirstPerson)
			{
				m_fAngleY = 0;
			}

			// ���� �ε�������
			if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
			{
				// ������ ��������
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
				// ���� �ʱ�ȭ�� ���� ��
				m_fAngleY = 0;
			}


			if (m_bCollisionWall == true)
			{
				if (CPlayer::GetInstance()->Get_SafeCamera_Area())
				{
					// �������뿡 �������� �׳� ī�޶� ������Բ�
					// ī�޶�� �� �浹�߻�
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
						// 1��Īó�� �ٲٱ� (�Ѿ� ���� ������ ���� �ش� ���� �ٲٱ�)
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
					// 1��Ī �������� ��ȯ
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
				// ī�޶�� �� �浹x
				m_vEye = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;
				m_bFirstPerson = false;
			}

#pragma region �Ⱦ��°�
			// ���� �ε����� safe_area�� ������
			//if (m_bCollisionWall == true && CPlayer::GetInstance()->Get_SafeCamera_Area())
			//{
			//	// �÷��̾ �������뿡 ����
			//	// �ٽ� �־����Բ�
			//	D3DXVECTOR3 _movevec;
			//	m_vCameraPosDir = -(playerDir);
			//	m_vGoalPosition = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;

			//	if ((m_vGoalPosition.x <= VTXCNTX - 1 && m_vGoalPosition.z <= VTXCNTX - 1 && m_vGoalPosition.x >= 1 && m_vGoalPosition.z >= 1))
			//	{
			//		D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
			//		m_vEye = _movevec;
			//		//m_vCameraPosDir = m_vEye - m_vAt;
			//		//1��Ī ���� �ƴ�
			//		m_bFirstPerson = false;

			//		_vec3 distance = m_vEye - m_vGoalPosition;

			//		// ���� ���ϴ� ī�޶� ��ġ�� ���� ��ġ�� ���̰� ���� �ȳ���
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
			//	// ������ ���� ����
			//	// ī�޶�� ���������
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
			//	// 1��Ī �������� ��ȯ
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
			//	// ������
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

		// ���콺 X�� �̵� ó��
		_vec3		vUp = { 0.f, 1.f, 0.f };

		_vec3	vLook = m_vEye - m_vAt;

		_matrix		matRotX, matRotY, matTotalRot;
		D3DXQuaternionRotationMatrix(&qRot, &matRotX);

		D3DXQuaternionRotationYawPitchRoll(&qRot, D3DXToRadian(dwMouseMoveX / 10.f), 0, 0);

		D3DXMatrixRotationQuaternion(&matRotX, &qRot);

		// ���콺 Y�� �̵� ó��
		_vec3		vCross;

		D3DXQuaternionRotationMatrix(&qRot, &matRotY);
		D3DXVec3Cross(&vCross, &m_vUp, &vLook);

		// ȸ�� ����
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

// ��ü �� ����ŷ
void CDynamicCamera::ShakeByPosition(const _float& fTimeDelta)
{
	if (m_bShakeCamera == true)
	{
		//��� �¾����� ��¦ ��鸮�°�

		if (m_fShakeTime > 0.0f)
		{
			// ����ŷ ���߱�
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

					// ��ǥ��ġ
					// moveDir�� �����ִ� ���� �۾ƾ���!!
					//m_vGoalPosition = m_vStartEyePosition + moveDir*0.2;
					// m_vGoalPosition = m_vStartEyePosition + moveDir * 0.2;


					m_vGoalPosition = m_vStartEyePosition + camright * 0.3;
				}
				else
				{
					//D3DXVECTOR3 _movevec;
					//// fTimeDelta�� 50 ���ϸ� ����..
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
			// ť�꿬�� ��
			//if (m_eAfterState == C_MOVE_TO_TARGET)
			//{
			//	//���󺹱�
			//	m_fShakeTime = 0;
			//	m_eAfterState = C_END;
			//	m_bFix = false; //��� Ǯ���ֱ�
			//	m_eCurState = C_MOVE_TO_TARGET;
			//	m_bChaseInit = true;
			//	m_bShakeCamera = false;
			//	Cinemachine_02_GoToIsaac();
			//	return;
			//}

			m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;
			m_bStopShakingWorld = false;
			//�ٽ� �÷��̾� ���ϰ�
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bShakeCamera = false;
			m_bFix = false; //��� Ǯ���ֱ�

			// �÷��̾� ��� Ǯ��
			CPlayer::GetInstance()->Set_KeyBlock(false);
		}

	}
}

void CDynamicCamera::ShakeByPosition_Sub(const _float& fTimeDelta)
{
	if (m_bShakeCamera_Sub == true)
	{
		//��� �¾����� ��¦ ��鸮�°�

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
				// fTimeDelta�� 50 ���ϸ� ����..
				D3DXVec3Lerp(&m_vShakeBoundary, &m_vShakeBoundary, &m_vGoalPosition, 0.9);
			}
			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			//m_vStartEyePosition = m_vCameraPosDir; -> �̷��� ��������
			m_vCameraPosDir = m_vStartEyePosition;
			m_fAngleY = 0;
			//m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;
			m_bShakeCamera_Sub = false;
			//�ٽ� �÷��̾� ���ϰ�
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bFix = false; //��� Ǯ���ֱ�

			// �÷��̾� ��� Ǯ��
			CPlayer::GetInstance()->Set_KeyBlock(false);
			
		}

	}
}

// rotation���� ������
void CDynamicCamera::ShakeByRotation(const _float& fTimeDelta)
{
	if (m_bShakeCamera_Rot)
	{
		//��� �¾����� ��¦ ��鸮�°�

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
					//����
					m_bChangeShakeDir = false;
					m_fTotalShakingAngle++;
				}
				else
				{
					//����
					m_bChangeShakeDir = true;
					m_fTotalShakingAngle--;
				}
			}
		
			// m_bChangeShakeDir = true; �ʱ�ȭ��
			if (m_bChangeShakeDir)
			{
				// true �� ����
				m_fTotalShakingAngle--;
				m_fAngleY = -1;
			}
			else
			{
				// false�� ���ϱ�
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
			// �����·�
			m_vCameraPosDir = m_vStartEyePosition;
			//m_vEye = m_vStartEyePosition;
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bFix = false; //��� Ǯ���ֱ�
			CPlayer::GetInstance()->Set_KeyBlock(false);
		}
	}
}


// ������ target�� �ε巴�� �̵�
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
				// �ó׸ӽ��ϸ� �����ž��Ѵ�
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


			// ���ȿ������϶� ���� ����
			if (m_bEpic == true)
			{
				//���ȿ����� ������
				m_eCurState = C_EPIC;
				// Ű���� ����� �� �ְԲ�
				CPlayer::GetInstance()->Set_KeyBlock(false);
				// Ÿ�� �����Ҷ��� ī�޶� ���콺 �̵� ����
				m_bFix = true;
				return;
			}

			if (m_bEpicCall == true)
			{
				// �ٽ� ���ڸ��� ���ư� �� �������� ����
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
				// ���콺 ������ ����
				m_bFix = true;
				//�÷��̾� ���Ǯ��
				CPlayer::GetInstance()->Set_KeyBlock(false);
			}


			m_bFix = false; //��� Ǯ���ֱ�

			// m_bFixedPos == true ���ڸ��� ����
			if (m_eAfterState == C_PLAYERCHASE && m_bFixedPos == true)
			{
				// ������������ �ٲ��� ������쿡�� bChaseInit = false�� ����� ���ο� �ڸ��� ���� ����
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
				// chaseinit ���ֱ�
				m_bChaseInit = true;
				m_bResetChaseInit = false;
			}
			


			// goalposition�� �������ϰ� ���� position���� ���ư��� ���
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

	// ���� �ε�������
	// if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
	if ((vPos.x > VTXCNTX - 3 || vPos.z > VTXCNTX - 3 || vPos.x < 3 || vPos.z < 3))
	{
		// ������ ��������
		return true;
	}
	else
	{
		return false;
	}
}


void CDynamicCamera::OnShakeCameraPos(float shakeTime, float shakeIntensity)
{
	// ī�޶� ��鸲 ����
	// public�̾ object �޾Ƽ� ����ұ� ������
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_bChangeShakeDir = false;

	//m_eCurState = C_SHAKING_POS;

	m_bShakeCamera = true;
	m_bFix = true; // ����� ������ ��� 

	m_vStartEyePosition = m_vEye;
	m_vGoalPosition = m_vEye;

	// �������� ���ϰ�
	CPlayer::GetInstance()->Set_KeyBlock(true);
}

void CDynamicCamera::OnShakeCameraPos_Sub(float shakeTime, float shakeIntensity)
{
	
	m_vGoalPosition = _vec3(0, 0, 0);
	m_vShakeBoundary = m_vGoalPosition;
	// ī�޶� ��鸲 ����
	// public�̾ object �޾Ƽ� ����ұ� ������
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_bChangeShakeDir = false;

	m_bShakeCamera_Sub = true;
	m_bFix = true; // ����� ������ ��� (ī�޶���)

	// cameraposdir ���� ����
	m_vStartEyePosition = m_vCameraPosDir;
	//m_vGoalPosition = m_vEye;

	// �������� ���ϰ�
	//CPlayer::GetInstance()->Set_KeyBlock(true);
}

void CDynamicCamera::OnShakeCameraRot(float shakeTime, float shakeIntensity)
{
	// ī�޶� ��鸲 ����
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	//m_iShakeNum = 0;
	
	//m_eCurState = C_SHAKING_ROT;

	/*m_vStartEyePosition = m_vEye;
	m_vStartAtPosition = m_vAt;*/

	m_vStartEyePosition = m_vCameraPosDir;
	
	// ����ŷ �� �ʱ�ȭ
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;
	m_fTotalShakingAngle = 0;

	m_bChangeShakeDir = true; // ���� �ٲ܋� ���
	m_bShakeCamera_Rot = true;
	m_bFix = true; // ����� ������ ��� 
}

void CDynamicCamera::OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate)
{
	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_eCurState = C_MOVE_TO_TARGET;

	m_bFix = true; // ����� ī�޶� ������ ��� 

	m_fMoveTime = moveTime;
	m_fMoveSpeed = moveSpeed;
	m_vGoalPosition = target;

	m_bFixedPos = fixedPosition;


	if (m_bFixedPos == false)
	{
		// �ٽ� ���ư����� ��� ù ������ ����
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

	m_bFix = true; // ����� ī�޶� ������ ��� 

	m_fMoveTime = moveTime;
	m_fMoveSpeed = moveSpeed;
	m_vGoalPosition = target;

	m_bFixedPos = fixedPosition;

	if (m_bFixedPos == false)
	{
		// �ٽ� ���ư����� ��� ù ������ ����
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

	m_bFix = true; // ����� ī�޶� ������ ��� 

	m_fMoveTime = moveTime;
	m_fMoveSpeed = moveSpeed;
	m_vGoalPosition = target;

	m_bFixedPos = fixedPosition;

	if (m_bFixedPos == false)
	{
		// �ٽ� ���ư����� ��� ù ������ ����
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
	// �ٶ󺸴� ����� �÷��̾�
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

	// �ٶ󺸴� ����� �÷��̾�
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
	// ���ȿ����� �Ϸ�
	m_bEpic = false;
	// ���ȿ����� �������°� �θ���
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

	// �ٶ󺸴� ����� �÷��̾�
	m_vAt = playerPos + playerDir * 2;

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vCameraPosDir = -(playerDir);

	_vec3	moveCamPos;

	
	moveCamPos = playerPos + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
	

	//OnMoveTargetCamera(1.f, 7.f, moveCamPos, false, 0);
	// 1��Ī�϶��� ���� ���� �ڸ��� ���ư��Բ�
	OnMoveTargetCamera(1.f, 7.f, m_vStartEyePosition, false, 0);
}

void CDynamicCamera::Cinemachine_00_Start()
{
	m_eAfterState = C_PLAYERCHASE;
	m_bChaseInit = true;
	m_eCurState = C_MOVE_TO_TARGET;
	m_fAngleY = 0;

	// ��������
	m_bStart = true;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;
	_vec3		cameraDir;
	_vec3		cameraPos;


	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);

	// ���� �÷��̾� ��ġ �޾ƿ��°� ����� �ȵż� �÷��̾� ���� ������ �߰� �������� �۵��ǰԲ���
	m_vAt = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) + (playerDir) * 2;

	m_vCameraPosDir = -(playerDir);
	
	// false�� �ϸ� m_vEye ���¸� m_vStartpos�� ������ ���ư��Բ� �ϱ� ������ 3��Ī �Ǵ� ������ m_vEye�� �ʱ�ȭ
	m_vEye = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos,startPos;


	goalPos = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) - (playerDir) * 3;
	startPos = _vec3(VTXCNTX / 2, 1, VTXCNTZ / 2) - (playerDir) * 3 + _vec3(6, 6, -3);

	m_bChaseInit = true;
	// false�ϸ� chaseInit�� true
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

	// �÷��̾� Ű ����
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

	// �÷��̾� Ű ����
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// �ٶ󺸴� ����� �÷��̾�
	

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

	// �÷��̾� Ű ����
	CPlayer::GetInstance()->Set_KeyBlock(true);

	_vec3	goalPos, startpos;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// �ٶ󺸴� ����� �÷��̾�
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

	// �÷��̾� Ű ����
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

	// ���Ƿ� ���� ����
	m_vStartEyePosition = m_vCameraPosDir;

	// atpos ���� ������ ���� ���־���
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
	// ���Ƿ� ���� ����
	m_vCameraPosDir = m_vStartEyePosition;

	// atpos ���� ������ ��� ���־���
	m_bFixAtPos = true;

	CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3		playerPos;
	_vec3		playerDir;

	playerInfo->Get_Info(INFO_POS, &playerPos);
	playerInfo->Get_Info(INFO_LOOK, &playerDir);

	D3DXVec3Normalize(&playerDir, &playerDir);
	// �ٶ󺸴� ����� �÷��̾�
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
