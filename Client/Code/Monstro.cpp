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

	m_fPower = 2.7f;
	m_fAccelTime = 0.f;

	m_bJump = false;
	m_bBullet = false; 

	m_eCurState = MONSTRO_END;

	return S_OK;
}

_int CMonstro::Update_GameObject(const _float& fTimeDelta)
{
	m_fFrame += m_iPicNum * fTimeDelta * m_fFrameSpeed;

	if (m_iPicNum < m_fFrame)
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

	if (MONSTRO_IDLE == m_eCurState || MONSTRO_END == m_eCurState) // 기본 상태일 때
	{
		if (Check_Time(fTimeDelta)) // 일정 시간마다 기믹3 - 큰 점프 발동
		{
			int iRandBum;

			DWORD dwSeed = (time(NULL) % 1000);
			srand(dwSeed);
			iRandBum = rand() % 2;

			if (1 == iRandBum)
			{
				m_bJump = true;
				m_eCurState = MONSTRO_UP;
			}
			else
			{
				m_bBullet = true;
				m_eCurState = MONSTRO_ATTACK;
			}
			Check_TargetPos();
		}
		else
		{
			if (Check_Time(fTimeDelta, 1.f)) // 일정 시간마다 기믹 1 - 작은 점프 발동
			{
				m_eCurState = MONSTRO_MOVE;
				Check_TargetPos();
			}
		}
	}
	else if (MONSTRO_MOVE == m_eCurState)
		MoveTo_Player(fTimeDelta);
	else if (MONSTRO_ATTACK == m_eCurState)
	{
		if (m_bBullet)
		{
			AttackTo_Player();
			m_bBullet = false;
		}

		if (Check_Time(fTimeDelta, 2.5f))
		{
			m_eCurState = MONSTRO_IDLE; // 이 부분 수정 필요할지도
			m_bBullet = false;
		}
	}

	if(m_bJump)
		JumpTo_Player(fTimeDelta);


	m_pCalculCom->Compute_Vill_Matrix(m_pTransformCom);

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CMonstro::LateUpdate_GameObject()
{
	Motion_Change();

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

#pragma region Texture

	// IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroTexture", pComponent });

	// ATTACL
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroAttackTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroAttackTexture", pComponent });

	// JUMP
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroJumpTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroJumpTexture", pComponent });

	// UP
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroUpTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroUpTexture", pComponent });

	// DOWN
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MonstroDownTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_MonstroDownTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;
}

void CMonstro::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case CMonstro::MONSTRO_IDLE:
			m_iPicNum = 3;
			m_fFrameSpeed = 0.3f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Monstro", L"Proto_MonstroTexture"));
			break;

		case CMonstro::MONSTRO_ATTACK:
			m_iPicNum = 1;
			m_fFrameSpeed = 1.f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Monstro", L"Proto_MonstroAttackTexture"));
			break;

		case CMonstro::MONSTRO_MOVE:
			m_iPicNum = 5;
			m_fFrameSpeed = 1.2f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Monstro", L"Proto_MonstroJumpTexture"));
			break;

		case CMonstro::MONSTRO_UP:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Monstro", L"Proto_MonstroUpTexture"));
			break;

		case CMonstro::MONSTRO_DOWN:
			m_iPicNum = 2;
			m_fFrameSpeed = 0.1f;
			m_pTextureCom = dynamic_cast<CTexture*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Monstro", L"Proto_MonstroDownTexture"));
			break;
		}
		m_ePreState = m_eCurState;
	}
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
		m_eCurState = MONSTRO_IDLE;
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

	if (MONSTRO_UP == m_eCurState)
	{
		if (vPos.y >= 10.f)
		{
			m_eCurState = MONSTRO_DOWN;
			vPos = m_vTargetPos;
			vPos.y = 80.f;
		}
		else
		{
			vPos.y += 0.9f;
		}
	}
	else if(MONSTRO_DOWN == m_eCurState)
	{
		if (vPos.y > 3.2f)
		{
			vPos.y -= 0.7f;
		}
		else
		{
			vPos.y = 3.2f;
			m_bJump = false;
			m_bBullet = true;
			m_eCurState = MONSTRO_ATTACK; 
			Check_TargetPos(); // 이걸로 될지?
		}
	}
	m_pTransformCom->Set_Pos(vPos);
}

void CMonstro::AttackTo_Player()
{
	_vec3 vPos, vDir, vRandPos;

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	vRandPos = m_vTargetPos;

	for (int i = 0; i < 20; ++i)
	{
		DWORD dwSeed = (i << 16) | (time(NULL) % 1000);
		srand(dwSeed);

		if (0 == (rand() % 2))
		{
			vRandPos.x += float(rand() % 3) + 0.2f;
			vRandPos.z += float(rand() % 3) + 0.2f;
		}
		else
		{
			vRandPos.x -= float(rand() % 3) - 0.2f;
			vRandPos.z -= float(rand() % 3) - 0.2f;
		}

		vRandPos.y += float(rand() % 2) - 0.2f;

		vDir = vRandPos - vPos;

		m_BulletList.push_back(CMstBullet::Create(m_pGraphicDev));
		dynamic_cast<CMstBullet*>(m_BulletList.back())->Set_Dir(vDir);
	}
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
