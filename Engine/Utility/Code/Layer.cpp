#include "..\..\Header\Layer.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent * CLayer::Get_Component(COMPONENTID eID, const _tchar * pObjTag, const _tchar * pComponentTag)
{
	auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return nullptr;
	
	return iter->second->Get_Component(eID, pComponentTag);
}

HRESULT CLayer::Add_GameObject(const _tchar * pObjTag, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mapObject.insert({ pObjTag, pGameObject });

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float & fTimeDelta)
{
	int		iResult = 0;

	for (auto& iter : m_mapObject)
	{
		iResult = iter.second->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}
	
	return iResult;
}

void CLayer::LateUpdate_Layer()
{
	for (auto& iter : m_mapObject)
		iter.second->LateUpdate_GameObject();
}


CLayer * CLayer::Create()
{
	CLayer *		pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);

		MSG_BOX("Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for_each(m_mapObject.begin(), m_mapObject.end(), CDeleteMap());
	m_mapObject.clear();
}
