#include "stdafx.h"
#include "Heart.h"
#include "Export_Utility.h"
#include "Player.h"

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
    : CItem(pGraphicDev)
{
    DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
    srand(dwSeed);
    m_iRandNum = rand() % 180;
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

    m_bDead = false;
    m_fFrame = 0;
    m_iCoin = 3;

    m_pTransformCom->m_vScale = { 0.5,0.5,0.5 };

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
        if (CPlayer::GetInstance()->Get_Hp() < CPlayer::GetInstance()->Get_MaxHp())
        {   
            // 구매해야할 경우
            if (CPlayer::GetInstance()->Get_Coin() >= m_iCoin)
            {
                CPlayer::GetInstance()->Set_Coin(-m_iCoin);
                m_bDead = true;
                CPlayer::GetInstance()->Set_Hp(1);
                float hp = CPlayer::GetInstance()->Get_Hp();
            }
        }
    }
    else if (m_eCurItemPlace != SP_SLOT && m_eCurItemPlace != SP_OBJECT)
    {
        if (CPlayer::GetInstance()->Get_Hp() < CPlayer::GetInstance()->Get_MaxHp())
        {
            // 그냥 바로 적용
            m_bDead = true;
            CPlayer::GetInstance()->Set_Hp(1);
        }
    }
}

void CHeart::Item_Spawn_Action()
{
    Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
    NULL_CHECK(pTerrainBufferCom);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _float	fHeight = m_pCalculCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

    _vec3 itemPos;
    m_pTransformCom->Get_Info(INFO_POS, &itemPos);

    if (m_eCurItemPlace == SP_SLOT)
    {

        _vec3 temp;
        m_pTransformCom->Get_Info(INFO_POS, &temp);

        if (temp.y <= fHeight + 1)
        {
            m_eCurItemPlace = SP_END;
            m_pTransformCom->Set_Pos(temp.x, fHeight + 1, temp.z);
        }


    }
    else if (m_eCurItemPlace == SP_OBJECT)
    {
        if (m_iTimer < 1)
        {
            //m_fMoveSpeed += 0.01;
            m_pTransformCom->Set_Pos(itemPos.x, itemPos.y + m_fMoveSpeed, itemPos.z);
        }
        else
        {
            _vec3 temp = itemPos - _vec3(0, 0.1, 0);

            if (temp.y <= fHeight + 1)
            {
                m_eCurItemPlace = SP_END;
                m_pTransformCom->Set_Pos(itemPos.x, fHeight + 1, itemPos.z);
            }
            else
            {
                m_pTransformCom->Set_Pos(itemPos.x, itemPos.y - 0.1, itemPos.z);
            }
        }
    }

    m_iTimer++;
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

    return S_OK;
}

void CHeart::Motion_Change()
{
}

CHeart* CHeart::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look, int iID)
{
    CHeart* pInstance = new CHeart(pGraphicDev, iID *3);
    srand((unsigned)time(NULL));

    if (spawnspot == 1)
    {
        _vec3 postemp = pos + _vec3(0, 3.f, 0);
        pInstance->Set_SpawnPos(postemp);

        float speed = rand() % 4 + 2;
        speed *= 0.1;
        pInstance->Set_MoveSpeed(speed);
    }
    else
    {
        pInstance->Set_SpawnPos(pos);
    }

    float fAngle = (float)(rand() % 100 - 50);
    _matrix mat;
    _vec3 templook = look;
    D3DXMatrixRotationY(&mat, fAngle);
    D3DXVec3TransformCoord(&templook, &templook, &mat);
    pInstance->Set_LookDir(templook);

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
