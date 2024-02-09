#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

END

class CObstacle :	public Engine::CGameObject
{
private:
	explicit CObstacle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CObstacle(const CObstacle& rhs);
	virtual ~CObstacle();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT Set_Cute_Texture(const _tchar* pTextureTag);

	void	Set_Pos(_vec3 vPos);

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CObstacle*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

