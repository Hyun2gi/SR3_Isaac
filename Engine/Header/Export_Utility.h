#pragma once

#include "Management.h"
#include "ProtoMgr.h"
#include "Renderer.h"
#include "LightMgr.h"
#include "ParticleMgr.h"

#include "AbstractFactory.h"

#include "TriCol.h"
#include "RcCol.h"
#include "RcTex.h"
#include "TerrainTex.h"
#include "CubeTex.h"

#include "Texture.h"

#include "Transform.h"
#include "Camera.h"
#include "Pipeline.h"
#include "Calculator.h"
#include "Shader.h"

#include "UI.h"

#include "ParticleScatter.h"
#include "ParticleExplosion.h"
#include "ParticleDust.h"
#include "ParticleSplash.h"
#include "ParticleSplashLeft.h"
#include "ParticleSplashRight.h"
#include "ParticleSplashForward.h"
#include "ParticleBurst.h"


BEGIN(Engine)

// Management

inline CComponent*		Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);

inline HRESULT			Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagementInstance);

inline HRESULT			Set_Scene(CScene* pScene);
inline _int				Update_Scene(const _float& fTimeDelta);
inline void				LateUpdate_Scene();
inline void				Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

// ProtoMgr
inline HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent);
inline CComponent*		Clone_Proto(const _tchar* pProtoTag);

// Renderer
inline void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject);
inline void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void		Clear_RenderGroup();
inline void		Set_Ending();

// LightMgr
inline HRESULT		Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
								const D3DLIGHT9* pLightInfo,
								const _uint& iIndex);

//ParticleMgr
inline void	Create_Scatter(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vMin = _vec3(-10.0f, -10.0f, -10.0f), _vec3 vMax = _vec3(10.0f, 10.0f, 10.0f), _float fSize = 0.25f, _int iCount = 15);
inline void	Kill_Scatter();
inline void	Create_Explosion(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 1.f, _int iCount = 10, _float fSpeed = 2.f, const _tchar* pTexturePath = L"../Bin/Resource/Texture/Particle/explosion.png");
inline void	Create_Dust(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 1.f, _int iCount = 30);
inline void	Create_Splash(LPDIRECT3DDEVICE9 pDevice, _matrix matWorld, const _tchar* pTextruePath = L"../Bin/Resource/Texture/Particle/BloodExp/BloodExp_%d.png", _int iMaxFrame = 7, _float fSize = 0.25f, _int iCount = 10);
inline void	Create_Splash_Left(LPDIRECT3DDEVICE9 pDevice, _matrix matWorld, const _tchar* pTextruePath = L"../Bin/Resource/Texture/Particle/BloodExp_Left/BloodExp_%d.png", _int iMaxFrame = 2, _float fSize = 0.5f, _int iCount = 15);
inline void	Create_Splash_Right(LPDIRECT3DDEVICE9 pDevice, _matrix matWorld, const _tchar* pTextruePath = L"../Bin/Resource/Texture/Particle/BloodExp_Right/BloodExp_%d.png", _int iMaxFrame = 2, _float fSize = 0.5f, _int iCount = 15);
inline void	Create_Splash_Forward(LPDIRECT3DDEVICE9 pDevice, _matrix matWorld, const _tchar* pTextruePath = L"../Bin/Resource/Texture/Particle/WaterDrop/WaterDrop_%d.png", _int iMaxFrame = 1, _float fSize = 0.25f, _int iCount = 15);
inline void	Create_Burst(LPDIRECT3DDEVICE9 pGraphicDev, _matrix matWorld, _float fSize = 0.5f, _int iCount = 10);

inline void	Update_Particles(_float fDeltaTime);
inline void	Render_Particles();


// Release
inline void				Release_Utility();

#include "Export_Utility.inl"

END