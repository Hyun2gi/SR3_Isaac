#pragma once

#include "Engine_Define.h"
#include "Base.h"
#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene :public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	CComponent*			Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);
	CGameObject*		Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);

	virtual void Delete_Layer(const _tchar* pLayerTag)
	{
		_ulong dwRefCnt = 0;

		if (m_mapLayer.find(pLayerTag) == m_mapLayer.end()) return;

		dwRefCnt = m_mapLayer.at(pLayerTag)->Release();

		if (0 == dwRefCnt)
			m_mapLayer.at(pLayerTag) = NULL;

		m_mapLayer.erase(pLayerTag);
	}


public:
	virtual HRESULT		Ready_Scene();
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		LateUpdate_Scene();
	virtual void		Render_Scene()PURE;


protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	map<const _tchar*, CLayer*>	m_mapLayer;

public:
	virtual void Free();
};

END