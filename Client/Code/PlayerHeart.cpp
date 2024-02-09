#include "stdafx.h"
#include "PlayerHeart.h"

#include "Export_Utility.h"

CPlayerHeart::~CPlayerHeart()
{
}

HRESULT CPlayerHeart::Ready_GameObject()
{
	return S_OK;
}

_int CPlayerHeart::Update_GameObject(const _float& fTimeDelta)
{
	return 0;
}

void CPlayerHeart::LateUpdate_GameObject()
{
}

void CPlayerHeart::Render_GameObject()
{
}

HRESULT CPlayerHeart::Add_Component()
{
	return S_OK;
}
