#include "stdafx.h"
#include "CampFire.h"

#include "Export_System.h"
#include "Export_Utility.h"

CCampFire::CCampFire(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pWood(nullptr), m_pFire(nullptr)
{
}

CCampFire::CCampFire(const CCampFire& rhs)
	: CMapObj(rhs),
	m_pWood(rhs.m_pWood), m_pFire(rhs.m_pFire)
{
}

CCampFire::~CCampFire()
{
}

HRESULT CCampFire::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(5.f, 1.f, 10.f);

	m_iLimitHit = 4;

	m_bCreate = false;

	return S_OK;
}

_int CCampFire::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bCreate)
	{
		Create_Wood();
		Create_Fire();
		m_bCreate = true;
	}

	if (m_pWood != nullptr)
		m_pWood->Update_GameObject(fTimeDelta);

	if (m_pFire != nullptr)
		m_pFire->Update_GameObject(fTimeDelta);

	if (Engine::Get_DIKeyState(DIK_Z) & 0x80)//Engine::Key_Down(DIK_X))
		m_pFire->Set_Hit();

	return 0;
}

void CCampFire::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pWood != nullptr)
		m_pWood->LateUpdate_GameObject();

	if (m_pFire != nullptr)
		m_pFire->LateUpdate_GameObject();
}

void CCampFire::Render_GameObject()
{
	if (m_pWood != nullptr)
		m_pWood->Render_GameObject();

	if (m_pFire != nullptr)
		m_pFire->Render_GameObject();
}

HRESULT CCampFire::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CCampFire::Create_Wood()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pWood = CWood::Create(m_pGraphicDev);
	m_pWood->Set_MyLayer(m_vecMyLayer[0]);
	m_pWood->Get_TransformCom()->Set_Pos(vPos.x, vPos.y, vPos.z);

	if (m_pWood == nullptr)
		return;
}

void CCampFire::Create_Fire()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pFire = CFire::Create(m_pGraphicDev);
	m_pFire->Set_MyLayer(m_vecMyLayer[0]);
	m_pFire->Get_TransformCom()->Set_Pos(vPos.x, vPos.y+0.7f, vPos.z);

	if (m_pFire == nullptr)
		return;
}

CCampFire* CCampFire::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCampFire* pInstance = new CCampFire(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CampFire Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCampFire::Free()
{
	Safe_Release<CWood*>(m_pWood);
	m_pWood = nullptr;

	Safe_Release<CFire*>(m_pFire);
	m_pFire = nullptr;

	__super::Free();
}
