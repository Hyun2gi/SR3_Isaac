#include "..\..\Header\VIBuffer.h"



CVIBuffer::CVIBuffer()
	: m_pVB(nullptr), m_pIB(nullptr), m_dwVtxCnt(0), m_dwTriCnt(0), m_dwVtxSize(0), m_dwFVF(0), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_pVB(nullptr), m_pIB(nullptr), m_dwVtxCnt(0), m_dwTriCnt(0), m_dwVtxSize(0), m_dwFVF(0), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs),
	m_pVB(rhs.m_pVB), m_pIB(rhs.m_pIB), 
	m_dwVtxCnt(rhs.m_dwVtxCnt), m_dwTriCnt(rhs.m_dwTriCnt),
	m_dwVtxSize(rhs.m_dwVtxSize), m_dwFVF(rhs.m_dwFVF),
	m_dwIdxSize(rhs.m_dwIdxSize)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
}


CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::Ready_Buffer()
{
	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer
	(m_dwVtxCnt * m_dwVtxSize,	// 생성할 버텍스 버퍼의 크기
		0,			// 버텍스 버퍼의 종류(0인 경우 정적 버퍼, d3dusage_dynamic인 경우 동적 버퍼)
		m_dwFVF,	// 버텍스 속성 옵션
		D3DPOOL_MANAGED, // 정적 버퍼인 managed
		&m_pVB,	// 생성할 버텍스 버퍼 컴 객체
		NULL),	// 공유 설정
		E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateIndexBuffer
	(m_dwTriCnt * m_dwIdxSize,	// 생성할 인덱스 버퍼의 크기
		0,			// 인덱스 버퍼의 종류(0인 경우 정적 버퍼, d3dusage_dynamic인 경우 동적 버퍼)
		m_IdxFmt,	// 인덱스 속성 옵션
		D3DPOOL_MANAGED, // 정적 버퍼인 managed
		&m_pIB,	// 생성할 인덱스 버퍼 컴 객체
		NULL),	// 공유 설정
		E_FAIL);


	return S_OK;
}

void CVIBuffer::Render_Buffer()
{
	// 디바이스 그리기 객체와 버텍스 버퍼를 링크

	// 1. 몇 번 스트림에 버퍼를 전송할 것인가
	// 2. 어떤 대상을 디바이스 장치와 링크시킬 것인가
	// 3. 어디서부터 전달할 것인가
	// 4. 어떤 크기로 전달할 것인가
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

	m_pGraphicDev->SetFVF(m_dwFVF);

	m_pGraphicDev->SetIndices(m_pIB);


	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

	/*1. 삼각형을 그리는 방식
	2. 버텍스의 인덱스 값
	3. 버텍스 버퍼에 있는 정점들 중 누구부터 그릴 것인가
	4. 내가 사용하고자 하는 버텍스의 개수
	5. 인덱스 버퍼에 들어가 있는 값중 몇 번째 부터 그릴 것인가
	6. 그리려는 삼각형 개수*/
	
	// 실제 그리기 함수
	/*1. 삼각형을 그리는 방식
	2. 몇 번째 버텍스부터 그릴 것인가
	3. 그리려는 삼각형 개수*/
	//m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriCnt);

}

void CVIBuffer::Free()
{
	//Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
