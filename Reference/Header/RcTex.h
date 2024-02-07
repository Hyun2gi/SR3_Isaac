#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex : public CVIBuffer
{
private:
	explicit CRcTex();
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex();

public:
	virtual HRESULT		Ready_Buffer();
	virtual HRESULT		Ready_VBuffer();
	virtual void		Render_Buffer();

	void	Set_Color(D3DXCOLOR _color) { m_dwColor = _color; }

public:
	static CRcTex*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*	Clone();

private:
	LPDIRECT3DDEVICE9 m_pDev;

	D3DXCOLOR		m_dwColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

private:
	virtual void	Free();
};

END