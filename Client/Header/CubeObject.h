#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CTransform;

END

class CCubeObject :	public Engine::CGameObject
{
private:
	explicit CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeObject(const CCubeObject& rhs);
	virtual ~CCubeObject();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT Set_Cute_Texture(const _tchar* pTextureTag);
	void Set_Dst_Pos(_vec3 vDst) { m_vDstPos = m_vTempPos = vDst; }

	bool	Get_Arrived() { return m_bIsArrived; }

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex*	m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

public:
	static CCubeObject*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	bool m_bIsArrived;

	_vec3 m_vDstPos;
	_vec3 m_vTempPos;

	_float m_fS;

private:
	virtual void Free() override;
};

