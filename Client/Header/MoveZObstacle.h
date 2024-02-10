#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

END

class CMoveZObstacle :	public Engine::CGameObject
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

private:
	HRESULT			Add_Component();

	void			Move(const _float& fTimeDelta);

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_float  m_fDistanceUp, m_fDistanceDown;
	_float	m_fSpeed;
	bool	m_bTurn;
	_vec3	m_vPos;

public:
	static CMoveZObstacle*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

