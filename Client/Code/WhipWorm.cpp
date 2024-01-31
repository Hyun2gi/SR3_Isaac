#include "stdafx.h"
#include "WhipWorm.h"
#include "Export_Utility.h"
#include "Player.h"

CWhipWorm::CWhipWorm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CWhipWorm::CWhipWorm(const CWhipWorm& rhs)
    : CItem(rhs)
{
}

CWhipWorm::~CWhipWorm()
{
}

HRESULT CWhipWorm::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_pTransformCom->Set_Pos(12.f, 1.f, 10.f);

    m_bDead = false;
    m_fFrame = 0;

    return S_OK;
}

_int CWhipWorm::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);
    
    if (m_bDead == true)
    {
        // Á×À½ Ã³¸®
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CWhipWorm::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CWhipWorm::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pTextureCom->Set_Texture((_uint)0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWhipWorm::Run_Item_Effect()
{
    m_bDead = true;
    CPlayer::GetInstance()->Set_BulletSpeed(50);
}

HRESULT CWhipWorm::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_Whipworm"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_Whipworm", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CWhipWorm::Motion_Change()
{
   
}

CWhipWorm* CWhipWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWhipWorm* pInstance = new CWhipWorm(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("WhipWorm Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CWhipWorm::Free()
{
    __super::Free();
}
