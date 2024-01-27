#include "stdafx.h"
#include "..\Header\Bullet.h"

#include "Export_Utility.h"

CPBullet::CPBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CPBullet::CPBullet(const CPBullet& rhs)
	: Engine::CGameObject(rhs)
{
}

CPBullet::~CPBullet()
{
}

HRESULT CPBullet::Ready_GameObject()
{
	return S_OK;
}

_int CPBullet::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CPBullet::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();
}

void CPBullet::Render_GameObject()
{

}

HRESULT CPBullet::Add_Component()
{
	return S_OK;
}

bool CPBullet::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

CPBullet* CPBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPBullet* pInstance = new CPBullet(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Bullet Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPBullet::Free()
{
	__super::Free();
}
