#include "stdafx.h"
#include "Export_Utility.h"
#include "Player.h"
#include "EpicBullet.h"

CEpicBullet::CEpicBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEpicBullet::CEpicBullet(const CEpicBullet& rhs)
	: CGameObject(rhs)
{
}

CEpicBullet::~CEpicBullet()
{
}

HRESULT CEpicBullet::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fAccTimeDelta = 0;
    m_ePreState = EPIC_END;

	// 지속시간
	m_fCallLimit = 1;
    m_fSpriteSpeed = 2;
    m_vBulletSpeed = _vec3(0, 5, 0);
    m_bStartFall = false;
    m_bEndingSetTarget = false;
    m_bCanAttack = true;

	return S_OK;
}

_int CEpicBullet::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    if (m_eCurState == EPIC_TARGET)
    {
        //m_pCalculatorCom->Compute_Vill_Matrix_X(m_pTransformCom);
    }
    else
    {
        m_pCalculatorCom->Compute_Vill_Matrix(m_pTransformCom);
    }
   
    Motion_Change();

    if (m_eCurState == EPIC_TARGET)
    {
        CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
        //NULL_CHECK_RETURN(pTerrainBufferCom, _vec3());

        CTransform* pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Terrain", L"Proto_Transform"));
        //NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

        _vec3 targetpos = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
        
        m_pTransformCom->Set_Pos(_vec3(targetpos.x, targetpos.y+1, targetpos.z));
        //m_pTransformCom->Rotation(ROT_X, 90);
    }
   
    if (m_eCurState == EPIC_BULLET && m_bStartFall)
    {
        m_vBulletSpeed += _vec3(0, 2, 0);
        m_vShootPos -= m_vBulletSpeed*fTimeDelta;
        m_pTransformCom->Set_Pos(m_vShootPos);

        // 땅에 닿으면
        if (m_vShootPos.y-(m_pTransformCom->m_vScale.y*0.5) <= 0 )
        {
            // 펑 되는 이펙트
            _vec3 vPos;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            Engine::Create_Explosion(m_pGraphicDev, *(m_pTransformCom->Get_WorldMatrix()), 1.2f, 10);


            m_eCurState = EPIC_EFFECT;
            m_fFrame = 0;
            // 터지는 순간 흔적남는걸로 변경
            CPlayer::GetInstance()->Change_LastEpicMark_To_Trace();
        }
    } 

    if (m_eCurState == EPIC_EFFECT)
    {
        m_fFrame += m_iPicNum * fTimeDelta * m_fSpriteSpeed;
        if (m_iPicNum < m_fFrame)
        {
            // 없애기
            m_bDead = true;
        }
    }

    if (m_bDead == true)
    {
        return 1;
    }

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);


    return 0;
}

void CEpicBullet::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CEpicBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    if (m_eCurState == EPIC_EFFECT)
    {
        m_pTextureCom->Set_Texture((_uint)m_fFrame);
    }
    else
    {
        m_pTextureCom->Set_Texture((_uint)0);
    }
    

    if (m_eCurState == EPIC_BULLET && m_bStartFall)
    {
        // BULLET 상태에서는
        // 떨어질때만 보이게
        m_pBufferCom->Render_Buffer();
    }
    else if(m_eCurState != EPIC_BULLET)
    {
        // BULLET 아니면 그냥 계속 RENDER
        m_pBufferCom->Render_Buffer();
    }
    

    /*m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);*/
}

void CEpicBullet::Motion_Change()
{
    if (m_ePreState != m_eCurState)
    {
        m_fFrame = 0.f;
        switch (m_eCurState)
        {
        case EPIC_TARGET:
            m_pTransformCom->m_vScale = { 1.f, 1.8f, 1.f };
            m_pTransformCom->m_vAngle = { D3DXToRadian(95),0,0 };
            m_iPicNum = 1;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicTarget"));
            break;
        case EPIC_BULLET:
            m_pTransformCom->m_vScale = { 2.5f, 2.5f, 2.5f };
            m_pTransformCom->m_vAngle = { 0,0,0 };
            m_iPicNum = 1;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicBullet"));
            break;
        case EPIC_EFFECT:
            m_pTransformCom->m_vScale = { 2.5f, 2.5f, 2.5f };
            m_iPicNum = 11;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicEff"));
            break;
        }

        m_ePreState = m_eCurState;
    }
}

CEpicBullet* CEpicBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag)
{
    CEpicBullet* pInstance = new CEpicBullet(pGraphicDev);
    pInstance->Set_MyLayer(pLayerTag);


    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEpicBullet Create Failed");
        return nullptr;
    }

    return pInstance;
}


void CEpicBullet::Set_Shoot(_vec3 shootpos)
{
    // (다시 클릭했을때 재설정 되지 않도록)
    if (m_bEndingSetTarget == false)
    {
        m_eCurState = EPIC_BULLET;
        m_vShootPos = shootpos + _vec3(0, 11, 0);
        m_pTransformCom->Set_Pos(m_vShootPos);

        // 타겟 높이 고려해서 위치 지정
        Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
        NULL_CHECK(pTerrainBufferCom);

        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

        m_vTargetPos = _vec3(m_vShootPos.x, fHeight, m_vShootPos.z);

        // 타겟 설정 끝남 (다시 클릭했을때 재설정 되지 않도록)
        m_bEndingSetTarget = true;
    }
}

HRESULT CEpicBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicTarget"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicTarget", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicEff"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicEff", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicSpace"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicSpace", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicBullet"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicBullet", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    return S_OK;
}

bool CEpicBullet::Check_Time(const _float& fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    if (m_fAccTimeDelta >= m_fCallLimit)
    {
        m_fAccTimeDelta = 0.f;
        return true;
    }

    return false;
}

void CEpicBullet::Free()
{
    __super::Free();
}
