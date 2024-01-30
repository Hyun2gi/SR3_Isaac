#include "stdafx.h"
#include "MomParts.h"

#include "Export_Utility.h"

CMomParts::CMomParts(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
	: CMonster(pGraphicDev)
{
	m_iIndex = iIndex;
}

CMomParts::CMomParts(const CMomParts& rhs)
	: CMonster(rhs)
{
}

CMomParts::~CMomParts()
{
}

HRESULT CMomParts::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->m_vScale = { 10.f, 20.f, 0.f };

	m_iRandNum = 0;

	m_fCallLimit = 3.f;
	m_fSpeed = 10.f;

	m_ePreState = MOM_END;

	return S_OK;
}

_int CMomParts::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
	{
		if (MOM_HAND == m_eCurState)
		{
			//m_eCurState = MOM_DOOR;
			m_fFrame = 2.9f;
		}
		else
			m_fFrame = 0.f;

	}

	Set_RandNum();

	if (Check_Time(fTimeDelta))
	{
		m_fCallLimit = (_float)m_iRandNum;
		// 랜덤하게 바꾸기
		Change_State();
	}

	CGameObject::Update_GameObject(fTimeDelta);

	// 각 방향 문마다 랜덤한 쿨타임으로 상태 변화 -> 매번 달라야함
	// Look 벡터가 애초에 이상해서 밑 함수 활용 결과도 이상한가 싶은
	//m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CMomParts::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMomParts::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMomParts::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// DOOR
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomDoorTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomDoorTexture", pComponent });

	// EYE
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomEyeTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomEyeTexture", pComponent });

	// SKIN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomSkinTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomSkinTexture", pComponent });

	// HAND
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MomHandTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MomHandTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMomParts::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CMomParts::MOM_DOOR:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"MomParts", L"Proto_MomDoorTexture"));
			break;

		case CMomParts::MOM_EYE:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"MomParts", L"Proto_MomEyeTexture"));
			break;

		case CMomParts::MOM_SKIN:
			m_iPicNum = 2;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"MomParts", L"Proto_MomSkinTexture"));
			break;

		case CMomParts::MOM_HAND:
			m_iPicNum = 3;
			m_fFrameSpeed = 1.5f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"MomParts", L"Proto_MomHandTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CMomParts::Set_RandNum()
{
	DWORD dwSeed = (m_iIndex << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 10;
}

void CMomParts::Change_State()
{
	if (0 == (m_iRandNum % 4))
		m_eCurState = MOM_EYE;
	else if (1 == (m_iRandNum % 4))
		m_eCurState = MOM_SKIN;
	else if (2 == (m_iRandNum % 4))
		m_eCurState = MOM_HAND;
	else if (3 == (m_iRandNum % 4))
		m_eCurState = MOM_DOOR;
}

void CMomParts::Setting_Value()
{
	switch (m_iIndex)
	{
	case 0: // 상
		m_pTransformCom->Set_Pos(20.f, 4.f, 30.f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(0.f));
		break;
	case 1: // 우
		m_pTransformCom->Set_Pos(40.f, 4.f, 15.f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f));
		break;
	case 2: // 하
		m_pTransformCom->Set_Pos(20.f, 4.f, 0.f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(180.f));
		break;
	case 3: // 좌
		m_pTransformCom->Set_Pos(0.f, 4.f, 15.f);
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(270.f));
		break;
	}
}

CMomParts* CMomParts::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
	CMomParts* pInstance = new CMomParts(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Mom's Parts Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMomParts::Free()
{
	__super::Free();
}
