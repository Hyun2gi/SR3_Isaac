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
	m_fCameraDistance = 8.f;

	m_pTarget = nullptr;
	m_ePreState = C_END;
	m_eCurState = C_PLAYERCHASE;

	m_bShake = false;
	m_bMove = false;
	m_bCollisionWall = false;

	m_fAngleY = 0;

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
	ShakeByRotation(fTimeDelta);

	// �ش���ġ�� �̵�
	MoveToTarget(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

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
		Mouse_Move();
		Chase_Character(fTimeDelta);
		//���콺 ������

		if (m_eCurState != C_EPIC)
		{
			// EPIC �϶��� ���콺 ������ �� �ְ��ϱ�
			// EPIC �ƴҶ��� ���콺 ������ �߰�����
			Mouse_Fix();
		}

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
		if (m_bShake == false)
		{
			//OnShakeCameraPos(float shakeTime, float shakeIntensity)
			OnShakeCameraPos(0.5, 2);
			//OnShakeCameraRot(2, 2);
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
		}
		else
		{
			// ȸ���Ѹ�ŭ ���̿� ������ ���Ϳ� ����Ǿ� �־
			// �÷��̾�� �ش���⸸ŭ ���
			_vec3 vPos = m_vAt + m_vCameraPosDir * m_fTotalDistanceWithPlayer;

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


			if (m_bCollisionWall == true)
			{
				// ī�޶�� �� �浹�߻�
				while (CheckCollisionWall(m_fFlexibleDistanceWithPlayer))
				{
					m_fFlexibleDistanceWithPlayer-=0.001;
				}
				m_vGoalPosition = m_vAt + m_vCameraPosDir * m_fFlexibleDistanceWithPlayer;
				D3DXVECTOR3 _movevec;
				D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 10);
				m_vEye = m_vAt + m_vCameraPosDir * m_fFlexibleDistanceWithPlayer;;
				m_fAngleY = 0;
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

// position ��ȭ�� �����̱�..
// ��鸲 ª�� �ٶ� ���� ���������� ������� ����ŷ���� ���x
void CDynamicCamera::ShakeByPosition(const _float& fTimeDelta)
{
	if (m_eCurState == C_SHAKING_POS)
	{
		//��� �¾����� ��¦ ��鸮�°�

		if (m_fShakeTime > 0.0f)
		{
			if (m_vGoalPosition == m_vEye)
			{
				float FLOAT_MAX = 1;
				float FLOAT_MIN = 0;

				srand((unsigned)time(NULL));

				_vec3 templook;
				templook = _vec3(m_vAt.x, 0, m_vAt.z) - _vec3(m_vStartAtPosition.x, 0, m_vStartAtPosition.x);
				templook = m_vAt - m_vStartAtPosition;

				D3DXVec3Normalize(&templook, &templook);


				_vec3 moveDir;
				D3DXVec3Cross(&moveDir, &(_vec3(0, 1, 0)), &templook);

				//�׽�Ʈ�߰�
				//moveDir = _vec3(moveDir.x, 0, moveDir.z);
				// x������ �����϶��� �翷����
				// moveDir = _vec3(1, 0, 0);
				//moveDir = _vec3(0.5, 0, 0.5);

				D3DXVec3Normalize(&moveDir, &moveDir);



				if (m_iShakeNum % 2 == 0)
				{
					moveDir *= -1;
				}
				m_iShakeNum++;

				// ��ǥ��ġ
				// moveDir�� �����ִ� ���� �۾ƾ���!!
				//m_vGoalPosition = m_vStartEyePosition + moveDir*0.2;
				m_vGoalPosition = m_vStartEyePosition + moveDir * 0.2;
			}
			else
			{
				D3DXVECTOR3 _movevec;
				// fTimeDelta�� 50 ���ϸ� ����..
				D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * 60);

				m_vEye = _movevec;
			}

			m_fShakeTime -= fTimeDelta;
		}
		else
		{
			m_vEye = m_vStartEyePosition;
			//m_vAt = m_vStartAtPosition;

			//�ٽ� �÷��̾� ���ϰ�
			m_eCurState = C_PLAYERCHASE;
			m_fShakeTime = 0;
			m_bShake = false;
			m_bFix = false; //��� Ǯ���ֱ�
		}

	}
}

// rotation���� ������
void CDynamicCamera::ShakeByRotation(const _float& fTimeDelta)
{
	if (m_eCurState == C_SHAKING_ROT)
	{
		//��� �¾����� ��¦ ��鸮�°�

		if (m_fShakeTime > 0.0f)
		{
			//m_vEye = m_vStartPosition;
			srand((unsigned)time(NULL));

			//�������� �����̰� ������

			float randx = (float)(rand() % 30);
			//float randy = (float)(rand() % 30);
			float randz = (float)(rand() % 30);

			// ���� ȸ������ �����϶� ���
			float randy = 5;

			if (m_iShakeNum % 2 == 1)
			{
				randy *= -1;
			}
			D3DXQUATERNION qRot;

			_matrix		matRotX, matRotY, matRotZ, matTotalRot;
			D3DXQuaternionRotationMatrix(&qRot, &matRotX);

			// yaw, pitch, roll ����
			// yaw : y��, pitch : x��, roll : z��
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
			m_bFix = false; //��� Ǯ���ֱ�
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
			m_vAt = playerPos + playerDir * 2;
		}
		else
		{
			m_bMove = false;
			m_bFix = false; //��� Ǯ���ֱ�


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

			if (m_eAfterState == C_PLAYERCHASE && m_bFix == false)
			{
				m_eCurState = C_PLAYERCHASE;
				m_bChaseInit = true;
				m_bFix = false;
			}
			else if (m_eAfterState == C_PLAYERCHASE && m_bFix == true)
			{
				// ������������ �ٲ��� ������쿡�� bChaseInit = false�� ����� ���ο� �ڸ��� ���� ����
				m_eCurState = C_PLAYERCHASE;
				m_bChaseInit = false;
				m_bFix = false;
			}
			else if (m_eAfterState == C_MOVE_TO_TARGET)
			{
				m_eCurState = C_MOVE_TO_TARGET;
				m_bFix = true;
			}

			// goalposition�� �������ϰ� ���� position���� ���ư��� ���
			if (m_bFixedPos == true)
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
	_vec3 vPos = m_vAt + m_vCameraPosDir * distance;

	// ���� �ε�������
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
	m_iShakeNum = 0;

	m_eCurState = C_SHAKING_POS;

	m_vStartEyePosition = m_vEye;

	m_bShake = true;
	m_bFix = true; // ����� ������ ��� 

	m_vGoalPosition = m_vEye;
}

void CDynamicCamera::OnShakeCameraRot(float shakeTime, float shakeIntensity)
{
	// ī�޶� ��鸲 ����
	m_fShakeTime = shakeTime;
	m_fShakeIntensity = shakeIntensity;
	m_iShakeNum = 0;

	m_eCurState = C_SHAKING_ROT;

	m_vStartEyePosition = m_vEye;
	m_vStartAtPosition = m_vAt;

	// ����ŷ �� �ʱ�ȭ
	m_fAngleX = 0;
	m_fAngleY = 0;
	m_fAngleZ = 0;

	m_bShake = true;
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
	}
}

void CDynamicCamera::OnMoveToPlayerFront()
{
	CPlayer::GetInstance()->Set_KeyBlock(true);
	m_bMove = true;
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
	m_vAt = playerPos + playerDir * 2;

	m_bFix = true;

	CPlayer::GetInstance()->Set_KeyBlock(true);

	m_vCameraPosDir = -(playerDir);

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

	OnMoveTargetCamera(1.f, 7.f, m_vStartEyePosition, true, 0);
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
