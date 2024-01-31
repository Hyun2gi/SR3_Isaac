#include "..\..\Header\GameObject.h"

CGameObject::CGameObject()
{
}

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_fViewZ(0.f)
{
	m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev), m_fViewZ(rhs.m_fViewZ)
{
	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
}


HRESULT CGameObject::Ready_GameObject()
{
	return S_OK;
}

_int CGameObject::Update_GameObject(const _float & fTimeDelta)
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return 0;
}

void CGameObject::LateUpdate_GameObject()
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->LateUpdate_Component();
}

void CGameObject::Render_GameObject()
{
}
void CGameObject::Free()
{
	for (size_t i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}

CComponent * CGameObject::Find_Component(COMPONENTID eID, const _tchar * pComponentTag)
{
	auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent[eID].end())
		return nullptr;
	
	return iter->second;
}
CComponent * CGameObject::Get_Component(COMPONENTID eID, const _tchar * pComponentTag)
{
	CComponent*		pComponent = Find_Component(eID, pComponentTag);

	NULL_CHECK_RETURN(pComponent, nullptr);
	
	return pComponent;
}

void Engine::CGameObject::Compute_ViewZ(const _vec3* pPos)
{
	_matrix		matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));

}

