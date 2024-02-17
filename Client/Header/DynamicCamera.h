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
	// 카메라 움직임 설정 (흔들림 시작이나 움직임 시작할때 해당함수 불러주기)
	void	OnShakeCameraPos(float shakeTime = 1.0f, float shakeIntensity = 0.1f);
	void	OnShakeCameraPos_Sub(float shakeTime = 1.0f, float shakeIntensity = 0.1f);
	void	OnShakeCameraRot(float shakeTime = 1.0f, float shakeIntensity = 0.1f);

	void	OnMoveTargetCamera(float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);
	void	OnMoveTargetCamera(_vec3 atPos, float moveTime, float moveSpeed, _vec3 target, bool fixedPosition, int afterstate);

	void	OnMoveTargetCamera(
		_vec3 atPos, // m_vAt : at 지점 
		float moveTime, 
		float moveSpeed, 
		_vec3 target, 
		_vec3 startpos, // cinemachine 할 때 시작장소
		bool fixedPosition,  
		int afterstate
	);

	// 따봉할때
	void	OnMoveToPlayerFront();
	// 따봉 후
	void	OnMoveToOriginPos();

	// 에픽에투스 설정
	void				Set_EpicBullet();
	void				Set_Shoot_End_Epic(); 

	// 시네머신용 카메라 (우는 아이작)
	void				Cinemachine_00_Start();
	// 큐브연출 있을 때 전체 화면으로 보기
	void				Cinemachine_01_TotalLand();
	// 조금내려가서 보게끔(안씀)
	void				Cinemachine_02_MiddleTotalLand();
	// 다시 플레이어로 돌아가는 연출
	void				Cinemachine_03_GoToIsaac();
	void				Cinemachine_04_Dople_GoToIsaac();
	

	void				Set_ChaseInit(bool _init) { m_bChaseInit = _init; }
	void				Set_FirstPerson(bool first) { m_bFirstPerson = first; m_bCollisionWall = true; m_bChaseInit = false; }

	_vec3				Get_EyePos() { return m_vEye; }

	bool				Get_FirstPerson() { return m_bFirstPerson; }

	// 카메라 움직임 등 카메라 자체 움직임을 시작
	void				Set_TotalCameraStart() { m_bMouseCameraStart = true; }

	bool				Get_ShakingCamera() { if (m_bShakeCamera || m_bShakeCamera_Sub || m_bShakeCamera_Rot) { return true; } else { return false; } }

	void				Set_StopShaking() { m_bStopShakingWorld = true; }

	// 슬롯모드 켜기
	void				Set_OnSlotCamera(CTransform* slotTransform);
	// 슬롯모드 끄기
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
	// 직접 사용하지 않고 On~ 함수를 통해 설정하면 자동으로 실행되는 함수들
	// 카메라 흔들림 실행하는 함수
	void	ShakeByPosition(const _float& fTimeDelta);
	void	ShakeByPosition_Sub(const _float& fTimeDelta);
	void	ShakeByRotation(const _float& fTimeDelta);
	// 움직이는 함수 실행
	void	MoveToTarget(const _float& fTimeDelta);

	bool	CheckCollisionWall(float distance);


private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_bool		m_bChaseInit = true;

	// 슬롯머신때 설정된 atpos에 고정
	_bool		m_bFixAtPos = false;

	// 사운드위해 만든 스타트한지 안한지
	_bool		m_bStart;

	// 카메라 자체 스타트인지 아닌지
	_bool		m_bMouseCameraStart = false;

	// 필요없을 경우 삭제하기
	// 카메라 흔들림 주기 전에 position
	_vec3		m_vStartEyePosition;
	_vec3		m_vOriginAtPosition;
	_vec3		m_vStartAtPosition;
	_vec3		m_vGoalPosition;

	_vec3		m_vShakeBoundary;

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
	bool		m_bChangeShakeDir; // -, + 조절

	float		m_fMoveTime;

	_bool		m_bShakeCamera;
	_bool		m_bShakeCamera_Sub;
	_bool		m_bShakeCamera_Rot = false; // 회전으로 쉐이킹
	float		m_fTotalShakingAngle = 0;

	_vec3		m_vWorldAtDir; // shake 연출 있을때 at 포지션을 위해
	_bool		m_bStopShakingWorld = false;

	_bool		m_bFirstPerson;
	_bool		m_bPreFirstPerson;


	// 이동 후에 goalposition에서 fixed 할건지 아니면 시작 position으로 갈건지
	_bool		m_bFixedPos;
	_bool		m_bResetChaseInit; // 다시 chaseinit true할것인지 말건지


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

	_bool		m_bCollisionWall;
	_bool		m_bPreCollisionWall;

	// (최대 고정된 거리)
	float		m_fTotalDistanceWithPlayer;
	float		m_fFlexibleDistanceWithPlayer;

	// target position
	CTransform*			m_pTarget;

	DYNAMICCAMERAID		m_eCurState;

	//필요없을시 삭제
	DYNAMICCAMERAID		m_ePreState;
	DYNAMICCAMERAID		m_eAfterState;

};

