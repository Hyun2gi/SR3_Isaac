#include "stdafx.h"
#include "PlayerBullet.h"

#include "Export_Utility.h"
#include "Player.h"

CPlayerBullet::CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& rhs)
    : CGameObject(rhs)
{
}

CPlayerBullet::~CPlayerBullet()
{
}

HRESULT CPlayerBullet::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };
    m_fAccTimeDelta = 0;
    m_fCallLimit = 1.5;
    m_fBulletSpeed = CPlayer::GetInstance()->Get_BulletSpeed();

    m_bCollision = false;

    m_ePreState = IDLEBULLET_END;
    m_eCurState = IDLEBULLET_IDLE;

    m_fFrame = 0;

    return S_OK;
}

_int CPlayerBullet::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    m_pCalculatorCom->Compute_Vill_Matrix(m_pTransformCom);

    if (m_bCollision)
    {
        m_eCurState = IDLEBULLET_EFFECT;
    }

    Motion_Change();

   
    // 충돌하지 않고 시간이 다되면 바로 DEAD 처리
    if (Check_Time(fTimeDelta) && m_eCurState == IDLEBULLET_IDLE)
    {
        // 시간 다 되면 삭제
        m_bDead = true;
    }

    // 시간다돼서
    if (m_bDead == true)
    {
        return 1;
    }

    if (m_eCurState == IDLEBULLET_EFFECT)
    {
        //마지막 이미지가 나오면 dead
        m_fFrame += m_iPicNum * fTimeDelta * m_fSpriteSpeed;

        if (m_iPicNum < m_fFrame)
        {
            m_fFrame = 0;
            // 없애기
            m_bDead = true;
        }
    }
    else if (m_eCurState == IDLEBULLET_IDLE)
    {
        m_fBulletSpeed += 0.2 * fTimeDelta;

        m_pTransformCom->Move_Pos(&m_vBulletDir, m_fBulletSpeed, fTimeDelta);
    }
    

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

    return 0;
}

void CPlayerBullet::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CPlayerBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    if (m_eCurState == IDLEBULLET_IDLE)
    {
        m_pTextureCom->Set_Texture((_uint)0);
    }
    else
    {
        m_pTextureCom->Set_Texture((_uint)m_fFrame);
    }
   

    m_pBufferCom->Render_Buffer();

    /*m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);*/
}

HRESULT CPlayerBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTear_Effect"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTear_Effect", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTear"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTear", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));


    //플레이어 첫 시작 위치 받아와서 거기서부터 시작
    _vec3   playerPos;
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_LOOK, &m_vBulletDir);

    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_LOOK, &m_vBulletDir);

    m_vBulletDir = _vec3(m_vBulletDir.x, m_vBulletDir.y/3, m_vBulletDir.z);
    D3DXVec3Normalize(&m_vBulletDir, &m_vBulletDir);

    playerPos += m_vBulletDir * 0.2;
    m_pTransformCom->Set_Pos(playerPos);
    
    // 플레이어가 보는 방향으로 bullet이 y축으로 회전해서 눈물이 정면으로 보이게 회전
    _vec3   BulletLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &BulletLook);
    //m_pTransformCom->Rotation(ROT_Y, acos(D3DXVec3Dot(&m_vBulletDir, &BulletLook)));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    return S_OK;
}


bool CPlayerBullet::Check_Time(const _float& fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    if (m_fAccTimeDelta >= m_fCallLimit)
    {
        //m_fAccTimeDelta = 0.f;
        return true;
    }

    return false;
}

void CPlayerBullet::Motion_Change()
{
    if (m_ePreState != m_eCurState)
    {
        m_fFrame = 0.f;
        switch (m_eCurState)
        {
        case IDLEBULLET_IDLE:
            m_iPicNum = 1;
            m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTear"));
            break;
        case IDLEBULLET_EFFECT:
            m_iPicNum = 10;
            m_fSpriteSpeed = 3.f;
            m_fAccTimeDelta = 0;
            m_fFrame = 0;
            m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTear_Effect"));
            break;
        }

        m_ePreState = m_eCurState;
    }
}

CPlayerBullet* CPlayerBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag)
{
    CPlayerBullet* pInstance = new CPlayerBullet(pGraphicDev);
    pInstance->Set_MyLayer(pLayerTag);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("PlayerBullet Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CPlayerBullet::Free()
{
    __super::Free();
}
