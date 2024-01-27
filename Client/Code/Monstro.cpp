#include "stdafx.h"
#include "Monstro.h"

#include "Export_Utility.h"

#include "MstBullet.h"

CMonstro::CMonstro(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CMonstro::CMonstro(const CMonstro& rhs)
	: CMonster(rhs)
{
}

CMonstro::~CMonstro()
{
}

HRESULT CMonstro::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(10.f, 3.2f, 10.f);
	m_pTransformCom->m_vScale = { 5.f, 5.f, 0.f };

	m_iHp = 5;

	m_fCallLimit = 5.f;
	m_fSpeed = 10.f;

	m_fPower = 2.f;
	m_fAccelTime = 0.f;

	m_bUp = false;
	m_bBullet = false; 

	m_eState = MONSTRO_IDLE;

	return S_OK;
}

_int CMonstro::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += 3.f * fTimeDelta * 0.4f;

	if (3.f < m_fFrame)
		m_fFrame = 0.f;

	// Bullet Update
	if (!m_BulletList.empty())
	{
		int iResult = 0;
		for (auto& iter = m_BulletList.begin();
			iter != m_BulletList.end();)
		{
			iResult = dynamic_cast<CMstBullet*>(*iter)->Update_GameObject(fTimeDelta);

			if (1 == iResult)
			{
				Safe_Release<CGameObject*>(*iter);
				iter = m_BulletList.erase(iter);
			}
			else
				++iter;
		}
	}

	CGameObject::Update_GameObject(fTimeDelta);

	if (MONSTRO_IDLE == m_eState) // 기본 상태일 때
	{
		if (Check_Time(fTimeDelta)) // 일정 시간마다 기믹3 - 큰 점프 발동
		{
			int iRandBum;

			DWORD dwSeed = (time(NULL) % 1000);
			srand(dwSeed);
			iRandBum = rand() % 2;

			if (1 == iRandBum)
			{
				m_bUp = true;
				m_eState = MONSTRO_JUMP;
			}
			else
			{
				m_bBullet = true;
				m_eState = MONSTRO_ATTACK;
			}
			
			Check_TargetPos();
		}
		else
		{
			if (Check_Time(fTimeDelta, 1.f)) // 일정 시간마다 기믹 1 - 작은 점프 발동
			{
				m_eState = MONSTRO_MOVE;
				Check_TargetPos();
			}
		}
	}
	else if (MONSTRO_MOVE == m_eState)
		MoveTo_Player(fTimeDelta);
	else if (MONSTRO_JUMP == m_eState)
		JumpTo_Player(fTimeDelta);

	if (m_bBullet)
		AttackTo_Player();

	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CMonstro::LateUpdate_GameObject()
{
	// Bullet LateUpdate
	if (!m_BulletList.empty())
	{
		for (auto& iter : m_BulletList)
			dynamic_cast<CMstBullet*>(iter)->LateUpdate_GameObject();
	}

	__super::LateUpdate_GameObject();

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CMonstro::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonstro::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMonstro::MoveTo_Player(const _float& fTimeDelta)
{
	_vec3 vPos, vDir;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fY = vPos.y + (m_fPower * m_fAccelTime) - (9.f * m_fAccelTime * m_fAccelTime * 0.5f);
	m_fAccelTime += 0.02f;

	if (fY < 3.0f)
	{
		m_fAccelTime = 0.f;
		fY = 3.2f;
		m_eState = MONSTRO_IDLE;
	}

	m_pTransformCom->Set_Pos(vPos.x, fY, vPos.z);

	vDir = m_vTargetPos - vPos;

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);

}

void CMonstro::JumpTo_Player(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if (m_bUp)
	{
		if (vPos.y >= 10.f)
		{
			m_bUp = false;
			vPos = m_vTargetPos;
			vPos.y = 80.f;
		}
		else
		{
			vPos.y += 0.9f;
		}
	}
	else
	{
		if (vPos.y > 3.2f)
			vPos.y -= 0.7f;
		else
		{
			vPos.y = 3.2f;
			m_eState = MONSTRO_IDLE;
			m_bBullet = true;
			Check_TargetPos(); // 이걸로 될지?
		}
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CMonstro::AttackTo_Player()
{
	_vec3 vPos, vDir, vRandPos;

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	vRandPos = m_vTargetPos; // 플레이어의 원래 위치 벡터

	for (int i = 0; i < 20; ++i)
	{
		DWORD dwSeed = (i << 16) | (time(NULL) % 1000);
		srand(dwSeed);

		if (0 == (rand() % 2))
		{
			vRandPos.x += float(rand() % 3);
			vRandPos.z += float(rand() % 3);
		}
		else
		{
			vRandPos.x -= float(rand() % 3);
			vRandPos.z -= float(rand() % 3);
		}

		vRandPos.y += float(rand() % 2) - 0.4f;

		vDir = vRandPos - vPos;

		m_BulletList.push_back(CMstBullet::Create(m_pGraphicDev));
		dynamic_cast<CMstBullet*>(m_BulletList.back())->Set_Dir(vDir);
	}

	m_eState = MONSTRO_IDLE;
	m_bBullet = false;
}

void CMonstro::Check_TargetPos()
{
	m_pTargetTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Proto_Transform"));

	m_pTargetTransCom->Get_Info(INFO_POS, &m_vTargetPos);

}

CMonster* CMonstro::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonstro* pInstance = new CMonstro(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Monstro Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMonstro::Free()
{
	__super::Free();
}
