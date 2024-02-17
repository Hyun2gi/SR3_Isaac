#include "stdafx.h"
#include "..\Header\Logo.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "BackGround.h"
#include "Stage.h"
#include "TestStage.h"
#include "LoadStage.h"
#include "Player.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	Engine::PlayBGM(L"Title.ogg", 1.f);

	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	
	//m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE_1);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	return S_OK;
}

Engine::_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = __super::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			Engine::CScene*		pScene = nullptr;

			pScene = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('0'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 10, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}


		if (GetAsyncKeyState('1'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 1, true);
      
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('2'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 2, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}
		
		if (GetAsyncKeyState('3'))
		{
			Engine::CScene*		pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 3, true);
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}		

		if (GetAsyncKeyState('4'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 4, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('5'))
		{
			Engine::CScene*		pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 5, true);
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}		
		
		if (GetAsyncKeyState('6'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 6, true);
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('7'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 7, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('8'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 8, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('9'))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CLoadStage::Create(m_pGraphicDev, 9, true);

			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState(VK_BACK))
		{
			Engine::CScene* pScene = nullptr;

			pScene = CTestStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}

		if (GetAsyncKeyState('3'))
		{
			// 일단 시스템 메뉴 UI 뜨도록

		}
	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	__super::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	Engine::Render_Font(L"Font_Default", m_pLoading->Get_String(), &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTex", CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Intro/intro_bg.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleTexture", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Ui/Intro/Title_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Shader_Rect", CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Rect.hlsl")), E_FAIL);

	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);
	
	
	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

CLogo * CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo *	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);

		MSG_BOX("Logo Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	__super::Free();
}
