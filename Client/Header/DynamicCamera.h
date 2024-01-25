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

	//�÷��̾�� ī�޶��� ���� �Ÿ��� ����
	//�÷��̾�� �ش���� �����ֱ�
	_vec3		m_vCameraPosDir;

	//x,z���� ����
	float		m_fCameraDistance;

	//y���� ����
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

