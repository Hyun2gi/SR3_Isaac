#pragma once

#include "Camera.h"

#include "Export_Utility.h"

class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

	// PLAYERCHASE : 플레이어 따라다님
	// WHOLELAND : 전체 맵 보여줌
	enum DYNAMICCAMERAID
	{
		C_PLAYERCHASE,
		C_WHOLELAND,
		C_SHAKING_POS, // Position 변화로 흔들림
		C_SHAKING_ROT, // Rotatation 변화로 흔들림
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
	// 카메라 움직임 설정 (흔들림 시작이나 움직임 시작할때 해당함수 불러주기)
	void	OnShakeCameraPos(float shakeTime = 1.0f, float shakeIntensity = 0.1f);
	void	OnShakeCameraRot(float shakeTime = 1.0f, float shakeIntensity = 0.1f);

	void	OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);
	void	OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);

	// 따봉할때
	void	OnMoveToPlayerFront();
	// 따봉 후
	void	OnMoveToOriginPos();

	// 에픽에투스 설정
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
	// 직접 사용하지 않고 On~ 함수를 통해 설정하면 자동으로 실행되는 함수들
	// 카메라 흔들림 실행하는 함수
	void	ShakeByPosition(const _float& fTimeDelta);
	void	ShakeByRotation(const _float& fTimeDelta);
	// 움직이는 함수 실행
	void	MoveToTarget(const _float& fTimeDelta);

private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_bool		m_bChaseInit = true;

	// 필요없을 경우 삭제하기
	// 카메라 흔들림 주기 전에 position
	_vec3		m_vStartEyePosition;
	_vec3		m_vOriginAtPosition;
	_vec3		m_vStartAtPosition;
	_vec3		m_vGoalPosition;


	// X,Y,Z 회전각
	float		m_fAngleX;
	float		m_fAngleY;
	float		m_fAngleZ;

	// 회전스피드, 이동스피드
	float		m_fRotateSpeed;
	float		m_fMoveSpeed;

	// 흔들림
	float		m_fShakeTime;
	float		m_fShakeIntensity;
	int			m_iShakeNum; // -, + 조절

	float		m_fMoveTime;

	_bool		m_bShake;
	_bool		m_bMove;
	_bool		m_bFirstPerson;


	// 이동 후에 goalposition에서 fixed 할건지 아니면 시작 position으로 갈건지
	_bool		m_bFixedPos;


	//플레이어와 카메라의 사이 거리와 방향
	//플레이어에서 해당방향 더해주기
	_vec3		m_vCameraPosDir;

	//현재 Epic 상태인지 아닌지 판별
	_bool		m_bEpic;
	//에픽에투스 누르고 플레이어로 돌아온 후에 떨어져야해서 판별
	_bool		m_bEpicCall;


	// 눈과 바로보는 지점의 거리
	// 바로보는 지점으로부터 눈까지 가는 방향벡터
	float		m_fCameraDistance;
	// y로의 길이
	float		m_fCameraHeight;
	float		m_fCameraShortDistance;
	float		m_fCameraShortHeight;

	bool		m_bCollisionWall;

	// target position
	CTransform*			m_pTarget;

	DYNAMICCAMERAID		m_eCurState;

	//필요없을시 삭제
	DYNAMICCAMERAID		m_ePreState;
	DYNAMICCAMERAID		m_eAfterState;

};

