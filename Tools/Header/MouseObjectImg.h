#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

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

	void Set_Cur_Texture_Name(string wstrTexture) { m_strCurTextureName = wstrTexture; }

	HRESULT Swap_Texture();

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculCom;

	string	m_strCurTextureName;

public:
	static CMouseObjectImg*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

