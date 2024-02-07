#include "stdafx.h"
#include "Pacer.h"

#include "Export_System.h"
#include "Export_Utility.h"

CPacer::CPacer(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
	: CMonster(pGraphicDev)
{
	DWORD dwSeed = (iID << 16) | (time(NULL) % 1000);
	srand(dwSeed);
	m_iRandNum = rand() % 180;
}

CPacer::CPacer(const CPacer& rhs)
	: CMonster(rhs)
{
}

CPacer::~CPacer()
{
}

HRESULT CPacer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));

	m_iHp = 3;

	m_fCallLimit = m_iRandNum % 5 + 2;
	m_fSpeed = 4.f;

	m_iPicNum = 10.f;

	m_eMstType = PACER;

	return S_OK;
}

_int CPacer::Update_GameObject(const _float& fTimeDelta)
{
	m_fSlowDelta = Engine::Get_TimeDelta(L"Timer_Second");

	m_fFrame += m_iPicNum * m_fSlowDelta;

	if (m_iPicNum < m_fFrame)
		m_fFrame = 0.f;

	if (m_bHit)
	{
		m_iHp -= 1;

		Hit_PushBack(m_fSlowDelta);

		m_bHit = false;

		if (0 >= m_iHp)
		{
			m_bDead = true;
		}
	}

	Face_Camera();

	if (Check_Time(m_fSlowDelta))
		Change_Dir();

	Move(m_fSlowDelta); // �⺻������ ��� ����

	CGameObject::Update_GameObject(m_fSlowDelta);

	Fix_Y();

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);

	return 0;
}

void CPacer::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPacer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();
}

HRESULT CPacer::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PacerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PacerTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CPacer::Motion_Change()
{
}

void CPacer::Face_Camera()
{
	CTransform* PlayerTransform =
		dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player(ID_DYNAMIC, L"Proto_Transform"));

	_vec3 vAngle = m_pCalculCom->Compute_Vill_Angle(m_pTransformCom, PlayerTransform);
	m_pTransformCom->m_vAngle.y = vAngle.y;
}

void CPacer::Change_Dir()
{
	m_iRandNum = rand() % 180; // (m_iIndex + 1)
	m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(_float(m_iRandNum)));

	m_pTransformCom->Get_Info(INFO_POS, &m_vMoveLook); // ȸ���� Look ���͸� ��� ������ �Ҵ�
	// ���� �и� �پ��ϰ� ���µ� m_vMoveLook ���ʹ� �ű⼭ �ű��ΰɱ�? �� �Դٰ��� �ұ�
}

void CPacer::Move(const _float& fTimeDelta)
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	// ��� ���� Look ���͸� ���
	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (vPos.x < VTXCNTX - m_pTransformCom->m_vScale.x &&
		vPos.z < VTXCNTZ - m_pTransformCom->m_vScale.z &&
		vPos.x > m_pTransformCom->m_vScale.x &&
		vPos.z > m_pTransformCom->m_vScale.z)
	{
		m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed, fTimeDelta);
	}
	else
	{
		m_pTransformCom->Move_Pos(&-m_vMoveLook, m_fSpeed, fTimeDelta);
	}
}

CPacer* CPacer::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iID)
{
	CPacer* pInstance = new CPacer(pGraphicDev, iID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Pacer Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CPacer::Free()
{
	__super::Free();
}
