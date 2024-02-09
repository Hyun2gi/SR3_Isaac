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
	return S_OK;
}

_int CPlayerHP::Update_GameObject(const _float& fTimeDelta)
{
	return 0;
}

void CPlayerHP::LateUpdate_GameObject()
{
	// Player의 최대 피만큼 객체 생성해두고 피가 까이면 그리지 않는 ?

}

void CPlayerHP::Render_GameObject()
{
}

HRESULT CPlayerHP::Add_Component()
{
	return E_NOTIMPL;
}
