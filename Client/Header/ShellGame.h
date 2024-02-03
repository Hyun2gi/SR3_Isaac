#pragma once

#include "MapObj.h"
#include "GameObject.h"

#include "ShellNpc.h"
#include "Shell.h"

class CShellGame : public CMapObj
{
private:
	const float SCALAR_X = -0.9f;
	const float SCALAR_Y = 0.1f;

private:
	explicit CShellGame(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShellGame(const CShellGame& rhs);
	virtual ~CShellGame();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Create_NPC();
	void				Create_Shell();

private:
	_bool				m_bGame;

	CShellNpc*			m_pShellNpc;
	vector<CShell*>		m_vecShell;

public:
	static CShellGame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;

};