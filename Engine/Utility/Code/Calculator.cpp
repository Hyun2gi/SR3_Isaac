#include "..\..\Header\Calculator.h"

#include "Export_Utility.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3 * pPos,
											const _vec3 * pTerrainVtxPos,
											const _ulong & dwCntX, 
											const _ulong & dwCntZ,
											const _ulong& dwVtxItv)
{

	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fRatioX = _float(pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fRatioZ = _float(pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE	Plane;

	// 오른쪽 위
	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	 // 왼쪽 아래 
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	// Y = (-AX -CZ - D) / B


	return  (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd,
									CTerrainTex * pTerrainBufferCom,
									CTransform * pTerrainTransCom,
									const _ulong& dwCntX,
									const _ulong& dwCntZ)
{
	POINT		ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	_vec3		vMousePos;

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;

	vMousePos.z = 0.f;

	// 투영 -> 뷰스페이스
	_matrix	matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰 스페이스 -> 월드
	_matrix	matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec3	vRayDir, vRayPos;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 -> 로컬
	_matrix		matWorld;
	pTerrainTransCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	const _vec3*		pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	_ulong	dwVtxIdx[3]{};
	_float	fU(0.f), fV(0.f), fDist(0.f);

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			_ulong	dwIndex = i * dwCntX + j;

			// 오른쪽 위

			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + dwCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[1]], 
			&pTerrainVtxPos[dwVtxIdx[0]],
			&pTerrainVtxPos[dwVtxIdx[2]],
			&vRayPos, &vRayDir, 
			&fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[0]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
							0.f,
							pTerrainVtxPos[dwVtxIdx[1]].z + fV * (pTerrainVtxPos[dwVtxIdx[2]].z - pTerrainVtxPos[dwVtxIdx[0]].z));
			}

			// V1 + U(V2 - V1) + V(V3 - V1)

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + dwCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[2]],
				&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&vRayPos, &vRayDir,
				&fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[2]].x + fU * (pTerrainVtxPos[dwVtxIdx[1]].x - pTerrainVtxPos[dwVtxIdx[2]].x),
							 0.f,
							 pTerrainVtxPos[dwVtxIdx[2]].z + fV * (pTerrainVtxPos[dwVtxIdx[0]].z - pTerrainVtxPos[dwVtxIdx[2]].z));
			}
		}
	}



	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CCalculator::Get_Picking_Dir(HWND hWnd, CTransform* pTransform)
{
	POINT		ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	// 뷰포트 -> 투영

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	_vec3		vMousePos;

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;

	vMousePos.z = 0.f;

	// 투영 -> 뷰스페이스
	_matrix	matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰 스페이스 -> 월드
	_matrix	matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec3	vRayDir, vRayPos;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	//넘겨받은 포지션에서 레이의 월드 포지션을 빼 방향벡터를 구해서 반환한다.
	_vec3 vDir = vRayPos - pTransform->m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vDir, &vDir);

	//월드 상의 Ray 방향을 반환한다.
	return vDir;
}

_vec3 CCalculator::Compute_Vill_Angle(CTransform* pSrcTrans, CTransform* pDstTrans)
{
	_vec3 vSrcPos = pSrcTrans->m_vInfo[INFO_POS];
	_vec3 vDstPos = pDstTrans->m_vInfo[INFO_POS];

	_vec3 vDir = vDstPos - vSrcPos;
	D3DXVec3Normalize(&vDir, &vDir);

	_vec3 vVlaue;

	_vec3 vTemp = pSrcTrans->m_vInfo[INFO_RIGHT];
	D3DXVec3Normalize(&vTemp, &vTemp);

	vVlaue.x = D3DXVec3Dot(&vDir, &vTemp);

	vTemp = pSrcTrans->m_vInfo[INFO_UP];
	D3DXVec3Normalize(&vTemp, &vTemp);

	vVlaue.y = D3DXVec3Dot(&vDir, &vTemp);

	vTemp = pSrcTrans->m_vInfo[INFO_LOOK];
	D3DXVec3Normalize(&vTemp, &vTemp);

	vVlaue.z = D3DXVec3Dot(&vDir, &vTemp);

	return vVlaue;
}

bool CCalculator::Compute_Vill_Matrix_X(CTransform* _pTransformCom)
{
	if (!_pTransformCom) return false;

	_matrix	matWorld, matView, matBill;

	_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._22 = matView._22;
	matBill._23 = matView._23;
	matBill._32 = matView._32;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

	return true;
}

//넘겨준 트랜스폼 객체의 월드행렬을 빌보드형식으로 바꿔준다.
bool CCalculator::Compute_Vill_Matrix(CTransform* _pTransformCom)
{
	if (!_pTransformCom) return false;

	_matrix	matWorld, matView, matBill;

	_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));

	return true;
}

CCalculator * CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*		pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Calculator Create Failed");
		return nullptr;
	}
	
	return pInstance;
}

CComponent * CCalculator::Clone(void)
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	CComponent::Free();
}
