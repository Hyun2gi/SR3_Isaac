#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CBossHP : public Engine::CUI
{
private:
	explicit CBossHP(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBossHP(const CBossHP& rhs);
	virtual ~CBossHP();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CBossHP* Create();

private:
	virtual void Free() override;
};

