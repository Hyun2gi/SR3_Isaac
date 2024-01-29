#include "stdafx.h"
#include "Coin.h"

#include "Export_System.h"
#include "Export_Utility.h"

CCoin::CCoin(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CCoin::CCoin(const CCoin& rhs)
	:CItem(rhs)
{
}

CCoin::~CCoin()
{
}

HRESULT CCoin::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 5.f, 10.f);

	m_ePreState = COIN_END;
	m_eCurState = COIN_IDLE;
	m_fFrame = 0;
	m_fSpriteSpeed = 2;
	m_iDelay = 0;

	return S_OK;
}

_int CCoin::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eCurState == COIN_IDLE)
	{
		if (m_iDelay == 0)
		{
			m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;
		}
		else
		{
			m_fFrame = 0.f;
			m_iDelay--;
		}


		if (m_fPicNum < m_fFrame)
		{
			m_fFrame = 0.f;
			m_iDelay = 70;
		}
	}
	else if(m_eCurState == COIN_GET)
	{
		m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;

		if (m_fPicNum < m_fFrame)
		{
			// 없애기

			//임의적용
			m_fFrame = m_fPicNum-1;
		}
	}
	
	Key_Input();

	CGameObject::Update_GameObject(fTimeDelta);

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CCoin::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CCoin::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CCoin::Run_Item_Effect()
{
	m_eCurState = COIN_GET;
	// 첫 이미지부터 시작
	m_fFrame = 0;
}

HRESULT CCoin::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// COIN 그냥상태
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoinTexture_IDLE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CoinTexture_IDLE", pComponent });

	// COIN 먹고 EFFECT
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoinTexture_EFFECT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_CoinTexture_EFFECT", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });
}

void CCoin::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;
		switch (m_eCurState)
		{
		case COIN_IDLE:
			m_fPicNum = 6;
			m_fSpriteSpeed = 2.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Coin", L"Proto_CoinTexture_IDLE"));
			break;
		case COIN_GET:
			m_fPicNum = 5;
			m_fSpriteSpeed = 3.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Coin", L"Proto_CoinTexture_EFFECT"));
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CCoin::Key_Input()
{
	if (Engine::Get_DIKeyState(DIK_U) & 0x80)
	{
		Run_Item_Effect();
	}
}

CCoin* CCoin::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCoin* pInstance = new CCoin(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("COIN Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCoin::Free()
{
	__super::Free();
}
