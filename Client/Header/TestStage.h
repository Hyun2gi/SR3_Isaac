#pragma once

#include "Scene.h"

BEGIN(Engine)

class CParticleScatter;
class CParticleExplosion;
class CParticleSplash;

END


class CTestStage : public Engine::CScene
{
private:
	explicit CTestStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestStage();

public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene() override;
	virtual void Render_Scene() override;

private:
	HRESULT			Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_GameItem(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_MapObj(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_Door(const _tchar* pLayerTag);
	HRESULT			Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT			Ready_LightInfo();

private:
	void			Run_Collision_Func();
	void			Door_Collision();
	void			Moster_Collision();
	void			MapObj_Collision();

	void			Drop_ITem();

	void			Insert_Child();

public:
	static CTestStage*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CParticleScatter* pScatter;
	CParticleExplosion* pExp;
	CParticleSplash* pSpl;

private:
	virtual void Free() override;

};

