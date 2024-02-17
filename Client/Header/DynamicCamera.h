#pragma once

#include "Camera.h"

#include "Export_Utility.h"

class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

	// PLAYERCHASE : �÷��̾� ����ٴ�
	// WHOLELAND : ��ü �� ������
	enum DYNAMICCAMERAID
	{
		C_PLAYERCHASE,
		C_WHOLELAND,
		C_SHAKING_POS, // Position ��ȭ�� ��鸲
		C_SHAKING_ROT, // Rotatation ��ȭ�� ��鸲
		C_MOVE_TO_TARGET,
		C_EPIC,
		C_CINEMACHINE_02,
		C_SLOTCLOSE,
		C_DOPLE,
		C_END
	};

public:
	HRESULT Ready_GameObject(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;

	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	void					Set_Target(CTransform* _target)
	{
		m_pTarget = _target;
	}

public:
	// ī�޶� ������ ���� (��鸲 �����̳� ������ �����Ҷ� �ش��Լ� �ҷ��ֱ�)
	void	OnShakeCameraPos(float shakeTime = 1.0f, float shakeIntensity = 0.1f);
	void	OnShakeCameraPos_Sub(float shakeTime = 1.0f, float shakeIntensity = 0.1f);
	void	OnShakeCameraRot(float shakeTime = 1.0f, float shakeIntensity = 0.1f);

	void	OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);
	void	OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);

	void	OnMoveTargetCamera(
		_vec3 atPos, // m_vAt : at ���� 
		float moveTime, 
		float moveSpeed, 
		_vec3 target, 
		_vec3 startpos, // cinemachine �� �� �������
		bool fixedPosition,  
		int afterstate
	);

	// �����Ҷ�
	void	OnMoveToPlayerFront();
	// ���� ��
	void	OnMoveToOriginPos();

	// ���ȿ����� ����
	void				Set_EpicBullet();
	void				Set_Shoot_End_Epic(); 

	// �ó׸ӽſ� ī�޶� (��� ������)
	void				Cinemachine_00_Start();
	// ť�꿬�� ���� �� ��ü ȭ������ ����
	void				Cinemachine_01_TotalLand();
	// ���ݳ������� ���Բ�(�Ⱦ�)
	void				Cinemachine_02_MiddleTotalLand();
	// �ٽ� �÷��̾�� ���ư��� ����
	void				Cinemachine_03_GoToIsaac();
	void				Cinemachine_04_Dople_GoToIsaac();
	

	void				Set_ChaseInit(bool _init) { m_bChaseInit = _init; }
	void				Set_FirstPerson(bool first) { m_bFirstPerson = first; m_bCollisionWall = true; m_bChaseInit = false; }

	_vec3				Get_EyePos() { return m_vEye; }

	bool				Get_FirstPerson() { return m_bFirstPerson; }

	// ī�޶� ������ �� ī�޶� ��ü �������� ����
	void				Set_TotalCameraStart() { m_bMouseCameraStart = true; }

	bool				Get_ShakingCamera() { if (m_bShakeCamera || m_bShakeCamera_Sub || m_bShakeCamera_Rot) { return true; } else { return false; } }

	void				Set_StopShaking() { m_bStopShakingWorld = true; }

	// ���Ը�� �ѱ�
	void				Set_OnSlotCamera(CTransform* slotTransform);
	// ���Ը�� ����
	void				Set_OffSlotCamera();

	void				Set_OnDople();
	void				Set_OffDople();

private:
	virtual void Free();
	void		Key_Input(const _float& fTimeDelta);
	void		Chase_Character(const _float& fTimeDelta);
	void		Mouse_Fix();
	void		Mouse_Move();

private:
	// ���� ������� �ʰ� On~ �Լ��� ���� �����ϸ� �ڵ����� ����Ǵ� �Լ���
	// ī�޶� ��鸲 �����ϴ� �Լ�
	void	ShakeByPosition(const _float& fTimeDelta);
	void	ShakeByPosition_Sub(const _float& fTimeDelta);
	void	ShakeByRotation(const _float& fTimeDelta);
	// �����̴� �Լ� ����
	void	MoveToTarget(const _float& fTimeDelta);

	bool	CheckCollisionWall(float distance);


private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_bool		m_bChaseInit = true;

	// ���ԸӽŶ� ������ atpos�� ����
	_bool		m_bFixAtPos = false;

	// �������� ���� ��ŸƮ���� ������
	_bool		m_bStart;

	// ī�޶� ��ü ��ŸƮ���� �ƴ���
	_bool		m_bMouseCameraStart = false;

	// �ʿ���� ��� �����ϱ�
	// ī�޶� ��鸲 �ֱ� ���� position
	_vec3		m_vStartEyePosition;
	_vec3		m_vOriginAtPosition;
	_vec3		m_vStartAtPosition;
	_vec3		m_vGoalPosition;

	_vec3		m_vShakeBoundary;

	// X,Y,Z ȸ����
	float		m_fAngleX;
	float		m_fAngleY;
	float		m_fAngleZ;

	// ȸ�����ǵ�, �̵����ǵ�
	float		m_fRotateSpeed;
	float		m_fMoveSpeed;

	// ��鸲
	float		m_fShakeTime;
	float		m_fShakeIntensity;
	bool		m_bChangeShakeDir; // -, + ����

	float		m_fMoveTime;

	_bool		m_bShakeCamera;
	_bool		m_bShakeCamera_Sub;
	_bool		m_bShakeCamera_Rot = false; // ȸ������ ����ŷ
	float		m_fTotalShakingAngle = 0;

	_vec3		m_vWorldAtDir; // shake ���� ������ at �������� ����
	_bool		m_bStopShakingWorld = false;

	_bool		m_bFirstPerson;
	_bool		m_bPreFirstPerson;


	// �̵� �Ŀ� goalposition���� fixed �Ұ��� �ƴϸ� ���� position���� ������
	_bool		m_bFixedPos;
	_bool		m_bResetChaseInit; // �ٽ� chaseinit true�Ұ����� ������


	//�÷��̾�� ī�޶��� ���� �Ÿ��� ����
	//�÷��̾�� �ش���� �����ֱ�
	_vec3		m_vCameraPosDir;

	//���� Epic �������� �ƴ��� �Ǻ�
	_bool		m_bEpic;
	//���ȿ����� ������ �÷��̾�� ���ƿ� �Ŀ� ���������ؼ� �Ǻ�
	_bool		m_bEpicCall;


	// ���� �ٷκ��� ������ �Ÿ�
	// �ٷκ��� �������κ��� ������ ���� ���⺤��
	float		m_fCameraDistance;
	// y���� ����
	float		m_fCameraHeight;

	_bool		m_bCollisionWall;
	_bool		m_bPreCollisionWall;

	// (�ִ� ������ �Ÿ�)
	float		m_fTotalDistanceWithPlayer;
	float		m_fFlexibleDistanceWithPlayer;

	// target position
	CTransform*			m_pTarget;

	DYNAMICCAMERAID		m_eCurState;

	//�ʿ������ ����
	DYNAMICCAMERAID		m_ePreState;
	DYNAMICCAMERAID		m_eAfterState;

};

