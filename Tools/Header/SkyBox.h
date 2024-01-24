#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CTransform;

END

class CSkyBox :	public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

public:
	static CSkyBox*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

