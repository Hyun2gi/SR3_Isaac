#include "..\..\Header\Renderer.h"

#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}


CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eID, CGameObject * pGameObject)
{
	if (RENDER_END <= eID || nullptr == pGameObject)
		return;
		
	m_RenderGroup[eID].push_back(pGameObject);
	pGameObject->AddRef();

}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev)
{

	if (!m_bIsEnd)
	{
		Render_Priority(pGraphicDev);
		Render_NonAlpha(pGraphicDev);
		Render_Alpha(pGraphicDev);
		Render_Alpha_Sorting(pGraphicDev);
		Render_Particles(pGraphicDev);
		Render_UI(pGraphicDev);
	}
	else
	{
		Render_UI(pGraphicDev);
		Render_Particles(pGraphicDev);
	}
	

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_GameObject();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//(RGB)A		 (R`G`B`)A`
	//(AR, AG, AB) + ((1 - A)R`, (1 - A)G`, (1 - A)B`)

	//pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);

	m_RenderGroup[RENDER_ALPHA].sort([](CGameObject* pDst, CGameObject* pSrc)->bool 
	{
		return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
	});

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_GameObject();

	//pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_Alpha_Sorting(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	int i = 0;
	m_RenderGroup[RENDER_ALPHA_SORTING].sort([&](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			i++;
			return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
		});

	for (auto& iter : m_RenderGroup[RENDER_ALPHA_SORTING])
		iter->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_Particles(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, Engine::CAbstractFactory::FtoDw(0.0f));

	// control the size of the particle relative to distance
	pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, Engine::CAbstractFactory::FtoDw(0.0f));
	pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, Engine::CAbstractFactory::FtoDw(0.0f));
	pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, Engine::CAbstractFactory::FtoDw(1.0f));

	// use alpha from texture
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_RenderGroup[RENDER_PARTICLES].sort([](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
		});

	for (auto& iter : m_RenderGroup[RENDER_PARTICLES])
	{
		CParticleSystem* pParticle = dynamic_cast<CParticleSystem*>(iter);

		_float size = pParticle->Get_Size();

		if (pParticle->Get_UsingTransform())
		{
			_matrix mat = pParticle->Get_World_Matrix();
			pGraphicDev->SetTransform(D3DTS_WORLD, &mat);
		}

		pGraphicDev->SetRenderState(D3DRS_POINTSIZE, Engine::CAbstractFactory::FtoDw(size));

		iter->Render_GameObject();
	}

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	_matrix matView, matProj;
	_matrix matGameView, matGameProj;

	pGraphicDev->GetTransform(D3DTS_VIEW, &matGameView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matGameProj);
	
	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, 800, 600, 0.0f, 1.f);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	m_RenderGroup[RENDER_UI].sort([](auto& iter, auto& iter2)
		{ 
			return dynamic_cast<CUI*>(iter)->Get_RenderIndex() > dynamic_cast<CUI*>(iter2)->Get_RenderIndex();
		});

	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_GameObject();

	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pGraphicDev->SetTransform(D3DTS_VIEW, &matGameView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matGameProj);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
