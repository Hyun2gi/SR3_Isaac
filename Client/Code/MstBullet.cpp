#include "stdafx.h"
#include "MstBullet.h"

#include "Export_Utility.h"

CMstBullet::CMstBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CMstBullet::CMstBullet(const CMstBullet& rhs)
	: CGameObject(rhs)
{
}

CMstBullet::~CMstBullet()
{
}

HRESULT CMstBullet::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fAccTimeDelta = 0.f;
	m_fCallLimit = 5.f;

    m_fPower = 1.f;
    m_fAccelTime = 0.f;

	return S_OK;
}

_int CMstBullet::Update_GameObject(const _float& fTimeDelta)
{
    CGameObject::Update_GameObject(fTimeDelta);

    if (Check_Time(fTimeDelta)) // 시간 다 되면 삭제
        m_bDead = true;

    if (m_bDead == true)
        return 1;

    Curve(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CMstBullet::LateUpdate_GameObject()
{
    __super::LateUpdate_GameObject();

    _vec3	vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    __super::Compute_ViewZ(&vPos);
}

void CMstBullet::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pTextureCom->Set_Texture((_uint)7);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CMstBullet* CMstBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMstBullet* pInstance = new CMstBullet(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("MstBullet Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CMstBullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MstTear"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Proto_MstTear", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));

    _vec3   vPos;
    dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Monstro", L"Proto_Transform"))->Get_Info(INFO_POS, &vPos);
    dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Monstro", L"Proto_Transform"))->Get_Info(INFO_LOOK, &m_vBulletDir);

    m_pTransformCom->Set_Pos(vPos);

    _vec3   BulletLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &BulletLook);
    m_pTransformCom->Rotation(ROT_Y, acos(D3DXVec3Dot(&m_vBulletDir, &BulletLook)));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

    return S_OK;
}

bool CMstBullet::Check_Time(const _float& fTimeDelta)
{
    m_fAccTimeDelta += fTimeDelta;

    if (m_fAccTimeDelta >= m_fCallLimit)
    {
        m_fAccTimeDelta = 0.f;
        return true;
    }

    return false;
}

void CMstBullet::Curve(const _float& fTimeDelta)
{
    // 총알이 포물선을 그리면서 떨어지도록 수정
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    float fY = vPos.y + (m_fPower * m_fAccelTime) - (9.f * m_fAccelTime * m_fAccelTime * 0.5f);
    m_fAccelTime += 0.007f;

    m_pTransformCom->Set_Pos(vPos.x, fY, vPos.z);

    m_pTransformCom->Move_Pos(&m_vBulletDir, 1.f, fTimeDelta);
}

void CMstBullet::Free()
{
    __super::Free();
}
