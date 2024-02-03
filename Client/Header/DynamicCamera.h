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
	void	OnShakeCameraRot(float shakeTime = 1.0f, float shakeIntensity = 0.1f);

	void	OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);
	void	OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);

	// �����Ҷ�
	void	OnMoveToPlayerFront();
	// ���� ��
	void	OnMoveToOriginPos();

	// ���ȿ����� ����
	void				Set_EpicBullet();
	void				Set_Shoot_End_Epic(); 

private:
	virtual void Free();
	void		Key_Input(const _float& fTimeDelta);
	void		Chase_Character(const _float& fTimeDelta);
	void		Whole_Land_Show();
	void		Mouse_Fix();
	void		Mouse_Move();

private:
	// ���� ������� �ʰ� On~ �Լ��� ���� �����ϸ� �ڵ����� ����Ǵ� �Լ���
	// ī�޶� ��鸲 �����ϴ� �Լ�
	void	ShakeByPosition(const _float& fTimeDelta);
	void	ShakeByRotation(const _float& fTimeDelta);
	// �����̴� �Լ� ����
	void	MoveToTarget(const _float& fTimeDelta);

private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_bool		m_bChaseInit = true;

	// �ʿ���� ��� �����ϱ�
	// ī�޶� ��鸲 �ֱ� ���� position
	_vec3		m_vStartEyePosition;
	_vec3		m_vOriginAtPosition;
	_vec3		m_vStartAtPosition;
	_vec3		m_vGoalPosition;


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
	int			m_iShakeNum; // -, + ����

	float		m_fMoveTime;

	_bool		m_bShake;
	_bool		m_bMove;
	_bool		m_bFirstPerson;


	// �̵� �Ŀ� goalposition���� fixed �Ұ��� �ƴϸ� ���� position���� ������
	_bool		m_bFixedPos;


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
	float		m_fCameraShortDistance;
	float		m_fCameraShortHeight;

	bool		m_bCollisionWall;

	// target position
	CTransform*			m_pTarget;

	DYNAMICCAMERAID		m_eCurState;

	//�ʿ������ ����
	DYNAMICCAMERAID		m_ePreState;
	DYNAMICCAMERAID		m_eAfterState;

};

