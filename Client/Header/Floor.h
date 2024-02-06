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

	bool	Get_Arrived();

private:
	HRESULT			Add_Component();

private:
	// ���� �ϳ����ؼ� �������� ť�긦 ���� ���� ť�긦 ������ �ְ��Ϸ��� ���� ���͹迭
	bool m_bStartScene;
	vector<CCubeObject*> m_vecCubes;

public:
	static CFloor*		Create(LPDIRECT3DDEVICE9	pGraphicDev, bool bStart = false);

private:
	virtual void Free() override;
};

