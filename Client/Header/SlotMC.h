#pragma once

#include "MapObj.h"
#include "GameObject.h"

#include "Machine.h"

class CSlotMC : public CMapObj
{
private:
	explicit CSlotMC(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSlotMC(const CSlotMC& rhs);
	virtual ~CSlotMC();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

private:
	virtual HRESULT		Add_Component()						override;

	void				Create_Machine();
	void				Create_Card();

private:
	// º¯¼ö
	_bool				m_bCreate;

	CMachine*			m_pMachine;

public:
	static CSlotMC*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free()								override;
};

