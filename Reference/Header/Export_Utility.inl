#include "Export_Utility.h"

CComponent * Get_Component(COMPONENTID eID, const _tchar * pLayerTag, const _tchar * pObjTag, const _tchar * pComponentTag)
{
	return CManagement::GetInstance()->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}

HRESULT Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement ** ppManagementInstance)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	*ppManagementInstance = pManagement;

	return S_OK;
}

HRESULT Set_Scene(CScene * pScene)
{
	return CManagement::GetInstance()->Set_Scene(pScene);
}
_int Update_Scene(const _float & fTimeDelta)
{
	return CManagement::GetInstance()->Update_Scene(fTimeDelta);
}

void LateUpdate_Scene()
{
	CManagement::GetInstance()->LateUpdate_Scene();
}

void Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CManagement::GetInstance()->Render_Scene(pGraphicDev);
}

HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent)
{
	return CProtoMgr::GetInstance()->Ready_Proto(pProtoTag, pComponent);
}
CComponent*		Clone_Proto(const _tchar* pProtoTag)
{
	return CProtoMgr::GetInstance()->Clone_Proto(pProtoTag);
}

// Renderer
void		Add_RenderGroup(RENDERID eID, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Add_RenderGroup(eID, pGameObject);
}
void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
}
void		Clear_RenderGroup()
{
	CRenderer::GetInstance()->Clear_RenderGroup();
}

HRESULT		Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
	const D3DLIGHT9* pLightInfo,
	const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

inline void Create_Scatter(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vMin, _vec3 vMax, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Scatter(pGraphicDev, vMin, vMax, fSize, iCount);
}

inline void Create_Explosion(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Explosion(pGraphicDev, vPos, fSize, iCount);
}

inline void Create_Splash(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, const _tchar* pTexturePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Splash(pGraphicDev, vPos, pTexturePath, iMaxFrame, fSize, iCount);
}

inline void Create_Splash_Left(LPDIRECT3DDEVICE9 pDevice, _vec3 vPos, const _tchar* pTextruePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Splash_Left(pDevice, vPos, pTextruePath, iMaxFrame, fSize, iCount);
}

inline void Create_Splash_Right(LPDIRECT3DDEVICE9 pDevice, _vec3 vPos, const _tchar* pTextruePath, _int iMaxFrame, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Splash_Right(pDevice, vPos, pTextruePath, iMaxFrame, fSize, iCount);
}

inline void Create_Burst(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fSize, _int iCount)
{
	CParticleMgr::GetInstance()->Create_Burst(pGraphicDev, vPos, fSize, iCount);
}

inline void Update_Particles(_float fDeltaTime)
{
	CParticleMgr::GetInstance()->Update_Particles(fDeltaTime);
}

inline void Render_Particles()
{
	CParticleMgr::GetInstance()->Render_Particles();
}

void Release_Utility()
{
	CLightMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CProtoMgr::DestroyInstance();
	CManagement::DestroyInstance();
	CParticleMgr::DestroyInstance();
}


