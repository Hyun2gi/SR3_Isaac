#include "stdafx.h"
#include "..\Header\CubeObject.h"

#include "Export_Utility.h"

CCubeObject::CCubeObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), 
	m_bIsArrived(false), m_bIsShaked(false), m_bIsExpansion(false),
	m_fShakingTimer(0.f)
{
}

CCubeObject::CCubeObject(const CCubeObject& rhs)
	: Engine::CGameObject(rhs)
{

}

CCubeObject::~CCubeObject()
{
}

HRESULT CCubeObject::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fS = 1.f;
	m_pTransformCom->m_vScale = { 0.5f, 0.5f, 0.5f };
	m_vDstScale = { 0.5f, 0.5f, 0.5f };
	m_vTempScale = { 0.f, 0.f, 0.f };
	m_vTempAngle = { 0.f, 0.f, 0.f };

	return S_OK;
}

Engine::_int CCubeObject::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bStartScene)
	{
		switch (m_iActionType)
		{
			//랜덤하게 모이기
		case RANDOM_CREATE:
		{
			if (m_fS > 0.86f)
			{
				m_fS = m_fS - (0.03f * fTimeDelta);
				D3DXVec3Lerp(&m_vTempPos, &m_vDstPos, &m_pTransformCom->m_vInfo[INFO_POS], m_fS);
			}
			else
			{
				m_bIsArrived = true;
			}

			m_pTransformCom->m_vInfo[INFO_POS] = m_vTempPos;

			break;
		}
		// 큐브 쉐이킹하면서 커지기
		case SHAKE_CREATE:
		{
			m_pTransformCom->m_vScale = m_vTempScale;

			if (m_fS > 0.94f)
			{
				m_fS = m_fS - (0.03f * fTimeDelta);

				if (m_pTransformCom->m_vScale.x < m_vDstScale.x)
				{
					m_vTempScale.x += 0.005;
					m_vTempScale.y += 0.005;
					m_vTempScale.z += 0.005;
				}

				if (m_fShakingTimer > 0.07f)
				{
					m_bIsShaked = !m_bIsShaked;
					m_fShakingTimer = 0.f;
				}
				else
				{
					m_fShakingTimer += fTimeDelta;
				}

				_float fValue = GetRandomFloat(90.f, 180.f);

				if (m_bIsShaked)
				{
					m_vTempAngle.y += D3DXToRadian(fValue) * fTimeDelta;

				}
				else
				{
					m_vTempAngle.y -= D3DXToRadian(fValue) * fTimeDelta;
				}

			}
			else
			{
				m_vTempScale = { 0.5f, 0.5f, 0.5f };
				m_vTempAngle = { 0.f, 0.f, 0.f };
				m_bIsArrived = true;
			}

			m_pTransformCom->m_vInfo[INFO_POS] = m_vDstPos;
			m_pTransformCom->m_vScale = m_vTempScale;
			m_pTransformCom->m_vAngle = m_vTempAngle;

			break;
		}
		// 큐브 띠용거리기
		case EXPANSION_CREATE:
		{
			m_pTransformCom->m_vScale = m_vTempScale;

			if (m_fS > 0.94f)
			{
				m_fS = m_fS - (0.03f * fTimeDelta);

				if (m_pTransformCom->m_vScale.x < m_vDstScale.x + 0.2f && !m_bIsExpansion)
				{
					m_vTempScale.x += GetRandomFloat(0.02f, 0.04f);
					m_vTempScale.y += GetRandomFloat(0.02f, 0.04f);
					m_vTempScale.z += GetRandomFloat(0.02f, 0.04f);
					//m_vTempScale.x += 0.01;
					//m_vTempScale.y += 0.01;
					//m_vTempScale.z += 0.01;
				}
				else
				{
					m_bIsExpansion = true;
				}

				if (m_pTransformCom->m_vScale.x > m_vDstScale.x && m_bIsExpansion)
				{
					m_vTempScale.x -= GetRandomFloat(0.02f, 0.04f);
					m_vTempScale.y -= GetRandomFloat(0.02f, 0.04f);
					m_vTempScale.z -= GetRandomFloat(0.02f, 0.04f);
					//m_vTempScale.x -= 0.01f;
					//m_vTempScale.y -= 0.01f;
					//m_vTempScale.z -= 0.01f;
				}
				else
				{
					m_bIsExpansion = false;
				}

			}
			else
			{
				m_vTempScale = { 0.5f, 0.5f, 0.5f };
				m_vTempAngle = { 0.f, 0.f, 0.f };
				m_bIsArrived = true;
			}

			m_pTransformCom->m_vInfo[INFO_POS] = m_vDstPos;
			m_pTransformCom->m_vScale = m_vTempScale;
			m_pTransformCom->m_vAngle = m_vTempAngle;

			break;
		}
		//한바퀴 돌면서 커지기
		case TURN_CREATE:
		{
			m_pTransformCom->m_vScale = m_vTempScale;

			if (m_fS > 0.94f)
			{
				m_fS = m_fS - (0.03f * fTimeDelta);

				if (m_pTransformCom->m_vScale.x < m_vDstScale.x)
				{
					m_vTempScale.x += 0.005;
					m_vTempScale.y += 0.005;
					m_vTempScale.z += 0.005;
				}

				_float fValue = GetRandomFloat(90.f, 180.f);
				m_vTempAngle.x -= D3DXToRadian(fValue) * fTimeDelta;
				m_vTempAngle.y += D3DXToRadian(fValue) * fTimeDelta;
				m_vTempAngle.z += D3DXToRadian(fValue) * fTimeDelta;

			}
			else
			{
				m_vTempScale = { 0.5f, 0.5f, 0.5f };
				m_vTempAngle = { 0.f, 0.f, 0.f };
				m_bIsArrived = true;
			}

			m_pTransformCom->m_vInfo[INFO_POS] = m_vDstPos;
			m_pTransformCom->m_vScale = m_vTempScale;
			m_pTransformCom->m_vAngle = m_vTempAngle;
			break;
		}
		}
	}


	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CCubeObject::LateUpdate_GameObject()
{
	Engine::Add_RenderGroup(RENDER_NONALPHA, this);

	__super::LateUpdate_GameObject();
}

void CCubeObject::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

HRESULT CCubeObject::Set_Cute_Texture(const _tchar* pTextureTag)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(pTextureTag));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ pTextureTag, pComponent });
}

void CCubeObject::Set_Dst_Pos(_vec3 vDst)
{
	if(!m_bStartScene)
		m_vDstPos = m_vTempPos = vDst;
	else
	{
		m_pTransformCom->m_vInfo[INFO_POS] = vDst;
		m_vDstPos = vDst;
		m_bIsArrived = true;
	}
}

void CCubeObject::Set_Cube_Action_Type(_int iAction)
{
	m_iActionType = iAction;

	if (m_iActionType == SEQUENTIAL_CREATE)
	{
		m_iActionType = 0;
		m_pTransformCom->m_vInfo[INFO_POS] = {-100, -100, 0};
	}

}

HRESULT CCubeObject::Add_Component()
{
	CComponent*		pComponent = nullptr;
		
	pComponent = m_pBufferCom = dynamic_cast<CCubeTex*>(Engine::Clone_Proto(L"Proto_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CubeTex", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });
	m_pTransformCom->m_vInfo[INFO_POS] = { _float(rand() % 1000) - 500, _float(rand() % 1000) - 500, _float(rand() % 1000) - 500 };
		
	return S_OK;
}

CCubeObject * CCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, bool bStartScene)
{
	CCubeObject *	pInstance = new CCubeObject(pGraphicDev);
	pInstance->m_bStartScene = bStartScene;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CubeObject Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCubeObject::Free()
{	
	__super::Free();
}


