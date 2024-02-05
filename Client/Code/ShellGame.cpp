#include "stdafx.h"
#include "ShellGame.h"

#include "Export_System.h"
#include "Export_Utility.h"


CShellGame::CShellGame(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMapObj(pGraphicDev),
	m_pShellNpc(nullptr)
{
}

CShellGame::CShellGame(const CShellGame& rhs)
	: CMapObj(rhs),
	m_pShellNpc(rhs.m_pShellNpc)
{
}

CShellGame::~CShellGame()
{
}

void CShellGame::Set_ShellObj_ToStage(CLayer* pLayer)
{
	pLayer->Add_GameObject(L"ShellNpc", m_pShellNpc);

	for (auto& iter : m_vecShell)
	{
		pLayer->Add_GameObject(L"Shell", iter);
	}
}

HRESULT CShellGame::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransformCom->Set_Pos(20.f, 1.f, 30.f);

	m_fSpeed = 0.1f;

	m_fCallLimit = 2.f;

	m_iShake_Lev = 0; // Shaking 단계

	m_bGame = false;
	m_bShellShaking = false;
	m_bReward = false;

	return S_OK;
}

_int CShellGame::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	if (m_pShellNpc == nullptr)
		Create_NPC();
	else
		m_pShellNpc->Update_GameObject(fTimeDelta);

	if (m_vecShell.empty())
		Create_Shell();
	else
	{
		for (auto& iter : m_vecShell)
		{
			iter->Update_GameObject(fTimeDelta);
		}
	}

	if (Engine::Get_DIKeyState(DIK_K) & 0x80) // 테스트용 (추후 제거)
	{
		m_bGame = true;
	}

	if (dynamic_cast<CShellNpc*>(m_pShellNpc)->Get_NPC_Game())
	{
		m_bGame = true;
	}

	if (m_bGame)
	{
		Game(fTimeDelta);
	}

	return 0;
}

void CShellGame::LateUpdate_GameObject()
{
	__super::LateUpdate_GameObject();

	if (m_pShellNpc != nullptr)
		m_pShellNpc->LateUpdate_GameObject();

	if (!m_vecShell.empty())
	{
		for (auto& iter : m_vecShell)
		{
			iter->LateUpdate_GameObject();
		}
	}
}

void CShellGame::Render_GameObject()
{
	if (m_pShellNpc != nullptr)
		m_pShellNpc->Render_GameObject();

	if (!m_vecShell.empty())
	{
		for (auto& iter : m_vecShell)
		{
			iter->Render_GameObject();
		}
	}
}

HRESULT CShellGame::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

#pragma region Texture

	// NPC _ IDLE
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellNpcTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellNpcTexture", pComponent });

	// NPC _ GAMING
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellNpcGameTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellNpcGameTexture", pComponent });

	// SHELL
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShellTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_ShellTexture", pComponent });

#pragma endregion Texture

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculator = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	return S_OK;

}

void CShellGame::Create_NPC()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pShellNpc = CShellNpc::Create(m_pGraphicDev);
	m_pShellNpc->Set_MyLayer(m_vecMyLayer[0]);
	m_pShellNpc->Get_TransformCom()->Set_Pos(vPos);

	if (m_pShellNpc == nullptr)
		return;
}

void CShellGame::Create_Shell()
{
	// Shell 세 개 생성
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	float fScalar = SCALAR_X;

	for (int i = 0; i < 3; ++i)
	{
		CShell* pShell = CShell::Create(m_pGraphicDev);
		pShell->Set_MyLayer(m_vecMyLayer[0]);
		pShell->Get_TransformCom()->Set_Pos(vPos.x + fScalar, vPos.y, vPos.z - 1.5f);
		m_vecShell.push_back(pShell);
		fScalar += INTERVALX;
	}
}

void CShellGame::Game(const _float& fTimeDelta)
{
	// 제일 우측 Shell 하나가 위로 들어졌다 내려옴
	// npc 상태 Gaming 으로 변환 / 일정 시간 동안 shell 섞임

	if (!m_bShellShaking)
	{
		// npc 의 상태 변화

		// Shell 3번 위로 
		dynamic_cast<CShell*>(m_vecShell.back())->Set_StartUp();

		if (dynamic_cast<CShell*>(m_vecShell.back())->Get_Shaking_Ready())
		{
			m_bShellShaking = true;
			Resetting_ShellPos();
		}
	}
	else
	{
		Shaking_Shell(fTimeDelta);
	}
}

void CShellGame::Shaking_Shell(const _float& fTimeDelta)
{
	_vec3 vFirstPos, vSecPos, vThridPos, vGamePos;

	m_pTransformCom->Get_Info(INFO_POS, &vGamePos); // 게임 자체의 위치

	m_vecShell.front()->Get_TransformCom()->Get_Info(INFO_POS, &vFirstPos); // 1번 Shell

	vector<CShell*>::iterator iter = m_vecShell.begin();
	iter++;
	(*iter)->Get_TransformCom()->Get_Info(INFO_POS, &vSecPos); // 2번 Shell

	m_vecShell.back()->Get_TransformCom()->Get_Info(INFO_POS, &vThridPos); // 3번 Shell

	if (0 == m_iShake_Lev) // 1 <-> 2
	{
		vFirstPos.x += m_fSpeed;
		vSecPos.x -= m_fSpeed;

		if ((vGamePos.x <= vFirstPos.x) && (vGamePos.x - INTERVALX) >= vSecPos.x)
		{
			vFirstPos.x = vGamePos.x;
			vSecPos.x = vGamePos.x - INTERVALX;
			++m_iShake_Lev;
		}
	}
	else if (1 == m_iShake_Lev) // 1 <-> 3
	{
		vFirstPos.x += m_fSpeed;
		vThridPos.x -= m_fSpeed;

		if ((vGamePos.x + INTERVALX) <= vFirstPos.x && vGamePos.x >= vThridPos.x)
		{
			vFirstPos.x = vGamePos.x + INTERVALX;
			vThridPos.x = vGamePos.x;
			++m_iShake_Lev;
		}
	}
	else if (2 == m_iShake_Lev) // 2 <-> 1
	{
		vSecPos.x += m_fSpeed;
		vFirstPos.x -= m_fSpeed;

		if ((vGamePos.x + INTERVALX) <= vSecPos.x && vGamePos.x - INTERVALX >= vFirstPos.x)
		{
			vSecPos.x = vGamePos.x + INTERVALX;
			vFirstPos.x = vGamePos.x - INTERVALX;

			m_iShake_Lev = 0;
			m_fSpeed += 0.1f;
		}
	}

	if (0.5 <= m_fSpeed) // 게임 종료
	{
		m_fSpeed = 0.1f;
		m_iShake_Lev = 0;
		m_bShellShaking = false;
		m_bGame = false;
		m_bReward = true;

		m_pShellNpc->Set_NpC_Game(); // Npc의 상태를 다시 false 로

		Setting_RewardShell(); // 3개 중 하나의 Shell 을 당첨으로 설정
	}

	m_vecShell.front()->Get_TransformCom()->Set_Pos(vFirstPos);
	(*iter)->Get_TransformCom()->Set_Pos(vSecPos);
	m_vecShell.back()->Get_TransformCom()->Set_Pos(vThridPos);
}

void CShellGame::Resetting_ShellPos()
{
	_vec3 vFirstPos, vSecPos, vThridPos, vGamePos;

	m_pTransformCom->Get_Info(INFO_POS, &vGamePos); // 게임 자체의 위치

	m_vecShell.front()->Get_TransformCom()->Set_Pos(vGamePos.x - INTERVALX, vGamePos.y, vGamePos.z - 1.5f);

	vector<CShell*>::iterator iter = m_vecShell.begin();
	iter++;
	(*iter)->Get_TransformCom()->Set_Pos(vGamePos.x, vGamePos.y, vGamePos.z - 1.5f);

	m_vecShell.back()->Get_TransformCom()->Set_Pos(vGamePos.x + INTERVALX, vGamePos.y, vGamePos.z - 1.5f);
}

void CShellGame::Setting_RewardShell()
{
	// 랜덤으로 당첨 Shell 선택
	DWORD dwSeed = time(NULL) % 1000;
	srand(dwSeed);

	int iRewardShell = rand() % 3;

	switch (iRewardShell)
	{
	case 0:
		m_vecShell.front()->Setting_Reward();
		break;
	case 1:
	{
		vector<CShell*>::iterator iter = m_vecShell.begin();
		iter++;
		(*iter)->Setting_Reward();
		break;
	}
	case 2:
		m_vecShell.back()->Setting_Reward();
		break;
	default:
		break;
	}
}

CShellGame* CShellGame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShellGame* pInstance = new CShellGame(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("ShellGame Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CShellGame::Free()
{
	Safe_Release<CShellNpc*>(m_pShellNpc);
	m_pShellNpc = nullptr;

	for_each(m_vecShell.begin(), m_vecShell.end(), CDeleteObj());

	__super::Free();
}
