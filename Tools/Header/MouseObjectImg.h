#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CTransform;

END

class CMouseObjectImg :	public Engine::CGameObject
{
private:
	explicit CMouseObjectImg(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMouseObjectImg(const CMouseObjectImg& rhs);
	virtual ~CMouseObjectImg();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_Material();

private:
	Engine::CTerrainTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

public:
	static CMouseObjectImg*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

