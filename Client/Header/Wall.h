#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;

END


class CCubeObject;

class CWall :	public Engine::CGameObject
{
private:
	const int MAX_Y = 10;

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
	void			Set_Texture_Tag(const _tchar* pTextureTag, int iAxis);

	Engine::CTransform* Get_Transform() { return m_pTransformCom; }

	bool Get_Arrived();

private:
	HRESULT			Add_Component();

	void	Free_Cubes();

private:
	// 벽을 하나로해서 여러개의 큐브를 띄우기 위해 큐브를 가지고 있게하려고 만든 벡터배열
	vector<CCubeObject*> m_vecCubes;

	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	//Engine::CTexture* m_pTextureCom;

	bool m_bIsDeleted;
	bool m_bStartScene;

	_int	m_iCubeActionType;

public:
	static CWall*		Create(LPDIRECT3DDEVICE9	pGraphicDev, 
		_int iCubeActionType,
		bool bStartScene = false);

private:
	virtual void Free() override;
};

