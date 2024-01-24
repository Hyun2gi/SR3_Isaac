#include "..\..\Header\TerrainTex.h"

CTerrainTex::CTerrainTex()
{
}

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
	, m_pPos(nullptr)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex & rhs)
	:CVIBuffer(rhs)
	, m_fH(rhs.m_fH)
	, m_iH(rhs.m_iH)
	, m_pPos(rhs.m_pPos)
{
}


CTerrainTex::~CTerrainTex()
{
}

HRESULT CTerrainTex::Ready_Buffer(const _ulong& dwCntX, 
									const _ulong& dwCntZ, 
									const _ulong& dwVtxItv)
{

	m_dwFVF = FVF_TEX;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwVtxSize = sizeof(VTXTEX);
	m_pPos = new _vec3[m_dwVtxCnt];

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32; 
	
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_hFile = CreateFile(L"../Bin/Resource/Texture/Terrain/Height.bmp",
		GENERIC_READ, 0,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		0);

	_ulong dwByte(0);

	ReadFile(m_hFile, &m_fH, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_iH, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

	_ulong*		pPixel = new _ulong[m_iH.biHeight * m_iH.biWidth];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_iH.biHeight * m_iH.biWidth, &dwByte, NULL);

	CloseHandle(m_hFile);
	
	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_ulong	dwIndex(0);

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPosition = 
			{ 
				float(j) * dwVtxItv, 
				(pPixel[dwIndex] & 0x000000ff) / 20.f, 
				float(i) * dwVtxItv };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };
			pVertex[dwIndex].vTexUV = { 
				float(j) / (dwCntX - 1) * 20.f,
				float(i) / (dwCntZ - 1) * 20.f 
			};
		}
	}


	Engine::Safe_Delete_Array(pPixel);

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwTriIdx(0);

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			// 오른쪽 위
			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;
			
			_vec3		vDest, vSrc, vNormal;

			vDest = pVertex[pIndex[dwTriIdx]._1].vPosition - pVertex[pIndex[dwTriIdx]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriIdx]._2].vPosition - pVertex[pIndex[dwTriIdx]._1].vPosition;
			
			D3DXVec3Cross(&vNormal, &vDest, &vSrc);

			pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;
			dwTriIdx++;
						
			// 왼쪽 아래
			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;

			vDest = pVertex[pIndex[dwTriIdx]._1].vPosition - pVertex[pIndex[dwTriIdx]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriIdx]._2].vPosition - pVertex[pIndex[dwTriIdx]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDest, &vSrc);

			pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;

			dwTriIdx++;
		}
	}

	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);


	m_pVB->Unlock();
	m_pIB->Unlock();
	return S_OK;
}

void CTerrainTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}



CTerrainTex * CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ulong & dwCntX, const _ulong & dwCntZ, const _ulong & dwVtxItv)
{
	CTerrainTex *		pInstance = new CTerrainTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
	{
		Safe_Release(pInstance);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent * CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

void CTerrainTex::Free()
{
	if (false == m_bClone)
		Safe_Delete_Array(m_pPos);

	CVIBuffer::Free();
}
