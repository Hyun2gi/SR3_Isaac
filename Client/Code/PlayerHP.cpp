#include "stdafx.h"
#include "PlayerHP.h"

#include "Export_Utility.h"

CPlayerHP::CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CUI(pGraphicDev)
{
}

CPlayerHP::CPlayerHP(const CPlayerHP& rhs)
	: Engine::CUI(rhs)
{
}

CPlayerHP::~CPlayerHP()
{
}

HRESULT CPlayerHP::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	__super::Ready_GameObject();
	Compute_ProjectionMatrix();

	return S_OK;
}

_int CPlayerHP::Update_GameObject(const _float& fTimeDelta)
{
	if (m_vecHeartUI.empty())
	{
		Create_HeartUI();
	}

	CUI::Update_GameObject(fTimeDelta);

	Update_UI();

	if (!m_vecHeartUI.empty())
	{
		for (auto& iter : m_vecHeartUI)
			iter->Update_GameObject(fTimeDelta);
	}

	return 0;

}

void CPlayerHP::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (!m_vecHeartUI.empty())
	{
		for (auto& iter : m_vecHeartUI)
			iter->LateUpdate_GameObject();
	}
}

void CPlayerHP::Render_GameObject()
{
	if (!m_vecHeartUI.empty())
	{
		for (auto& iter : m_vecHeartUI)
			iter->Render_GameObject();
	}
}

HRESULT CPlayerHP::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	return S_OK;
}

void CPlayerHP::Create_HeartUI()
{
	for (int i = 0; i < 3; ++i)
	{
		CHeartUI* pHeartUI = CHeartUI::Create(m_pGraphicDev, 30.f, 30.f, (-330.f + (i * 30.f)), 230.f, 1, 3);
		m_vecHeartUI.push_back(pHeartUI);
	}
}

void CPlayerHP::Resetting_UI(_float fUIFrame1, _float fUIFrame2, _float fUIFrame3)
{
	vector<CHeartUI*>::iterator iter = m_vecHeartUI.begin();
	dynamic_cast<CHeartUI*>(*iter)->Resetting_Texture(fUIFrame1);
	++iter;
	dynamic_cast<CHeartUI*>(*iter)->Resetting_Texture(fUIFrame2);
	++iter;
	dynamic_cast<CHeartUI*>(*iter)->Resetting_Texture(fUIFrame3);
}

void CPlayerHP::Update_UI()
{
	float fPlayerHP = CPlayer::GetInstance()->Get_Hp();

	if (0 == fPlayerHP)
		Resetting_UI(0.f, 0.f, 0.f);
	else if(0.5f == fPlayerHP)
		Resetting_UI(1.f, 0.f, 0.f);
	else if (1.f == fPlayerHP)
		Resetting_UI(2.f, 0.f, 0.f);
	else if (1.5f == fPlayerHP)
		Resetting_UI(2.f, 1.f, 0.f);
	else if (2.f == fPlayerHP)
		Resetting_UI(2.f, 2.f, 0.f);
	else if (2.5f == fPlayerHP)
		Resetting_UI(2.f, 2.f, 1.f);
	else if (3.f == fPlayerHP)
		Resetting_UI(2.f, 2.f, 2.f);
}

CPlayerHP* CPlayerHP::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fSizeX, _float fSizeY, _float fPosX, _float fPosY, _int iAnimFrameCount, _int iMaxFrameCount, _float fWinCX, _float fWinCY)
{
	CPlayerHP* pInstance = new CPlayerHP(pGraphicDev);

	pInstance->Set_WindowSize(fWinCX, fWinCY);
	pInstance->Set_Size(fSizeX, fSizeY);
	pInstance->Set_Pos(fPosX, fPosY, fWinCX, fWinCY);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("PlayerHP Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPlayerHP::Free()
{
	__super::Free();

	for_each(m_vecHeartUI.begin(), m_vecHeartUI.end(), CDeleteObj());
}