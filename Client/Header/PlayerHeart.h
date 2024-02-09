#pragma once

#include "Base.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CPlayerHeart : public Engine::CUI
{
private:
	explicit CPlayerHeart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerHeart(const CPlayerHeart& rhs);
	virtual ~CPlayerHeart();

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
	static CPlayerHeart* Create();

private:
	virtual void Free() override;
};

