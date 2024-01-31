#pragma once

#include "Base.h"
#include "GameObject.h"

class CCubeObject;

class CWall :	public Engine::CGameObject
{
private:
	const int MAX_Y = 6;

	enum Wall_Axis
	{
		WALL_LEFT,
		WALL_RIGHT,
		WALL_TOP,
		WALL_BOTTOM
	};

private:
	explicit CWall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWall(const CWall& rhs);
	virtual ~CWall();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT			Set_Cube_Texture_Tag(const _tchar* pCubeTextureTag, int iAxis);

	bool Get_Arrived();

private:
	HRESULT			Add_Component();

private:
	// 벽을 하나로해서 여러개의 큐브를 띄우기 위해 큐브를 가지고 있게하려고 만든 벡터배열
	vector<CCubeObject*> m_vecCubes;

public:
	static CWall*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

