#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CTransform;

END

class CCubeObject :	public Engine::CGameObject
{
private:
	explicit CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeObject(const CCubeObject& rhs);
	virtual ~CCubeObject();

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

	HRESULT Set_Cute_Texture(const _tchar* pTextureTag);
	void	Set_Dst_Pos(_vec3 vDst);
	void	Set_Cube_Action_Type(_int iAction) { m_iActionType = iAction; }

	bool	Get_Arrived() { return m_bIsArrived; }

private:
	HRESULT			Add_Component();

	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound) // bad input
			return lowBound;

		// get random float in [0, 1] interval
		float f = (rand() % 10000) * 0.0001f;

		// return float in [lowBound, highBound] interval. 
		return (f * (highBound - lowBound)) + lowBound;
	}

private:
	Engine::CCubeTex*	m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_int m_iActionType;

public:
	static CCubeObject*		Create(LPDIRECT3DDEVICE9	pGraphicDev, bool bStartScene);

private:
	bool m_bIsArrived;
	bool m_bStartScene;

	_vec3 m_vDstPos;
	_vec3 m_vTempPos;

	_vec3 m_vDstScale;
	_vec3 m_vTempScale;
	bool	m_bIsExpanion;

	_vec3 m_vTempAngle;
	bool	m_bIsShaked;
	_float	m_fShakingTimer;

	_float m_fS;

private:
	virtual void Free() override;
};

