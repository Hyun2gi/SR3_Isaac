#include "stdafx.h"
#include "PLCoinFont.h"

#include "Export_Utility.h"

#include "Player.h"

CPLCoinFont::CPLCoinFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CPLCoinFont::CPLCoinFont(const CPLCoinFont& rhs)
	: Engine::CUI(rhs)
{
}

CPLCoinFont::~CPLCoinFont()
{
}

HRESULT CPLCoinFont::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStartFrame = 0;

	m_fCurFrame = 0.f;

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	m_pTransformCom->Set_Pos(_vec3(m_fPosX, m_fPosY, 0.0f));

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CPLCoinFont::Update_GameObject(const _float& fTimeDelta)
{


	CUI::Update_GameObject(fTimeDelta);

	Resetting_Texture();

	return 0;
}

void CPLCoinFont::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CPLCoinFont::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)m_fCurFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CPLCoinFont::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// Boss HP Bar
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerCoinFontTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerCoinFontTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

void CPLCoinFont::Resetting_Texture()
{
	int iCoin = CPlayer::GetInstance()->Get_Coin();

	if (0 == m_iIndex) // 십의 자리
	{
		iCoin /= 10;
		m_fCurFrame = (_float)(iCoin % 10);
	}
	else if (1 == m_iIndex) // 일의 자리
	{
		m_fCurFrame = (_float)(iCoin % 10);
	}
}

CPLCoinFont* CPLCoinFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CPLCoinFont* pInstance = new CPLCoinFont(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);
	pInstance->m_iAnimFrameCount = iAnimFrameCount;
	pInstance->m_iMaxFrameCount = iMaxFrameCount;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("PLCoinFont Create Failed");
		return nullptr;
	}

	return pInstance;

}

void CPLCoinFont::Free()
{
	__super::Free();
}
