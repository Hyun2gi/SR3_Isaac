#include "stdafx.h"
#include "Mom.h"

#include "Export_Utility.h"

CMom::CMom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CMom::CMom(const CMom& rhs)
	: CMonster(rhs)
{
}

CMom::~CMom()
{
}

HRESULT CMom::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	//m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));
	m_pTransformCom->Set_Pos(10.f, 1.f, 10.f);

	m_iHp = 645; // 조정 필요 할지도

	m_fCallLimit = 2;
	m_fSpeed = 8.f;

	//m_ePreState = LEAPER_END;

	return S_OK;
}

_int CMom::Update_GameObject(const _float& fTimeDelta)
{
	// eye, skin, hand 가 네 방향 문에서 랜덤하게 나옴
	// 덮어 씌우기 전의 문이 필요? Mom 을 쓰러뜨리면 문도 사라져야 함
	// 아니면 텍스쳐 세 개를 다 갖는 오브젝트를 만들어서 랜덤하게 오브젝트 띄우기
	// 평면인데 손 후려치기는 어떻게?

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CMom::LateUpdate_GameObject()
{
	Motion_Change();

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMom::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMom::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	// 텍스쳐 추가 필요

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CMom::Motion_Change()
{
}

CMom* CMom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMom* pInstance = new CMom(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Mom Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMom::Free()
{
	__super::Free();
}
