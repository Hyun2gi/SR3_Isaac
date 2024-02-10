#include "stdafx.h"
#include "BossHP.h"

#include "Export_Utility.h"

CBossHP::CBossHP(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev),
	m_pMonster(nullptr)
{
}

CBossHP::CBossHP(const CBossHP& rhs)
	: Engine::CUI(rhs),
	m_pMonster(rhs.m_pMonster)
{
}

CBossHP::~CBossHP()
{
}

HRESULT CBossHP::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 0;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CBossHP::Update_GameObject(const _float& fTimeDelta)
{
	// Boss의 HP를 받아와서 상태 변경

	m_fCurFrame = 0.f;

	Update_Scale();
	
	CUI::Update_GameObject(fTimeDelta);

	return 0;
}

void CBossHP::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CBossHP::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	if(0 < m_iTargetHP)
		m_pBufferCom->Render_Buffer();
}

HRESULT CBossHP::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// Boss HP Bar
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BossHPBarTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_BossHPBarTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

void CBossHP::Update_Scale()
{
	m_iTargetHP = m_pMonster->Get_HP();

	_float fHpSize = 30 * 6.6f;
	_float fItvX = fHpSize / 30;

	m_pTransformCom->m_vScale.x = m_iTargetHP * 6.6f;

	m_pTransformCom->m_vInfo[INFO_POS].x = -fItvX * (30 - m_iTargetHP) * 0.6f;

}

CBossHP* CBossHP::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CBossHP* pInstance = new CBossHP(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("BossHP Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBossHP::Free()
{
	__super::Free();
}
