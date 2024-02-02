#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI :	public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;

	void	Compute_ProjectionMatrix();

	_int	Get_RenderIndex() { return m_iRenderIndex; }
	void	Set_RenderIndex(_int iRenderIndex) { m_iRenderIndex = iRenderIndex; }

	void	Set_Size(_float x, _float y) 
	{
		m_fSizeX = x * 0.5f;
		m_fSizeY = y * 0.5f;
	}

	void	Set_WindowSize(_float x, _float y)
	{
		m_fWinCX = x;
		m_fWinCY = y;
	}

	void	Set_Pos(_float x, _float y, _float winX, _float winY)
	{
		// 윈도우 좌표계로 설정해주는 계산식
		//m_fPosX = x - winX * 0.5f;
		//m_fPosY = -y + winY * 0.5f;
		m_fPosX = x;
		m_fPosY = y;
	}

protected:
	_float			m_fWinCX, m_fWinCY;
	_float			m_fSizeX, m_fSizeY;
	_float			m_fPosX, m_fPosY;
	_matrix			m_matView, m_matProj;

	//임의의 큰 값
	_int m_iRenderIndex = 99;

protected:
	virtual void	Free();
};

END

