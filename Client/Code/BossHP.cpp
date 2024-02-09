#include "stdafx.h"
#include "BossHP.h"

CBossHP::~CBossHP()
{
}

HRESULT CBossHP::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CBossHP::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CBossHP::LateUpdate_GameObject()
{
}

void CBossHP::Render_GameObject()
{
}

HRESULT CBossHP::Add_Component()
{
	return E_NOTIMPL;
}

CBossHP* CBossHP::Create()
{
	return nullptr;
}

void CBossHP::Free()
{
}
