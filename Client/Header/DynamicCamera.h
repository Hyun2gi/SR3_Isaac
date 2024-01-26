#pragma once

#include "Camera.h"

#include "Export_Utility.h"

class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

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

private:
	void		Key_Input(const _float& fTimeDelta);

	void		Chase_Character();
	void		Mouse_Fix();
	void		Mouse_Move();

private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_bool		m_bChaseInit = true;

	//플레이어와 카메라의 사이 거리와 방향
	//플레이어에서 해당방향 더해주기
	_vec3		m_vCameraPosDir;

	//x,z로의 길이
	float		m_fCameraDistance;

	//y로의 길이
	float		m_fCameraHeight;


	CTransform* m_pTarget;

public:
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

private:
	virtual void Free();
};

