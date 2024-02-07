#include "..\..\Header\Layer.h"
#include "..\..\Header\CollisionMgr.h"

#include "Transform.h"

#include "Export_System.h"
#include "Export_Utility.h"

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

CGameObject* CLayer::Get_GameObject(const _tchar* pObjTag)
{
	auto	iter = find_if(m_mapObject.begin(), m_mapObject.end(), CTag_Finder(pObjTag));

	if (iter == m_mapObject.end())
		return nullptr;

	return iter->second;
}

CGameObject* CLayer::Collision_GameObject(CGameObject* pSrc)
{
	CTransform* pSrcTrans = dynamic_cast<CTransform*>(pSrc->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

	for (auto& iter : m_mapObject)
	{
		if (iter.second == pSrc) continue;

		CGameObject* pDst = iter.second;
		CTransform* pDstTrans = dynamic_cast<CTransform*>(pDst->Get_Component(ID_DYNAMIC, L"Proto_Transform"));

		if (CCollisionMgr::Check_Intersect(pSrcTrans, pDstTrans))
			return pDst;
	}

	return nullptr;
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

	/*for (auto& iter : m_mapObject)
	{
		iResult = iter.second->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}*/

	for (auto& iter = m_mapObject.begin();
		iter != m_mapObject.end(); )
	{
		iResult = (*iter).second->Update_GameObject(fTimeDelta);

		if (1 == iResult)
		{
			//Safe_Delete<CGameObject*>(*iter);
			//CDeleteMap(iter);
			iter->second->Release();
			//Safe_Delete<CGameObject*>(iter->second);
			m_mapObject.erase(iter++);
		}
		else
		{
			++iter;
		}

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
