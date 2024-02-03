#include "stdafx.h"
#include "Epic.h"
#include "Export_Utility.h"
#include "Player.h"

CEpic::CEpic(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CEpic::CEpic(const CEpic& rhs)
    : CItem(rhs)
{
}

CEpic::~CEpic()
{
}

HRESULT CEpic::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_pTransformCom->Set_Pos(10.f, 1.f, 10.f);

    m_bDead = false;
    m_fFrame = 0;
    m_iCoin = 15;

    return S_OK;
}

_int CEpic::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

    if (m_bDead == true)
    {
        // 죽음 처리
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

    return 0;
}

void CEpic::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CEpic::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)0);

    m_pBufferCom->Render_Buffer();

 
}

void CEpic::Run_Item_Effect()
{
    if (m_eCurItemPlace == SP_SHOP)
    {
        // 구매해야할 경우
        if (CPlayer::GetInstance()->Get_Coin() >= m_iCoin)
        {
            CPlayer::GetInstance()->Set_BulletType(3);
            CPlayer::GetInstance()->Set_Coin(-m_iCoin);
            m_bDead = true;
            CPlayer::GetInstance()->Set_Item_Get_Anim();
        }
    }
    
}

void CEpic::Item_Spawn_Action()
{
}

HRESULT CEpic::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_EpicItem"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_EpicItem", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    m_pTransformCom->Set_Pos(m_vSpawnPos);

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CEpic::Motion_Change()
{
}

CEpic* CEpic::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
    CEpic* pInstance = new CEpic(pGraphicDev);
    //정확한 위치 설정
    pInstance->Set_SpawnPos(pos);
    pInstance->Set_LookDir(look);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Epic Create Failed");
        return nullptr;
    }
    pInstance->Set_Item_SpawnSpot(spawnspot);

    return pInstance;
}

void CEpic::Free()
{
    __super::Free();
}
