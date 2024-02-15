#include "stdafx.h"
#include "Epic.h"
#include "Export_Utility.h"
#include "Export_System.h"
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

    m_bDead = false;
    m_fFrame = 0;
    m_iCoin = 15;
    m_iUpTimer = 0;
    m_pTransformCom->m_vScale = { 0.7,0.7,0.7 };

    m_fTop = 1.3f;
    m_fFloor = 0.7f;
    m_fBig = 0.7;
    m_fSmall = 0.5;

    m_iSubTimer = 0;
    m_iSubSizeTimer = 0;

    m_eItemType = EPIC;

    return S_OK;
}

_int CEpic::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

    if (m_bDead == false)
    {
        _vec3 goalpos, curpos, movevec;
        _vec3 goalsize, cursize, sizevec;
        m_pTransformCom->Get_Info(INFO_POS, &curpos);
        cursize = m_pTransformCom->m_vScale;
        if (m_iSubTimer % 2 == 0)
        {
            // 困率
            goalpos = curpos - _vec3(0, curpos.y, 0) + _vec3(0, m_fTop, 0);
        }
        else
        {
            // 关率
            goalpos = curpos - _vec3(0, curpos.y, 0) + _vec3(0, m_fFloor, 0);
        }

        if (m_iSubSizeTimer % 2 == 0)
        {
            goalsize = _vec3(1, 1, 1) * m_fBig;
        }
        else
        {
            goalsize = _vec3(1, 1, 1) * m_fSmall;
        }

        if (abs(goalpos.y - curpos.y) < 0.2f)
        {
            m_iSubTimer++;
        }

        if (abs(goalsize.x - cursize.x) < 0.09f)
        {
            m_iSubSizeTimer++;
        }

        D3DXVec3Lerp(&movevec, &curpos, &goalpos, fTimeDelta*1.3f);
        m_pTransformCom->Set_Pos(movevec);


        D3DXVec3Lerp(&sizevec, &cursize, &goalsize, fTimeDelta * 0.7);
        m_pTransformCom->m_vScale = sizevec;
    }

    if (m_bDead == true)
    {
        m_iUpTimer++;
        CTransform* playerInfo = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

        _vec3		playerPos;

        playerInfo->Get_Info(INFO_POS, &playerPos);

        m_pTransformCom->Set_Pos(playerPos.x, 2.4, playerPos.z);

        m_bDead = true;
    }

    if (m_bDead == true && m_iUpTimer > 135)
    {
        // 磷澜 贸府
        return 1;
    }

    Item_Spawn_Action();

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

bool CEpic::Run_Item_Effect()
{
    CPlayer::GetInstance()->Set_BulletType(3);
    m_bDead = true;
    CPlayer::GetInstance()->Set_Item_Get_Anim();
    Engine::PlayEffect(L"Item_Good.wav", SOUND_EFFECT_ITEM_STOPSUDDEN, 1.f);

    return true;
}

void CEpic::Item_Spawn_Action()
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

        m_pTransformCom->Set_Pos(itemPos.x + m_vLookVec.x * 0.2, itemPos.y - 0.3, itemPos.z + m_vLookVec.z * 0.2);

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

    return S_OK;
}

void CEpic::Motion_Change()
{
}

CEpic* CEpic::Create(LPDIRECT3DDEVICE9 pGraphicDev, int spawnspot, _vec3 pos, _vec3 look)
{
    CEpic* pInstance = new CEpic(pGraphicDev);
    srand((unsigned)time(NULL));

    if (spawnspot == 1)
    {
        _vec3 postemp = pos + _vec3(0, 3.f, 0);
        pInstance->Set_SpawnPos(postemp);

        float speed = rand() % 4 + 2;
        speed *= 0.1;
        pInstance->Set_MoveSpeed(speed);

        float fAngle = (float)(rand() % 100 - 50);
        _matrix mat;
        _vec3 templook = look;
        D3DXMatrixRotationY(&mat, fAngle);
        D3DXVec3TransformCoord(&templook, &templook, &mat);
        pInstance->Set_LookDir(templook);
    }
    else
    {
        pInstance->Set_SpawnPos(pos);
    }

    

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
