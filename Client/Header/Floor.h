#pragma once

#include "Base.h"
#include "GameObject.h"

class CCubeObject;

BEGIN(Engine)

class CTransform;

END

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

	HRESULT		Create_Cubes(bool bBool);

	Engine::CTransform* Get_Transform() { return m_pTransformCom; }

	bool	Get_Arrived();

private:
	HRESULT			Add_Component();

private:
	// 벽을 하나로해서 여러개의 큐브를 띄우기 위해 큐브를 가지고 있게하려고 만든 벡터배열
	bool m_bStartScene;

	Engine::CTransform* m_pTransformCom;
	vector<CCubeObject*> m_vecCubes;

	_int	m_iCubeActionType;

	_int		m_iCubeCount;
	_int		m_iCubeMaxCount;
	_float		m_fCubeCreateTimer;

	wstring m_wstrTexture;

	bool m_bGood = true;

public:
	static CFloor*		Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_int iCubeActionType,
		bool bStart = false);

private:
	virtual void Free() override;
};

