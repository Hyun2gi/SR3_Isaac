#pragma once

#include "Base.h"
#include "MapObj.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

END

class CMoveZObstacle :	public CMapObj
{
private:
	explicit CMoveZObstacle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMoveZObstacle(const CMoveZObstacle& rhs);
	virtual ~CMoveZObstacle();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	CTransform* Get_Transform() { return m_pTransformCom; }

	HRESULT Set_Cute_Texture(const _tchar* pTextureTag);

	void	Set_Pos(_vec3 vPos);

	void	Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }

	void	Set_Distance_Up(_float fLeft) { m_fDistanceUp = fLeft; }
	void	Set_Distance_Down(_float fRight) { m_fDistanceDown = fRight; }

	void	Set_Turn() { m_bTurn = !m_bTurn; }

private:
	HRESULT			Add_Component();

	void			Move(const _float& fTimeDelta);
	void			Check_Wall_Collision();

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_bool	m_bTurn;
	_bool	m_bArrived = false;

	_float  m_fDistanceUp, m_fDistanceDown;
	_float	m_fSpeed, m_fDropSpeed;
	_float	m_fDorpTimer = 0.f;

	_vec3	m_vPos;

public:
	static CMoveZObstacle*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

