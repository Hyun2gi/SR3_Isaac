#pragma once

#include "Base.h"
#include "MapObj.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

END

class CMoveXObstacle :	public CMapObj
{
private:
	explicit CMoveXObstacle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMoveXObstacle(const CMoveXObstacle& rhs);
	virtual ~CMoveXObstacle();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	CTransform* Get_Transform() { return m_pTransformCom; }
	OBJECT_TYPE		Get_Type() { return m_eObjType; }

	HRESULT Set_Cute_Texture(const _tchar* pTextureTag);

	void	Set_Pos(_vec3 vPos);

	void	Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }

	void	Set_Distance_Left(_float fLeft) { m_fDistanceLeft = fLeft; }
	void	Set_Distance_Right(_float fRight) { m_fDistanceRight = fRight; }

private:
	HRESULT			Add_Component();

	void			Move(const _float& fTimeDelta);
	void			Check_Wall_Collision();

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	OBJECT_TYPE				m_eObjType;

	_float  m_fDistanceLeft, m_fDistanceRight;
	_float	m_fSpeed;
	bool	m_bTurn;
	_vec3	m_vPos;

public:
	static CMoveXObstacle*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

