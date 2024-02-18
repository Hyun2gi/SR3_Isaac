#include "stdafx.h"
#include "..\Header\Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "PlayerBullet.h"
#include "BrimStoneBullet.h"
#include "DynamicCamera.h"
#include "EpicBullet.h"
#include "EpicBulletMark.h"

#include "PlayerLeg.h"

IMPLEMENT_SINGLETON(CPlayer)

CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
	Free();
}

HRESULT CPlayer::Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (m_bInitialize == false)
	{
		m_bInitialize = true;

		FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
		m_pGraphicDev = pGraphicDev;

		m_ePreState = P_END;
		m_eCurState = P_IDLE;

		m_ePreBulletState = P_BULLET_END;
		m_eCurBulletState = P_BULLET_IDLE; //P_BULLET_IDLE; // P_BULLET_BRIMSTONE // P_BULLET_EPIC
		m_ePreState = P_END;

		// ������ �ð� �ʱ�ȭ
		m_fShootDelayTime = 0;
		m_fDelayTime = 0;
		m_bKeyBlock = false;
		m_fSpriteSpeed = 0.9f;

		m_fFrame = 0.f;
		m_fPicNum = 0.f;

		m_fMaxHp = 3;
		m_fHp = 3;
		m_iCoin = 20;

		m_fMoveSpeed = 10;
		m_fBulletSpeed = 60;

		// �Ѿ� ���� �ð�
		m_fAttackSpeed = 20;
		m_bStartScene = true;

		//m_pTransformCom->m_vScale = { 2.f, 1.f, 1.f };
		m_bMouseYRotataion = true;

		m_bEpicTargetRun = false;
		m_bEpicLieTiming = false;

		m_bShoot = false;
		m_bBrimeStoneShoot = false;

		m_bUnbeatable = false;
		m_fUnbeatableTime = 0;

		m_iColorTimer = 0;
		m_iTempTimer = 0;
		m_pCamera = nullptr;
		m_vStartPos = _vec3(VTXCNTX / 2, 0, VTXCNTZ / 2);
		m_bRender = true;

		m_bStartAnim = true;

		// ���������� ����
		m_eCurPlayerVer = P_ISAAC;   //P_ISAAC;  //P_AZAZEL;

		// 0�϶��� ������
		m_iAzaelStateSet = 0;

		// ����������
		m_bAzazelType = false;

		// ���������� ���� �ٸ������
		m_pLeg = CPlayerLeg::Create(m_pGraphicDev);
		m_bCameraShaking = false;
		m_fUnbeatableTimeFix = 1.5f;
		m_bAlreadyDead = false;

		m_bDople = false;
	}
	else
	{
		dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_ChaseInit(true);
	}
	

	return S_OK;
}

Engine::_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bStartScene)
	{
		// �� �� ��������
		// EpicMakrList �ʱ�ȭ���ֱ�
		if (!m_EpicMarkList.empty())
		{
			for (auto& iter = m_EpicMarkList.begin();
				iter != m_EpicMarkList.end(); )
			{
				Engine::Safe_Release(*iter);
				iter = m_EpicMarkList.erase(iter);
			}
		}

		Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
		NULL_CHECK(pTerrainBufferCom);

		_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&m_vStartPos, pTerrainBufferCom->Get_VtxPos());

		if (m_bStartAnim)
		{
			// ù �ִϸ��̼� ������ �߰�����
			m_pTransformCom->Set_Pos(_vec3(VTXCNTX / 2, 0, VTXCNTZ / 2));
		}
		else
		{
			m_pTransformCom->Set_Pos(m_vStartPos.x, 0, m_vStartPos.z);
		}
		
		m_bStartScene = false;

		if (m_pCamera != nullptr)
		{
			//dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_FirstPerson(true);
		}

		m_bKeyBlock = false;

	}

	if (m_bStartAnim)
	{
		m_bStartAnim = false;
		// �ּ� ���ֱ�
		//Set_Cry_Anim();
	}

	// Ư�� ��� ó��
	Specific_Motion(fTimeDelta);
	Check_UnBeatable_Time(fTimeDelta);
		
	if (m_bKeyBlock == false)
	{
		Key_Input(fTimeDelta);
	}
	else
	{
		//block�϶� player�� ��� �����̴°� ����
		if (m_eCurState == P_IDLEWALK || m_eCurState == P_LEFTWALK || m_eCurState == P_RIGHTWALK || m_eCurState == P_SHOOTWALK)
		{
			m_eCurState = P_BACKWALK;
			m_fFrame = 0;
		}
	}

	if (!m_bKeyBlock)
	{
		m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;
	}


	if (m_eCurPlayerVer == P_AZAZEL && m_eCurState != P_SHOOTIDLE && m_eCurState != P_SHOOTWALK && m_fPicNum < m_fFrame)
	{
		m_fFrame = 0.f;
	}
	else if (m_eCurPlayerVer == P_AZAZEL && (m_eCurState == P_SHOOTIDLE || m_eCurState == P_SHOOTWALK))
	{
		// 7 �̻�
		if (6 < m_fFrame)
		{
			if (m_PlayerBulletList.empty() && m_bBrimeStoneShoot)
			{
				Engine::PlayEffect(L"blood laser long.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 0.4f);
				// bullet �߰��ϱ�
				for (int i = 0; i < 50; i++)
				{
					m_PlayerBulletList.push_back(CBrimStoneBullet::Create(m_pGraphicDev, m_pLayerTag, i, false));
					m_PlayerBulletList.push_back(CBrimStoneBullet::Create(m_pGraphicDev, m_pLayerTag, i, true));
				}
				m_bBrimeStoneShoot = false;
			}
		}

		// �������϶� ��� ����
		if (m_fPicNum < m_fFrame)
		{
			m_fFrame = m_fPicNum - 1;
		}

		if (!m_PlayerBulletList.empty())
		{
			//Engine::PlayEffect(L"blood laser short.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 0.8f);
		}

		if (m_PlayerBulletList.empty())
		{
			Engine::StopSound(SOUND_EFFECT_PLAYER_STOPSUDDEN);
		}
	}


	// P_THUMBS_UP �϶��� ó�� ��������Ʈ�� ���ư��� �ȵ�
	if (m_fPicNum < m_fFrame && m_eCurState != P_THUMBS_UP && m_eCurPlayerVer == P_ISAAC)
	{
		m_fFrame = 0.f;
	}
	
	// �Ѿ� update
	if (!m_PlayerBulletList.empty())
	{
		int		iResult = 0;
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			if (m_eCurBulletState == P_BULLET_IDLE)
			{
				iResult = dynamic_cast<CPlayerBullet*>(*iter)->Update_GameObject(fTimeDelta);
			}
			else if (m_eCurBulletState == P_BULLET_BRIMSTONE)
			{
				iResult = dynamic_cast<CBrimStoneBullet*>(*iter)->Update_GameObject(fTimeDelta);
			}
			else if (m_eCurBulletState == P_BULLET_EPIC)
			{
				iResult = dynamic_cast<CEpicBullet*>(*iter)->Update_GameObject(fTimeDelta);
			}
			
			if (1 == iResult)
			{
				//Safe_Delete<CGameObject*>(*iter);
				Safe_Release<CGameObject*>(*iter);
				iter = m_PlayerBulletList.erase(iter);
			}
			else
			{
				++iter;
			}
			 
		}
	}

	if (!m_EpicMarkList.empty())
	{
		int		iResult = 0;
		for (auto& iter = m_EpicMarkList.begin();
			iter != m_EpicMarkList.end(); )
		{
			iResult = dynamic_cast<CEpicBulletMark*>(*iter)->Update_GameObject(fTimeDelta);

			if (1 == iResult)
			{
				//Safe_Delete<CGameObject*>(*iter);
				Engine::Safe_Release<CGameObject*>(*iter);
				iter = m_PlayerBulletList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		
	}

	// �������϶� �ٸ� ������Ʈ
	if (m_eCurPlayerVer == P_AZAZEL)
	{
		dynamic_cast<CPlayerLeg*>(m_pLeg)->Update_GameObject(fTimeDelta);

		// �������϶� �ٸ� ���� ���� ���
		_vec3 playerpos = m_pTransformCom->m_vInfo[INFO_POS];
		playerpos -= _vec3(0, 0.2, 0);
		m_pTransformCom->Set_Pos(playerpos);
	}

	if (m_pTransformCom->m_vInfo[INFO_POS].y != 1)
	{
		m_pTransformCom->m_vInfo[INFO_POS].y = 1;
	}
	

	CGameObject::Update_GameObject(fTimeDelta);

	//Height_OnTerrain();

	if (m_bRender)
	{
		Engine::Add_RenderGroup(RENDER_ALPHA_SORTING, this);
	}

	return 0;
}

void CPlayer::LateUpdate_GameObject()
{
	Motion_Change();
	Bullet_Change();

	// �Ѿ� late update
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter : m_PlayerBulletList)
		{
			if (m_eCurBulletState == P_BULLET_IDLE)
			{
				dynamic_cast<CPlayerBullet*>(iter)->LateUpdate_GameObject();
			}
			else if (m_eCurBulletState == P_BULLET_BRIMSTONE)
			{
				dynamic_cast<CBrimStoneBullet*>(iter)->LateUpdate_GameObject();
			}
			else if (m_eCurBulletState == P_BULLET_EPIC)
			{
				dynamic_cast<CEpicBullet*>(iter)->LateUpdate_GameObject();
			}
		}
	}

	if (!m_EpicMarkList.empty())
	{
		for (auto& iter : m_EpicMarkList)
		{
			dynamic_cast<CEpicBulletMark*>(iter)->LateUpdate_GameObject();
		}
	}

	// �������϶� �ٸ� ������Ʈ
	if (m_eCurPlayerVer == P_AZAZEL)
	{
		dynamic_cast<CPlayerLeg*>(m_pLeg)->LateUpdate_GameObject();

	}


	__super::LateUpdate_GameObject();

	

	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	__super::Compute_ViewZ(&vPos);
}

void CPlayer::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pTextureCom->Set_Texture((_uint)m_fFrame);

	

	m_pBufferCom->Render_Buffer();
	/*m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);*/
}

HRESULT CPlayer::Add_Component()
{
	CComponent*		pComponent = nullptr;
	
	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_RcTex", pComponent });

	//PLAYER �ؽ�ó
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_BACK"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_BACK", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_BACK_SMALL"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_BACK_SMALL", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_RIGHT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_RIGHT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_LEFT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_LEFT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_THUMBS_UP"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_THUMBS_UP", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_ATTACKED"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_ATTACKED", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_GET_BAD_ITEM"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_GET_BAD_ITEM", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_LIE_CRY"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_LIE_CRY", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_LIE_CRY_OPEN_EYE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_LIE_CRY_OPEN_EYE", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_DIE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_DIE", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_IDLE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_IDLE", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Proto_Transform", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_Calculator", pComponent });

	// ������ �ؽ�ó
	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_HEAD_BACK"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_HEAD_BACK", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_HEAD_LEFT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_HEAD_LEFT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_HEAD_RIGHT"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_HEAD_RIGHT", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_HEAD_IDLE"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_HEAD_IDLE", pComponent });

	pComponent = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTexture_AZAZEL_ATTACKED"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Proto_PlayerTexture_AZAZEL_ATTACKED", pComponent });
		
	return S_OK;
}


//CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
//{
//	CPlayer *	pInstance = new CPlayer(pGraphicDev);
//
//	if (FAILED(pInstance->Ready_GameObject(pGraphicDev)))
//	{
//		Safe_Release(pInstance);
//		MSG_BOX("Player Create Failed");
//		return nullptr;
//	}
//
//	return pInstance;
//}


void CPlayer::Set_Camera_Cinemachine_01()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Cinemachine_01_TotalLand(); 
}

void CPlayer::Set_Camera_Cinemachine_02()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Cinemachine_02_MiddleTotalLand();
}

void CPlayer::Set_Camera_Cinemachine_03()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Cinemachine_03_GoToIsaac();
}

void CPlayer::Set_Camera_Cinemachine_04()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Cinemachine_04_Dople_GoToIsaac();
}

void CPlayer::Set_Player_Pos(_vec3 pos)
{
	m_pTransformCom->Set_Pos(pos); 
}


void CPlayer::Set_MouseRotation(float xRad, float yRad)
{
	m_pTransformCom->Rotation(ROT_Y, xRad);
}


void CPlayer::Set_BulletType(int _bullet)
{
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			Engine::Safe_Release(*iter);
			iter = m_PlayerBulletList.erase(iter);
		}
	}

	switch (_bullet)
	{
	case 1:
		m_eCurBulletState = P_BULLET_IDLE;
		m_fShootDelayTime = 20;
		break;
	case 2:
		m_eCurBulletState = P_BULLET_BRIMSTONE;
		m_fAttackSpeed = 120;
		break;
	case 3:
		m_eCurBulletState = P_BULLET_EPIC;
		m_fAttackSpeed = 80;
		break;
	}
}

void CPlayer::Set_EpicFall()
{
	if (m_eCurBulletState == P_BULLET_EPIC)
	{
		if (!m_PlayerBulletList.empty())
		{
			// target ���°� �ƴ� (Ű���� ������ Ǯ��)
			// ���� ������ �������°� ����
			// Ű���� ���°͵� ��
			m_bEpicTargetRun = false;
			dynamic_cast<CEpicBullet*>(m_PlayerBulletList.back())->Set_StartFall(true);
		}
	}
}


void CPlayer::Set_Item_Get_Anim()
{
	m_bKeyBlock = true;
	m_eCurState = P_THUMBS_UP;
	m_fDelayTime = 0;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToPlayerFront();
}

void CPlayer::Set_Item_Get_Anim_Brim()
{
	m_bKeyBlock = true;
	m_bAzazelType = true;
	m_eCurState = P_THUMBS_UP;
	m_fDelayTime = 0;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToPlayerFront();
}

void CPlayer::Set_Brim_Cinemachine()
{
	CTransform* pTest = dynamic_cast<CTransform*>(CPlayer::GetInstance()->Get_Component_Player_Transform());

	_matrix mat = *(pTest->Get_WorldMatrix());
	//mat._42 += 3.f;

	//�Լ� ��� ��
	Engine::Create_Explosion(m_pGraphicDev, mat);
	Engine::Create_Explosion(m_pGraphicDev, mat);
	Engine::Create_Explosion(m_pGraphicDev, mat);
	Engine::Create_Explosion(m_pGraphicDev, mat);
	Engine::Create_Dust(m_pGraphicDev, mat);
	Engine::Create_Dust(m_pGraphicDev, mat);
	Engine::Create_Dust(m_pGraphicDev, mat);
	Engine::Create_Dust(m_pGraphicDev, mat);
	//mat._41 += 3.f;


	/*D3DXCOLOR temp;
	temp += D3DXCOLOR(0.01f, 0.01f, 0.01f, 0.01f);
	if (CPlayer::GetInstance()->Get_Component_Player_TexBuffer())
		CPlayer::GetInstance()->Get_Component_Player_TexBuffer()->Set_Color(temp);*/
}

void CPlayer::Set_Item_Get_Anim_Bad()
{
	m_bKeyBlock = true;
	m_eCurState = P_GET_BAD_ITEM;
	m_fDelayTime = 0;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToPlayerFront();
}

bool CPlayer::Get_Camera_WallBlock()
{
	_vec3	vPos, vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vScale = m_pTransformCom->m_vScale;

	if (vPos.x < VTXCNTX - 2.7 && vPos.z < VTXCNTX - 2.7
		&& vPos.x > 2.7 && vPos.z > 2.7)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CPlayer::Get_SafeCamera_Area()
{
	_vec3	vPos, vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vScale = m_pTransformCom->m_vScale;

	if (vPos.x < VTXCNTX - 4 && vPos.z < VTXCNTX - 4
		&& vPos.x > 4 && vPos.z > 4 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CPlayer::Get_SafeCamera_Area_For_ChangeStage()
{
	_vec3	vPos, vScale;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vScale = m_pTransformCom->m_vScale;

	if (vPos.x < VTXCNTX - 6 && vPos.z < VTXCNTX - 6
		&& vPos.x > 6 && vPos.z > 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CPlayer::Get_Player_OutOfRange()
{
	_vec3	vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	
	if (vPos.x >= VTXCNTX - 3 || vPos.z >= VTXCNTX - 3
		|| vPos.x <= 3 || vPos.z <= 3)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CPlayer::Plus_EpicBulletMark(_vec3 pos)
{
	// �ٴ����� ��������
	_vec3 tempPos = pos;
	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);
	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&tempPos, pTerrainBufferCom->Get_VtxPos());
	tempPos = _vec3(tempPos.x, fHeight + 0.2, tempPos.z);

	m_EpicMarkList.push_back(CEpicBulletMark::Create(m_pGraphicDev, m_pLayerTag, tempPos));
}


// �� �̵������� �ٴڿ� �ִ� ���� ���� �����
void CPlayer::Clear_EpicBulletMark()
{
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			Safe_Release<CGameObject*>(*iter);
			iter = m_PlayerBulletList.erase(iter);
		}
	}
}

void CPlayer::Change_LastEpicMark_To_Trace()
{
	dynamic_cast<CEpicBulletMark*>(m_EpicMarkList.back())->Set_Bullet_Mark(2);
}

int CPlayer::Get_PlayerCurState()
{
	switch (m_eCurState)
	{
	case P_IDLE:
		return 0;
	case P_IDLEWALK:
		return 1;
	case P_BACKWALK:
		return 2;
	case P_LEFTWALK:
		return 3;
	case P_RIGHTWALK:
		return 4;
	case P_SHOOTWALK:
		return 5;
	case P_THUMBS_UP:
		return 6;
	case P_GET_BAD_ITEM:
		return 7;
	case P_ATTACKED:
		return 8;
	}
}

int CPlayer::Get_PlayerBulletState()
{
	switch (m_eCurBulletState)
	{
	case P_BULLET_IDLE:
		return 0;
	case P_BULLET_BRIMSTONE:
		return 1;
	case P_BULLET_EPIC:
		return 2;
	}
}

void CPlayer::Set_Cry_Anim()
{
	m_eCurState = P_CRY_LIE;
	m_bKeyBlock = true;
	m_fDelayTime = 0.f;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Cinemachine_00_Start();

	// �ٲ��� ������
	Engine::StopAll();
	Engine::PlayEffect(L"GameStart.ogg", SOUND_BGM, 1.f);
}

void CPlayer::Set_Attacked()
{
	if (m_eCurState != P_ATTACKED && m_bUnbeatable == false)
	{
		srand((unsigned)time(NULL));
		int num = rand() % 4;

		if (num == 0)
		{
			// ����
			Engine::PlayEffect(L"hurt grunt 1.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 1.f);
		}
		else if (num == 1)
		{
			// ����
			Engine::PlayEffect(L"Hit_1.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 1.f);
		}
		else if (num == 2)
		{
			// ����
			Engine::PlayEffect(L"Hit_2.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 1.f);
		}
		else if (num == 3)
		{
			// ����
			Engine::PlayEffect(L"Hit_3.wav", SOUND_EFFECT_PLAYER_STOPSUDDEN, 1.f);
		}
		
		
		if (m_fHp >= 0.5)
		{
			m_fHp -= 0.5;
		}

		// �������϶���
		if (m_fHp == 0 && m_eCurPlayerVer == P_ISAAC && m_bAlreadyDead == false)
		{
			m_eCurState = P_DIE;
			m_bUnbeatable = true;
			m_bKeyBlock = true;
			m_fDelayTime = 0.f;
			m_bAlreadyDead = true;
			return;
		}


		m_fUnbeatableTime = 0;
		m_bUnbeatable = true;
		m_eCurState = P_ATTACKED;
		// Ű����
		m_bKeyBlock = true;
		m_fDelayTime = 0.f;

		// �������� �ǰݴ��Ҷ� �ٸ� �����ǵ���
		m_iAzaelStateSet = 5;
	}
}

void CPlayer::Set_StartAngle()
{
	m_pTransformCom->m_vAngle = { 0,0,0 };
}

void CPlayer::Set_StartCameraMouse()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_TotalCameraStart(); 
}

_vec3 CPlayer::Get_BulletDir()
{
	_vec3 playerDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &playerDir);
	if (dynamic_cast<CDynamicCamera*>(m_pCamera)->Get_FirstPerson())
	{
		// 1��Ī��
		m_vBulletDir = _vec3(playerDir.x, playerDir.y / 3, playerDir.z);
	}
	else
	{
		_vec3 camerapos, playerpos;
		m_pTransformCom->Get_Info(INFO_POS, &playerpos);
		camerapos = dynamic_cast<CDynamicCamera*>(m_pCamera)->Get_EyePos();

		//�Ѿ� �� ���� ������ ����
		playerpos += _vec3(0, 2, 0);

		m_vBulletDir = playerpos - camerapos;

		D3DXVec3Normalize(&m_vBulletDir, &m_vBulletDir);

		if (m_vBulletDir.y < 0)
		{
			m_vBulletDir = _vec3(playerDir.x, playerDir.y / 3, playerDir.z);
		}
	}
	

	return m_vBulletDir;
}


void CPlayer::Set_CameraShaking(float shakeTime, float shakeIntensity)
{
	//OnShakeCameraPos(0.2, 2);
	// shakeIntensity : 1����..(�⺻��)
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnShakeCameraPos(shakeTime, shakeIntensity);
}

void CPlayer::Set_CameraShaking_Sub(float shakeTime, float shakeIntensity)
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnShakeCameraPos_Sub(shakeTime, shakeIntensity);
}

void CPlayer::Set_CameraShaking_Rot(float shakeTime, float shakeIntensity)
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->OnShakeCameraRot(shakeTime, shakeIntensity);
}

void CPlayer::Set_OnSlotMode(CTransform* slotTransform)
{
	// �����϶� player render ����
	m_bRender = false;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_OnSlotCamera(slotTransform);

	_vec3 slotPos, slotDir;
	slotTransform->Get_Info(INFO_POS, &slotPos);
	slotTransform->Get_Info(INFO_LOOK, &slotDir);

	slotPos += -slotDir*4;

	// �÷��̾� ������ ����
	m_bKeyBlock = true;

	m_pTransformCom->Set_Pos(slotPos);
}

void CPlayer::Set_OffSlotMode()
{
	// �÷��̾� ������ �ѱ�
	m_bKeyBlock = false;
	m_eCurState = P_BACKIDLE;
	m_fFrame = 0.f;
	// �����϶� player render �ѱ�
	m_bRender = true;
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_OffSlotCamera();
}

void CPlayer::Set_OnDople()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_OnDople();
	m_pTransformCom->m_vAngle = { D3DXToRadian(90),0,0 };
	m_bKeyBlock = false;
	m_bDople = true;
}

void CPlayer::Set_OffDople()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_OffDople();
	m_pTransformCom->m_vAngle = { 0,0,0 };
	m_bDople = false;
}

void CPlayer::Set_StopShaking()
{
	dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_StopShaking();
}


void CPlayer::Bullet_Change_To_Brim()
{
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			Safe_Release<CGameObject*>(*iter);
			iter = m_PlayerBulletList.erase(iter);
		}
	}

	m_eCurBulletState = P_BULLET_BRIMSTONE;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	// W,A,S,D ������
	_vec3		vDir;
	m_pTransformCom->Get_Info(INFO_LOOK, &vDir);

	_vec3		vPos;
	_vec3		vScale;

	vScale = m_pTransformCom->m_vScale;


	// epictarget ���� �����϶��� block��
	if (m_bEpicTargetRun == false && m_bDople == false)
	{
		//vDir = _vec3(vDir.x, 0, vDir.z);

		if (Engine::Get_DIKeyState(DIK_W) & 0x80)
		{
			m_eCurState = P_BACKWALK;
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (m_fMoveSpeed)*fTimeDelta;
			
			
			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, m_fMoveSpeed, fTimeDelta);
			}
			
			

			if (m_bShoot && m_eCurPlayerVer == P_ISAAC)
			{
				m_eCurState = P_SHOOTWALK;
				// ��鼭 ������ ���� ���
				m_iShootWalkDir = 0;
			}
			else if ((m_bBrimeStoneShoot || !m_PlayerBulletList.empty()) && m_eCurPlayerVer == P_AZAZEL)
			{
				m_eCurState = P_SHOOTWALK;
				// ��鼭 ������ ���� ���
				m_iShootWalkDir = 0;
			}


			// ������ anim ����
			m_iAzaelStateSet = 1;
		}
		else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
		{
			m_eCurState = P_IDLEWALK;
			vDir = _vec3(vDir.x, 0, vDir.z);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (-m_fMoveSpeed) * fTimeDelta;
			
			
			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, -m_fMoveSpeed, fTimeDelta);
			}
			
			if (m_bShoot && m_eCurPlayerVer == P_ISAAC)
			{
				m_eCurState = P_SHOOTWALK;
				m_iShootWalkDir = 1;
			}
			else if ((m_bBrimeStoneShoot || !m_PlayerBulletList.empty()) && m_eCurPlayerVer == P_AZAZEL)
			{
				m_eCurState = P_SHOOTWALK;
				// ��鼭 ������ ���� ���
				m_iShootWalkDir = 0;
			}

			// ������ anim ����
			m_iAzaelStateSet = 2;
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
			//vDir = _vec3(vDir.x, 0, vDir.z);
			m_eCurState = P_LEFTWALK;
			D3DXVec3Normalize(&vDir, &vDir);

			
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (-m_fMoveSpeed) * fTimeDelta;

			
			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, -m_fMoveSpeed, fTimeDelta);
			}
			

			if (m_bShoot && m_eCurPlayerVer == P_ISAAC)
			{
				m_eCurState = P_SHOOTWALK;
				m_iShootWalkDir = 2;
			}
			else if ((m_bBrimeStoneShoot || !m_PlayerBulletList.empty()) && m_eCurPlayerVer == P_AZAZEL)
			{
				m_eCurState = P_SHOOTWALK;
				// ��鼭 ������ ���� ���
				m_iShootWalkDir = 0;
			}
		

			// ������ anim ����
			m_iAzaelStateSet = 3;
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
			//vDir = _vec3(vDir.x, 0, vDir.z);
			m_eCurState = P_RIGHTWALK;
			D3DXVec3Normalize(&vDir, &vDir);

			
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (m_fMoveSpeed)*fTimeDelta;
			
			
			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, m_fMoveSpeed, fTimeDelta);
			}
			


			if (m_bShoot && m_eCurPlayerVer == P_ISAAC)
			{
				m_eCurState = P_SHOOTWALK;
				m_iShootWalkDir = 3;
			}
			else if ((m_bBrimeStoneShoot || !m_PlayerBulletList.empty()) && m_eCurPlayerVer == P_AZAZEL)
			{
				m_eCurState = P_SHOOTWALK;
				// ��鼭 ������ ���� ���
				m_iShootWalkDir = 0;
			}
			

			// ������ anim ����
			m_iAzaelStateSet = 4;
		}
		else
		{
			if (m_bShoot && m_eCurPlayerVer == P_ISAAC)
			{
				m_eCurState = P_SHOOTIDLE;
			}
			else
			{
				m_eCurState = P_IDLE;
			}

			if (m_bBrimeStoneShoot && m_eCurPlayerVer == P_AZAZEL)
			{
				m_eCurState = P_SHOOTIDLE;
			}
			else
			{
				m_eCurState = P_IDLE;
			}
			
			// ������ anim ����
			m_iAzaelStateSet = 0;
		}
	}
	else if (m_bEpicTargetRun == false && m_bDople == true)
	{
		_vec3		vDir;
		m_pTransformCom->Get_Info(INFO_UP, &vDir);
		
		if (Engine::Get_DIKeyState(DIK_W) & 0x80)
		{
			m_eCurState = P_IDLEWALK;
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (m_fMoveSpeed)*fTimeDelta;


			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, m_fMoveSpeed - 2, fTimeDelta);
			}


			// ������ anim ����
			m_iAzaelStateSet = 1;
		}
		else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
		{
			m_eCurState = P_IDLEWALK;
			vDir = _vec3(vDir.x, 0, vDir.z);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (-m_fMoveSpeed) * fTimeDelta;


			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, -(m_fMoveSpeed - 2), fTimeDelta);
			}

			// ������ anim ����
			m_iAzaelStateSet = 2;
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
			//vDir = _vec3(vDir.x, 0, vDir.z);
			m_eCurState = P_LEFTWALK;
			D3DXVec3Normalize(&vDir, &vDir);


			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (-m_fMoveSpeed) * fTimeDelta;


			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, -(m_fMoveSpeed - 2), fTimeDelta);
			}

			// ������ anim ����
			m_iAzaelStateSet = 3;
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
			//vDir = _vec3(vDir.x, 0, vDir.z);
			m_eCurState = P_RIGHTWALK;
			D3DXVec3Normalize(&vDir, &vDir);


			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos += vDir * (m_fMoveSpeed)*fTimeDelta;


			if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
				&& vPos.x > vScale.x && vPos.z > vScale.z)
			{
				m_pTransformCom->Move_Pos(&vDir, m_fMoveSpeed-2, fTimeDelta);
			}


			// ������ anim ����
			m_iAzaelStateSet = 4;
		}
		else
		{
			m_eCurState = P_IDLE;
		}
	}
	

	// �� delay�� ������ �ʰ� �־ ī��Ʈ �ؾ��ϱ� ������ �����ֱ�
	if (m_fShootDelayTime != 0)
	{
		m_fShootDelayTime++;

		if (m_fShootDelayTime > m_fAttackSpeed)
		{
			m_fShootDelayTime = 0;
		}
	}

	

	// �Ѿ� �߻�
	if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	{
		m_iTempTimer++;
		if (m_eCurBulletState == P_BULLET_IDLE)
		{
			if (m_eCurState != P_SHOOTWALK)
			{
				m_eCurState = P_SHOOTIDLE;
			}

			// m_iTempTimer : �Ӹ� �۾����� Ŀ���°ſ� ��ȭ�ִ� ���
			if (m_fShootDelayTime != 0 && m_eCurState == P_SHOOTIDLE && m_iTempTimer > 30 && m_eCurBulletState == P_BULLET_IDLE)
			{
				// ���� �������� ���� ���ϰ� ������
				// ������ ��������
				m_iTempTimer = 0;
				m_eCurState = P_BACKIDLE;
			}
			else if (m_fShootDelayTime != 0 && m_eCurState == P_SHOOTWALK && m_iTempTimer > 30 && m_eCurBulletState == P_BULLET_IDLE)
			{
				// ���� �������� ���� ���ϰ� ������
				// �����϶�
				m_iTempTimer = 0;
				m_eCurState = P_BACKWALK;
			}

			//// m_iTempTimer : �Ӹ� Ŀ���°� ��ȭ�ȹ�����
			//if (m_fShootDelayTime != 0 && m_eCurState == P_SHOOTIDLE  && m_eCurBulletState != P_BULLET_IDLE)
			//{
			//	// ���� �������� ���� ���ϰ� ������
			//	// ������ ��������
			//	m_iTempTimer = 0;
			//	m_eCurState = P_BACKIDLE;
			//}
			//else if (m_fShootDelayTime != 0 && m_eCurState == P_SHOOTWALK && m_eCurBulletState != P_BULLET_IDLE)
			//{
			//	// ���� �������� ���� ���ϰ� ������
			//	// �����϶�
			//	m_iTempTimer = 0;
			//	m_eCurState = P_BACKWALK;
			//}



			// m_fShootDelay�� 0�϶��� �� �� ����
			if (m_fShootDelayTime == 0)
			{
				m_bShoot = true;
				if (m_eCurBulletState == P_BULLET_IDLE)
				{
					m_PlayerBulletList.push_back(CPlayerBullet::Create(m_pGraphicDev, m_pLayerTag));
				}
				else if (m_eCurBulletState == P_BULLET_BRIMSTONE)
				{
					/*for (int i = 0; i < 50; i++)
					{
						m_PlayerBulletList.push_back(CBrimStoneBullet::Create(m_pGraphicDev, m_pLayerTag, i, false));
						m_PlayerBulletList.push_back(CBrimStoneBullet::Create(m_pGraphicDev, m_pLayerTag, i, true));
					}*/
				}
				m_fShootDelayTime++;
			}
			else
			{
				if (m_PlayerBulletList.empty())
				{
					m_eCurState = P_BACKIDLE;
				}
			}
		}
		else if (m_eCurBulletState == P_BULLET_BRIMSTONE)
		{
			// delay�� 0�̸� �� �� ����
			if (m_fShootDelayTime == 0)
			{
				m_fShootDelayTime++;
				m_bShoot = true;
				//brimstone �� �� �ִ��� ������
				m_bBrimeStoneShoot = true;
			}
			
		}
		else if (m_eCurBulletState == P_BULLET_EPIC)
		{
			if (!m_PlayerBulletList.empty())
			{
				// �ٽ� �÷��̾� �������� ����
				// �ش� ��ġ�� �̵��ϱ�
				//m_PlayerBulletList.push_back(CEpicBullet::Create(m_pGraphicDev, m_pLayerTag));

				CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
				//NULL_CHECK_RETURN(pTerrainBufferCom, _vec3());

				CTransform* pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Terrain", L"Proto_Transform"));
				//NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

				_vec3 targetpos = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
				dynamic_cast<CEpicBullet*>(m_PlayerBulletList.back())->Set_Shoot(_vec3(targetpos.x, 0, targetpos.z));
				dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_Shoot_End_Epic();

				//epicmark�� ����
				Plus_EpicBulletMark(_vec3(targetpos.x, 0, targetpos.z));

				m_bEpicLieTiming = false;
			}
		}
	}
	else
	{
		// ���� ���콺 Ŭ������
		m_bShoot = false;
	}


	// BRIMSTONE�� ���� Ŭ�� �����ϰ� ���������� SHOOT�̾ LIST�� BULLET�� ������ m_bShoot = true����
	//if (m_eCurBulletState == P_BULLET_BRIMSTONE )
	//{
	//	m_bShoot = true;
	//}


	if ((Engine::Get_DIMouseState(DIM_RB) & 0x80) && m_eCurBulletState == P_BULLET_EPIC && m_eCurState!= P_SHOOTWALK)
	{
		//���������
		if (m_PlayerBulletList.empty())
		{
			// ī�޶� ���� �̵� 
			m_eCurState = P_SHOOTWALK;
			m_PlayerBulletList.push_back(CEpicBullet::Create(m_pGraphicDev, m_pLayerTag));
			dynamic_cast<CEpicBullet*>(m_PlayerBulletList.back())->Set_Bullet(1);
			dynamic_cast<CDynamicCamera*>(m_pCamera)->Set_EpicBullet();

			//Ÿ�ٻ���
			m_bEpicTargetRun = true;
			m_bEpicLieTiming = true;
		}
	}


	if (m_eCurBulletState == P_BULLET_BRIMSTONE && !m_PlayerBulletList.empty() && m_eCurState!=P_SHOOTWALK)
	{
		m_eCurState = P_SHOOTIDLE;
	}

	if (dynamic_cast<CDynamicCamera*>(m_pCamera)->Get_ShakingCamera() == false)
	{
		//���콺 ȸ������ �÷��̾� ���� �ٲٱ�
		_long	dwMouseMove(0);

		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
		{
			//m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
		}

		if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
		{
			if (m_bMouseYRotataion)
			{
				//m_pTransformCom->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 20.f));
			}
		}
	}
	
}

void CPlayer::Height_OnTerrain()
{
	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, L"GameLogic", L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

	m_pTransformCom->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

_vec3 CPlayer::Picking_OnTerrain()
{
	CTerrainTex*		pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(ID_STATIC, m_vecMyLayer[0], L"Terrain", L"Proto_TerrainTex"));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3());

	CTransform*			pTerrainTransCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, m_vecMyLayer[0], L"Terrain", L"Proto_Transform"));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3());

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}

void CPlayer::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		if (m_eCurPlayerVer == P_AZAZEL && ((m_ePreState == P_SHOOTIDLE && m_eCurState == P_SHOOTWALK) || (m_ePreState == P_SHOOTWALK && m_eCurState == P_SHOOTIDLE)))
		{
			// �������϶��� shootidle�� shootwalk ���̾���
			return;
		}
		else
		{
			m_fFrame = 0.f;
		}
		

		if (m_eCurPlayerVer == P_ISAAC)
		{
			switch (m_eCurState)
			{
			case P_IDLE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_IDLE"));
				break;
			case P_IDLEWALK:
				m_fPicNum = 11;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_IDLE"));
				break;
			case P_BACKWALK:
				m_fPicNum = 11;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_BACK"));
				break;
			case P_BACKIDLE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_BACK"));
				break;
			case P_SHOOTWALK:
				m_fPicNum = 11;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_BACK_SMALL"));
				break;
			case P_SHOOTIDLE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_BACK_SMALL"));
				break;
			case P_LEFTWALK:
				m_fPicNum = 8;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_LEFT"));
				break;
			case P_RIGHTWALK:
				m_fPicNum = 8;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_RIGHT"));
				break;
			case P_THUMBS_UP:
				// ����
				Engine::StopSound(SOUND_EFFECT_PLAYER_ALLPLAY);
				m_fPicNum = 3;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_THUMBS_UP"));
				break;
			case P_GET_BAD_ITEM:
				// ����
				Engine::StopSound(SOUND_EFFECT_PLAYER_ALLPLAY);
				Engine::PlayEffect(L"thumbs down.wav", SOUND_EFFECT_ITEM_STOPSUDDEN, 1.f);
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_GET_BAD_ITEM"));
				break;
			case P_ATTACKED:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_ATTACKED"));
				break;
			case P_CRY_LIE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_LIE_CRY"));
				break;
			case P_CRY_OPEN_EYE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_LIE_CRY_OPEN_EYE"));
				break;
			case P_DIE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_DIE"));
				break;
			}
		}
		else if (m_eCurPlayerVer == P_AZAZEL)
		{
			// HEAD�� �ѱ��
			switch (m_eCurState)
			{
			case P_IDLE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_IDLE"));
				break;
			case P_IDLEWALK:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_IDLE"));
				break;
			case P_BACKWALK:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_BACK"));
				break;
			case P_BACKIDLE:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_BACK"));
				break;
			case P_SHOOTWALK:
				m_fPicNum = 8;
				m_fSpriteSpeed = 1.f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_BACK"));
				break;
			case P_SHOOTIDLE:
				m_fPicNum = 8;
				m_fSpriteSpeed = 1.f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_BACK"));
				break;
			case P_LEFTWALK:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_LEFT"));
				break;
			case P_RIGHTWALK:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.5f;
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_HEAD_RIGHT"));
				break;
			case P_ATTACKED:
				m_fPicNum = 1;
				m_fSpriteSpeed = 1.f;
				m_bKeyBlock = true; //key ����
				m_pTextureCom = dynamic_cast<CTexture*>(Get_Component_Player(ID_STATIC, L"Proto_PlayerTexture_AZAZEL_ATTACKED"));
				break;
			}
		}
		
		m_ePreState = m_eCurState;
	}
}

void CPlayer::Bullet_Change()
{
	if (m_eCurBulletState != m_ePreBulletState)
	{
		switch (m_eCurBulletState)
		{
		case P_BULLET_IDLE:
			m_fAttackSpeed = 20;
			break;
		case P_BULLET_BRIMSTONE:
			m_fAttackSpeed = 120;
			break;
		case P_BULLET_EPIC:
			m_fAttackSpeed = 80;
			break;
		}
		m_ePreBulletState = m_eCurBulletState;
	}
}

bool CPlayer::Check_Time(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

void CPlayer::Specific_Motion(const _float& fTimeDelta)
{
	if (m_eCurState == P_DIE)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime > 0.8)
		{
			m_fFrame = 1;
		}

		if (m_fDelayTime > 2)
		{
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ
			m_bUnbeatable = false;
			m_fHp = 3;
			//dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToOriginPos();
		}
	}


	if (m_eCurState == P_CRY_LIE)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime > 4)
		{
			m_eCurState = P_CRY_OPEN_EYE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
		}
	}

	if (m_eCurState == P_CRY_OPEN_EYE)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime > 1)
		{
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ
			dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToOriginPos();
		}
	}

	if (m_eCurState == P_ATTACKED)
	{
		m_fDelayTime += fTimeDelta;

		m_iColorTimer++;

		_vec3 vDir, vPos;
		_vec3 vScale = m_pTransformCom->m_vScale;
		m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
		vDir = _vec3(vDir.x, 0, vDir.z);
		vDir *= -1;
		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		vPos += vDir * (m_fMoveSpeed)*fTimeDelta;

		if (vPos.x < VTXCNTX - vScale.x && vPos.z < VTXCNTX - vScale.z
			&& vPos.x > vScale.x && vPos.z > vScale.z)
		{
			m_pTransformCom->Move_Pos(&vDir, 3, fTimeDelta*3);
		}

		if (m_iColorTimer % 5 == 2)
		{
			D3DXCOLOR temp = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			m_pBufferCom->Set_Color(temp);
		}
		else
		{
			D3DXCOLOR temp = D3DXCOLOR(1.f, 0.0f, 0.0f, 1.f);
			m_pBufferCom->Set_Color(temp);
		}
		

		if (m_fDelayTime > 0.2)
		{
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ

			D3DXCOLOR temp = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			m_pBufferCom->Set_Color(temp);
		}
	}

	if (m_eCurState == P_GET_BAD_ITEM)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime > 1.5)
		{
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ
			dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToOriginPos();
		}
	}

	if (m_eCurState == P_THUMBS_UP && !m_bAzazelType)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime < 2.6 && m_fDelayTime >2.5)
		{
			// ����
			Engine::PlayEffect(L"thumbs up.wav", SOUND_EFFECT_ITEM_STOPSUDDEN, 1.f);
		}

		if (m_fDelayTime > 2 )
		{
			m_fFrame += m_fPicNum * fTimeDelta * m_fSpriteSpeed;
		}

		if (m_fFrame > 2)
		{
			m_fFrame = 2;
		}

		// 2�� ���� ����
		if (m_fDelayTime > 4)
		{
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ
			dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToOriginPos();
		}
	}


	if (m_eCurState == P_THUMBS_UP && m_bAzazelType)
	{
		m_fDelayTime += fTimeDelta;

		if (m_fDelayTime < 2.6 && m_fDelayTime >2.5)
		{
			// ����
			Engine::PlayEffect(L"�Ŀ���2.wav", SOUND_EFFECT_ITEM_STOPSUDDEN, 1.f);
			Set_Brim_Cinemachine();
			m_iAzaelStateSet = 0;
			m_fFrame = 0.f;
			m_bRender = false;
		}
	
		// 2�� ���� ����
		if (m_fDelayTime > 4)
		{
			m_bRender = true;
			m_eCurPlayerVer = P_AZAZEL;
			m_eCurState = P_IDLE;
			m_fDelayTime = 0; // ������ �ð� �ʱ�ȭ
			m_fFrame = 0.f;
			m_bKeyBlock = false; // key �Է� Ȱ��ȭ
			dynamic_cast<CDynamicCamera*>(m_pCamera)->OnMoveToOriginPos();
			m_bAzazelType = false;
		}
	}


}

void CPlayer::Check_UnBeatable_Time(const _float& fTimeDelta)
{
	// �������� �����ð� COUNT���� �ʱ�
	if (m_bUnbeatable && m_eCurState != P_DIE)
	{
		m_fUnbeatableTime += fTimeDelta;

		// �����ð� ����
		// cinemachine�� �ƴ϶� �׳� hurt
		// && !m_bMapCinemachine
		if (m_fUnbeatableTime >= m_fUnbeatableTimeFix && !m_bMapCinemachine)
		{
			m_bUnbeatable = false;
			m_fUnbeatableTime = 0;
		}
	}
}


void CPlayer::Free()
{
	if (!m_PlayerBulletList.empty())
	{
		for (auto& iter = m_PlayerBulletList.begin();
			iter != m_PlayerBulletList.end(); )
		{
			Engine::Safe_Release(*iter);
			iter = m_PlayerBulletList.erase(iter);
		}
	}

	if (!m_EpicMarkList.empty())
	{
		for (auto& iter = m_EpicMarkList.begin();
			iter != m_EpicMarkList.end(); )
		{
			Engine::Safe_Release(*iter);
			iter = m_EpicMarkList.erase(iter);
		}
	}

	if (m_pLeg != nullptr)
	{
		Engine::Safe_Release(m_pLeg);
	}
	
	__super::Free();
}