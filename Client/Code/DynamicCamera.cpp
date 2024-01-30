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

	// �ش���ġ�� �̵�
	MoveToTarget(fTimeDelta);

	_int iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Chase_Character();
		Whole_Land_Show();
		//���콺 ������
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
			// �ٶ󺸴� ����� �÷��̾�
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
		// �ٶ󺸴� ����� �÷��̾�
		m_vAt = playerPos + playerDir * 2;

		// ���ο� ī�޶� ��ġ
		if (m_bChaseInit == true)
		{
			// ī�޶� ó�� �ʱ�ȭ
			// �÷��̾� �ڷ� ��ġ�ϰԲ� �ϰ� ���� ����
			m_vCameraPosDir = -(playerDir);
			m_vEye = m_vAt + m_vCameraPosDir * m_fCameraDistance + _vec3(0, m_fCameraHeight, 0);
		}
		else
		{
			// ȸ���Ѹ�ŭ ���̿� ������ ���Ϳ� ����Ǿ� �־
			// �÷��̾�� �ش���⸸ŭ ���
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

		// ���콺 X�� �̵� ó��
		_vec3		vUp = { 0.f, 1.f, 0.f };

		_vec3	vLook = m_vEye - m_vAt;

		_matrix		matRotX, matRotY, matTotalRot;
		D3DXQuaternionRotationMatrix(&qRot, &matRotX);

		D3DXQuaternionRotationYawPitchRoll(&qRot, D3DXToRadian(dwMouseMoveX / 10.f), 0, 0);

		D3DXMatrixRotationQuaternion(&matRotX, &qRot);

		// �÷��̾� - ���콺 X�� �̵������� ȸ��
		//dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"))->Rotation(ROT_Y, D3DXToRadian(dwMouseMoveX / 10.f));
		// �÷��̾� - ���콺 Y�� �̵������� ȸ��
		//dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"))->Rotation(ROT_Y, D3DXToRadian(dwMouseMoveX / 30.f));

		// ���콺 Y�� �̵� ó��
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

		// 90�� �̻� ȸ�� ���ϰ� �ϴ�
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

				// -1~1 ������ ���� ����

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
				D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta*60);

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
		if (m_fMoveTime > 0.0f)
		{
			D3DXVECTOR3 _movevec;
			D3DXVec3Lerp(&_movevec, &m_vEye, &m_vGoalPosition, fTimeDelta * m_fMoveSpeed);

			m_vEye = _movevec;
		}
		else
		{
			m_bMove = false;
			m_bFix = false; //��� Ǯ���ֱ�
			m_eCurState = C_PLAYERCHASE;

			// goalposition�� �������ϰ� ���� position���� ���ư��� ���
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

void CDynamicCamera::OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition)
{
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
}

void CDynamicCamera::OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition)
{
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
