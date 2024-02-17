#include "stdafx.h"
#include "BrimStoneBullet.h"

#include "Export_Utility.h"
#include "Player.h"

CBrimStoneBullet::CBrimStoneBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CBrimStoneBullet::CBrimStoneBullet(const CBrimStoneBullet& rhs)
    : CGameObject(rhs)
{
}

CBrimStoneBullet::~CBrimStoneBullet()
{
}

HRESULT CBrimStoneBullet::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_eCurState = BRIM_CENTER;
    m_fAccTimeDelta = 0;

    // 지속시간
    m_fCallLimit = 1.5;
    m_bRotate = false;
    m_iParticleTimer = 0;
    m_pTransformCom->m_vScale = { 0.6f,0.6f,0.6f };

    return S_OK;
}

_int CBrimStoneBullet::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);
    m_pCalculatorCom->Compute_Vill_Matrix(m_pTransformCom);

    m_iParticleTimer++;

    if (m_iBulletIndex == 4)
    {
        if (m_iParticleTimer == 1)
        {
            //D3DXMATRIX _world;
            //_world = *(m_pTransformCom->Get_WorldMatrix());
            //_vec3 move;
            //m_pTransformCom->Get_Info(INFO_LOOK, &move);
            //// _world._41 += move.x * 2;
            //_world._42 = 0.2;
            ////_world._43 += move.z * 2;
            //Engine::Create_Splash_Left(m_pGraphicDev, _world);
            //Engine::Create_Splash_Right(m_pGraphicDev, _world);
        }  
    }


    //플레이어 첫 시작 위치 받아와서 거기서부터 시작
    _vec3   playerPos;
    _vec3   playerDir, playerDirRight;
    _vec3   bulletPos;
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_LOOK, &m_vBulletDir);

    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_LOOK, &playerDir);
    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_LOOK, &playerDirRight);

    m_vBulletDir = _vec3(playerDir.x, 0, playerDir.z);
    D3DXVec3Normalize(&m_vBulletDir, &m_vBulletDir);

    bulletPos = playerPos + ((m_iBulletIndex + 1) * m_pTransformCom->m_vScale.x) * playerDir;
    bulletPos += _vec3(0, 0, 0);
    
    int legstate = CPlayer::GetInstance()->Get_Azazel_Anim();
    switch (legstate)
    {
    case 0:
        //front
        break;
    case 1:
        //front 걷기
        bulletPos += (playerDir * 0.5);
        break;
    case 2:
        //back
        break;
    case 3:
        //left
        bulletPos += (-playerDirRight * 0.5);
        break;
    case 4:
        //right
        bulletPos += (playerDirRight * 0.6);
        break;
    case 5:
        //no show
        break;
    }

    m_pTransformCom->Set_Pos(bulletPos);

   /* if (m_bLie == false)
    {
        m_pTransformCom->Rotation(ROT_X, 100);
        m_pTransformCom->Rotation(ROT_Z, 100);
    }*/
   
    m_bRotate = true;


    if (Check_Time(fTimeDelta))
    {
        // 시간 다 되면 삭제
        m_bDead = true;
    }

    if (m_bDead == true)
    {
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);


    return 0;
}

void CBrimStoneBullet::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CBrimStoneBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pTextureCom->Set_Texture((_uint)0);

    m_pBufferCom->Render_Buffer();

    /*m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);*/

}

CBrimStoneBullet* CBrimStoneBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, int bulletIndex, bool lie)
{
    CBrimStoneBullet* pInstance = new CBrimStoneBullet(pGraphicDev);
    pInstance->Set_MyLayer(pLayerTag);
    pInstance->Set_BulletIndex(bulletIndex);
    pInstance->Set_BulletLie(lie);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("PlayerBullet Create Failed");
        return nullptr;
    }
    return pInstance;


}

HRESULT CBrimStoneBullet::Set_HeadTexture(bool _head, int _index)
{
    m_bhead = _head;
    m_iBulletIndex = _index;

    CComponent* pComponent = nullptr;
    if (m_bhead)
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_BrimHead"));
        NULL_CHECK_RETURN(pComponent, E_FAIL);
        m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_BrimHead", pComponent });
    }
    else
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_BrimCenter"));
        NULL_CHECK_RETURN(pComponent, E_FAIL);
        m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_BrimCenter", pComponent });
    }
}

HRESULT CBrimStoneBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    /*pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_BrimHead"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_BrimHead", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_BrimCenter"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_BrimCenter", pComponent });*/

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_BrimCenter"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_BrimCenter", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);

    _vec3   playerPos;
    _vec3   playerDir;
    _vec3   bulletPos;
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    //dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Player", L"Proto_Transform"))->Get_Info(INFO_LOOK, &m_vBulletDir);

    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_POS, &playerPos);
    dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"))->Get_Info(INFO_LOOK, &playerDir);

    m_vBulletDir = _vec3(playerDir.x, 0, playerDir.z);
    D3DXVec3Normalize(&m_vBulletDir, &m_vBulletDir);

    bulletPos = playerPos + ((m_iBulletIndex + 1) * m_pTransformCom->m_vScale.x*0.5) * m_vBulletDir;
    m_pTransformCom->Set_Pos(bulletPos);

    if (m_bLie == true)
    {
        //m_pTransformCom->Rotation(ROT_X, 90);
        m_pTransformCom->m_vAngle = { D3DXToRadian(90),0,0 };
    }

    if (m_bLie == false)
    {
        m_pTransformCom->m_vScale = { 1.f,1.f,1.f };
    }

    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    return S_OK;
}

bool CBrimStoneBullet::Check_Time(const _float& fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    if (m_fAccTimeDelta >= m_fCallLimit)
    {
        m_fAccTimeDelta = 0.f;
        return true;
    }

    return false;
}

void CBrimStoneBullet::Free()
{
    __super::Free();
}
