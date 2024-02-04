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

	// 지속시간
	m_fCallLimit = 1;
	m_fSpriteSpeed = 2;


	return S_OK;
}

_int CEpicBulletMark::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

   
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

	m_pTextureCom->Set_Texture((_uint)0);

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
            m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
            m_iPicNum = 1;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicTarget"));
            break;
        case EPIC_MARK_TRACE:
            m_pTransformCom->m_vScale = { 2.f, 2.f, 2.f };
            m_iPicNum = 1;
            m_fSpriteSpeed = 1.5f;
            m_pTextureCom = dynamic_cast<CTexture*>(m_mapComponent[ID_STATIC].at(L"Proto_BulletTexture_EpicBullet"));
            break;
        }

        m_ePreState = m_eCurState;
    }
}

CEpicBulletMark* CEpicBulletMark::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEpicBulletMark* pInstance = new CEpicBulletMark(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEpicBulletMark Create Failed");
        return nullptr;
    }

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
    m_pTransformCom->Set_Pos(0, 0, 0);
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

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
