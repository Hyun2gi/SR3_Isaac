#include "stdafx.h"
#include "Heart.h"
#include "Export_Utility.h"
#include "Player.h"

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CHeart::CHeart(const CHeart& rhs)
    : CItem(rhs)
{
}

CHeart::~CHeart()
{
}

HRESULT CHeart::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_pTransformCom->Set_Pos(3.f, 2.f, 3.f);

    m_bDead = false;
    m_fFrame = 0;
    m_iCoin = 3;

    return S_OK;
}

_int CHeart::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

    Item_Spawn_Action();

    Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
    NULL_CHECK(pTerrainBufferCom);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _float	fHeight = m_pCalculCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

    m_pTransformCom->Set_Pos(VTXCNTX / 2, fHeight + 1, VTXCNTZ / 2);

    if (m_eCurItemPlace == SP_SLOT)
    {
        // 크게 튀어나옴
        if (m_iTimer < 40)
        {

        }
        else if(m_iTimer == 40)
        {

        }
    }
    else if (m_eCurItemPlace == SP_OBJECT)
    {
        // 조금 튀어오름
        if (m_iTimer < 40)
        {

        }
        else if(m_iTimer == 40)
        {

        }
    }
    

    if (m_bDead == true)
    {
        // 죽음 처리
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

    return 0;
}

void CHeart::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CHeart::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)0);

    m_pBufferCom->Render_Buffer();

}

void CHeart::Run_Item_Effect()
{
    if (m_eCurItemPlace == SP_SHOP)
    {
        // 구매해야할 경우
        if (CPlayer::GetInstance()->Get_Coin() >= m_iCoin)
        {
            CPlayer::GetInstance()->Set_Coin(-m_iCoin);
            m_bDead = true;
            CPlayer::GetInstance()->Set_Hp(1);
        }
    }
    else
    {
        // 그냥 바로 적용
        m_bDead = true;
        CPlayer::GetInstance()->Set_Hp(1);
    }
}

void CHeart::Item_Spawn_Action()
{
}

HRESULT CHeart::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_Heart"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_Heart", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    m_pTransformCom->Set_Pos(m_vSpawnPos);

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CHeart::Motion_Change()
{
}

CHeart* CHeart::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos)
{
    CHeart* pInstance = new CHeart(pGraphicDev);
    //정확한 위치 설정
    pInstance->Set_SpawnPos(pos);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Heart Create Failed");
        return nullptr;
    }
    pInstance->Set_Item_SpawnSpot(spawnspot);

    return pInstance;
}

void CHeart::Free()
{
    __super::Free();
}
