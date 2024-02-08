#pragma once

#include "Camera.h"

BEGIN(Engine)

class CTransform;

END

class CTestCamera : public Engine::CCamera
{
private:
	explicit CTestCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestCamera();

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
	void		Mouse_Fix();
	void		Mouse_Move();
	
public:
	void		Set_Target(CComponent* pTarget);
	void		Set_Camera_Pos();

	_vec3		Get_Look() { return m_vLook; }
	_vec3		Get_Front() { return m_vFornt; }

	void				Set_FirstPerson(bool first) { m_bFirstPerson = first; m_bCollisionWall = true; m_bChaseInit = false; }
	 
private:
	_bool		m_bFix = false;
	_bool		m_bCheck = false;

public:
	static CTestCamera*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
									const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFov,
									const _float& fAspect,
									const _float& fNear,
									const _float& fFar);

private:
	virtual void Free();

private:
	CTransform* m_pTarget;
	_vec3 m_vFornt;
	_vec3 m_vLook;

	bool m_bFirstPerson, m_bCollisionWall, m_bChaseInit;
};

