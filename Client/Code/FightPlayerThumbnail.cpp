#include "stdafx.h"
#include "..\Header\FightPlayerThumbnail.h"

#include "Export_Utility.h"

#include "Player.h"

CFightPlayerThumbnail::CFightPlayerThumbnail(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CFightPlayerThumbnail::CFightPlayerThumbnail(const CFightPlayerThumbnail& rhs)
	: Engine::CUI(rhs)
{
}

CFightPlayerThumbnail::~CFightPlayerThumbnail()
{
}

HRESULT CFightPlayerThumbnail::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale.x = m_fSizeX;
	m_pTransformCom->m_vScale.y = m_fSizeY;

	
	m_vSecondDestPos = _vec3(m_fPosX, m_fPosY, 0.98f);
	m_pTransformCom->Set_Pos(_vec3(-500.f, m_fPosY, 0.98f));
	m_vDestPos = _vec3(m_pTransformCom->m_vInfo[INFO_POS].x + 250.f, m_fPosY, 0.98f);

	__super::Ready_GameObject();

	return S_OK;
}

Engine::_int CFightPlayerThumbnail::Update_GameObject(const _float& fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	_vec3 vDir = m_vDestPos - m_pTransformCom->m_vInfo[INFO_POS];
	_vec3 vSecondDir = m_vSecondDestPos - m_pTransformCom->m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vSecondDir, &vSecondDir);

	
	if (vDir.x > 0)
	{
		m_fMoveSpeed += m_fIncreaseSpeed;
		m_pTransformCom->Move_Pos(&vDir, m_fMoveSpeed, fTimeDelta);
	}
	else if(!m_bArrivedFirst)
	{
		m_bArrivedFirst = true;
		m_fMoveSpeed = 100.f;
	}

	if(m_bArrivedFirst && vSecondDir.x > 0)
	{
		m_fMoveSpeed -= m_fIncreaseSpeed * 0.1;

		if (m_fMoveSpeed < 0.f)
		{
			m_fMoveSpeed = 10.f;
		}

		m_pTransformCom->Move_Pos(&vSecondDir, m_fMoveSpeed, fTimeDelta);
	}

	return 0;
}

void CFightPlayerThumbnail::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	//CGameObject::LateUpdate_GameObject();
	__super::LateUpdate_GameObject();
}

void CFightPlayerThumbnail::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture((_int)0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CFightPlayerThumbnail::Add_Component()
{
	CComponent*		pComponent = nullptr;
		
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	if (m_bIsaac)
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzazelThumbnailTexture"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].insert({ L"Proto_AzazelThumbnailTexture", pComponent });
	}
	else
	{
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_IsaacThumbnailTexture"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_STATIC].insert({ L"Proto_IsaacThumbnailTexture", pComponent });
	}

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
		
	return S_OK;
}

CFightPlayerThumbnail * CFightPlayerThumbnail::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
	_float fSizeX, _float fSizeY, 
	_float fPosX, _float fPosY, 
	_float fWinCX, _float fWinCY)
{
	CFightPlayerThumbnail *	pInstance = new CFightPlayerThumbnail(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);

	pInstance->m_bIsaac = CPlayer::GetInstance()->Get_AzazelMode();

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("EndingHW Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFightPlayerThumbnail::Free()
{	
	__super::Free();
}
