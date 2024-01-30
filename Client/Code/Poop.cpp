#include "stdafx.h"
#include "Poop.h"

#include "Export_Utility.h"

CPoop::CPoop(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev)
{
}

CPoop::CPoop(const CPoop& rhs)
	: CMapObj(rhs)
{
}

CPoop::~CPoop()
{
}

HRESULT CPoop::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 1.f, 10.f);

	m_iLimitHit = 4;

	return E_NOTIMPL;
}

_int CPoop::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CPoop::LateUpdate_GameObject()
{
}

void CPoop::Render_GameObject()
{
}

HRESULT CPoop::Add_Component()
{
	return E_NOTIMPL;
}

void CPoop::Motion_Change()
{
}

void CPoop::Hit()
{

}

CPoop* CPoop::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

void CPoop::Free()
{
}
