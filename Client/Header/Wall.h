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
	// ���� �ϳ����ؼ� �������� ť�긦 ���� ���� ť�긦 ������ �ְ��Ϸ��� ���� ���͹迭
	vector<CCubeObject*> m_vecCubes;

public:
	static CWall*		Create(LPDIRECT3DDEVICE9	pGraphicDev);

private:
	virtual void Free() override;
};

