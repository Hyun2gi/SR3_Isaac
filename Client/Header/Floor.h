#pragma once

#include "Base.h"
#include "GameObject.h"

class CCubeObject;

class CFloor :	public Engine::CGameObject
{
private:
	explicit CFloor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFloor(const CFloor& rhs);
	virtual ~CFloor();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT			Set_Cube_Texture_Tag(const _tchar* pCubeTextureTag);

private:
	HRESULT			Add_Component();

private:
	vector<CCubeObject*> m_vecCubes;

	//vector<const _tchar*> m_vecTextureTag;

public:
	static CFloor*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

