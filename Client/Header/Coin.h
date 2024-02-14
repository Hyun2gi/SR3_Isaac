#pragma once

#include "Item.h"
#include "GameObject.h"

class CCoin : public CItem
{
private:
	explicit CCoin(LPDIRECT3DDEVICE9 pGraphicDev, int iID);
	explicit CCoin(const CCoin& rhs);
	virtual ~CCoin();

	enum COINSTATE
	{
		COIN_IDLE,
		COIN_GET,
		COIN_END
	};

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject()					 override;
	virtual void	Render_GameObject()						 override;

public:
	bool			Run_Item_Effect()						override;
	void			Item_Spawn_Action()						override;

private:
	virtual HRESULT		Add_Component()						override;
	virtual void		Motion_Change()						override;

private:
	_float					m_fPicNum;

	COINSTATE				m_ePreState;
	COINSTATE				m_eCurState;

	// ���� ��¦ �Ÿ��°� ���� ���ϰ� ������
	_int					m_iDelay;
	_int					m_iRandNum;

public:
	static CCoin* Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look, int iID);

private:
	virtual void		Free()									override;

};

