#include "stdafx.h"
#include "HeartHalf.h"
#include "Export_Utility.h"
#include "Player.h"

CHeartHalf::CHeartHalf(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CHeartHalf::CHeartHalf(const CHeartHalf& rhs)
    : CItem(rhs)
{
}

CHeartHalf::~CHeartHalf()
{
}

HRESULT CHeartHalf::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_pTransformCom->Set_Pos(8.f, 1.f, 10.f);

    m_bDead = false;
    m_fFrame = 0;
    m_iCoin = 2;

    return S_OK;
}

_int CHeartHalf::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

    if (m_bDead == true)
    {
        // ���� ó��
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

    return 0;
}

void CHeartHalf::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CHeartHalf::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)0);

    m_pBufferCom->Render_Buffer();
}

void CHeartHalf::Run_Item_Effect()
{
    if (m_eCurItemPlace == SP_SHOP)
    {
        // �����ؾ��� ���
        if (CPlayer::GetInstance()->Get_Coin() >= m_iCoin)
        {
            CPlayer::GetInstance()->Set_Coin(-m_iCoin);
            m_bDead = true;
            CPlayer::GetInstance()->Set_Hp(0.5);
        }
    }
    else 
    {
        // �׳� �ٷ� ����
        m_bDead = true;
        CPlayer::GetInstance()->Set_Hp(0.5);
    }
}

void CHeartHalf::Item_Spawn_Action()
{
}

HRESULT CHeartHalf::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTexture_HeartHalf"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_ItemTexture_HeartHalf", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    m_pTransformCom->Set_Pos(m_vSpawnPos);

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CHeartHalf::Motion_Change()
{
}

CHeartHalf* CHeartHalf::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
    CHeartHalf* pInstance = new CHeartHalf(pGraphicDev);
    //��Ȯ�� ��ġ ����
    pInstance->Set_SpawnPos(pos);
    pInstance->Set_LookDir(look);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("HeartHalf Create Failed");
        return nullptr;
    }
    pInstance->Set_Item_SpawnSpot(spawnspot);

    return pInstance;
}

void CHeartHalf::Free()
{
    __super::Free();
}
