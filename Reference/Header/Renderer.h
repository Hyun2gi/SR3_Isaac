#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup();

public:
	void		Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Alpha_Sorting(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Particles(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

	void		Set_Ending() { m_bIsEnd = true; }
	
private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	bool		m_bIsEnd = false;

private:
	virtual void		Free();
};

END