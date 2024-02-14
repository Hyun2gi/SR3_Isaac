#pragma once

#include "Item.h"
#include "GameObject.h"

class CEpic : public CItem
{
	explicit CEpic(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEpic(const CEpic& rhs);
	virtual ~CEpic();

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
	// ���������� �� �̹��� �����ϸ� ��� ����
	_float					m_fPicNum;
	int						m_iUpTimer;

	// �εս��� ���� ��
	float					m_fTop;
	float					m_fFloor;
	int						m_iSubTimer;
	int						m_iSubSizeTimer;
	float					m_fBig;
	float					m_fSmall;


public:
	static CEpic* Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look);

private:
	virtual void		Free()									override;
};

