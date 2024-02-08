#include "stdafx.h"
#include "Export_Utility.h"
#include "EpicBulletMark.h"

CEpicBulletMark::CEpicBulletMark(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CEpicBulletMark::CEpicBulletMark(const CEpicBulletMark& rhs)
	: CGameObject(rhs)
{
}

CEpicBulletMark::~CEpicBulletMark()
{
}

HRESULT CEpicBulletMark::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fAccTimeDelta = 0;
	m_ePreState = EPIC_MARK_END;
    m_eCurState = EPIC_MARK_TARGET;

	// 지속시간
	m_fCallLimit = 1;
	m_fSpriteSpeed = 2;
    m_iPicNum = 0;
    m_fFrame = 0;
    m_bPicStop = false;

	return S_OK;
}

_int CEpicBulletMark::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    if (m_eCurState == EPIC_MARK_TRACE && m_bPicStop == false)
    {
        m_fFrame += m_iPicNum * m_fSpriteSpeed;

        // 한번 돌고 멈추기
        if (m_iPicNum < m_fFrame)
        {
            m_fFrame = m_iPicNum-1;
            m_bPicStop = true;
        }   
    }
    else
    {
        m_fFrame = 0;
    }
   
    Motion_Change();

    Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);


    return 0;
}

void CEpicBulletMark::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CEpicBulletMark::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

void CEpicBulletMark::Motion_Change()
{
    if (m_ePreState != m_eCurState)
    {
        m_fFrame = 0.f;
        switch (m_eCurState)
        {
        case EPIC_MARK_TARGET:
            m_pTransformCom->m_vScale = { 1.f, 1.8f, 1.f };
            m_iPicNum = 1;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicTarget"));
            break;
        case EPIC_MARK_TRACE:
            m_pTransformCom->m_vScale = { 1.3f, 1.3f, 1.3f };
            m_iPicNum = 8;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicSpace"));
            break;
        }

        m_ePreState = m_eCurState;
    }
}

CEpicBulletMark* CEpicBulletMark::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, _vec3 pos)
{
    CEpicBulletMark* pInstance = new CEpicBulletMark(pGraphicDev);
    pInstance->Set_MyLayer(pLayerTag);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEpicBulletMark Create Failed");
        return nullptr;
    }

    pos = _vec3(pos.x, 0.1, pos.z);
    pInstance->Set_Pos(pos);

    return pInstance;
}

HRESULT CEpicBulletMark::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicTarget"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicTarget", pComponent });

    pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BulletTexture_EpicSpace"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_BulletTexture_EpicSpace", pComponent });


    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    m_pTransformCom->m_vAngle = { D3DXToRadian(95),0,0 };

    return S_OK;
}

bool CEpicBulletMark::Check_Time(const _float& fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    if (m_fAccTimeDelta >= m_fCallLimit)
    {
        m_fAccTimeDelta = 0.f;
        return true;
    }

    return false;
}

void CEpicBulletMark::Free()
{
    __super::Free();
}
