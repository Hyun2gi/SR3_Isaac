#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCalculator;

END

class CPlacementObject : public Engine::CGameObject
{
private:
	explicit CPlacementObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlacementObject(const CPlacementObject& rhs);
	virtual ~CPlacementObject();

public:
	HRESULT Ready_GameObject(wstring wstrName, int iType, int iIndex);
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	void Set_TextureName(wstring wstrName) { m_wstrTextureName = wstrName; }

	void Set_Obj_State(int iType, int iIndex) 
	{
		m_iObjType = iType;
		m_iIndex = iIndex;
	}

private:
	HRESULT			Add_Component();
	HRESULT			Add_TextureComponent();

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculCom;

	wstring m_wstrTextureName;
	wstring m_wstrProtoName;
	int m_iObjType;
	int m_iIndex;

public:
	static CPlacementObject*		Create(LPDIRECT3DDEVICE9	pGraphicDev, wstring wstrName, int iType, int iIndex);

private:
	virtual void Free() override;
};

