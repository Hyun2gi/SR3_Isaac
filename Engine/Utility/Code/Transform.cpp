#include "..\..\Header\Transform.h"

CTransform::CTransform()
: m_vAngle(0.f, 0.f, 0.f), m_vScale(1.f, 1.f, 1.f)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_vAngle(0.f, 0.f, 0.f), m_vScale(1.f,1.f, 1.f)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs), m_vAngle(rhs.m_vAngle), m_vScale(rhs.m_vScale)
{
	for (size_t i = 0; i < INFO_END; ++i)
		m_vInfo[i] = rhs.m_vInfo[i];

	m_matWorld = rhs.m_matWorld;
}


CTransform::~CTransform()
{
}

HRESULT CTransform::Ready_Transform()
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_int i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	
	return S_OK;
}

_int CTransform::Update_Component(const _float & fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_int i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	// 크기 변환

	for (_int i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	// 회전 변환
	_matrix			matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_int i = 0; i < INFO_POS; ++i)
	{
		for (_int j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	for (_int i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	return 0;
}

void CTransform::Chase_Target(const _vec3 * pTargetPos, const _float & fSpeed, const _float & fTimeDelta)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];

	m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;

	_matrix		matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POS].x, m_vInfo[INFO_POS].y, m_vInfo[INFO_POS].z);

	matRot = *Compute_LookAtTarget(pTargetPos);

	m_matWorld = matScale * matRot * matTrans;

}

const _matrix* Engine::CTransform::Compute_LookAtTarget(const _vec3* pTargetPos)
{
	_vec3	vDir = *pTargetPos - m_vInfo[INFO_POS];

	/*_vec3	vAxis = *D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir);

	_vec3	vUp;
	_matrix	matRot;
	
	float	fDot = D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir), D3DXVec3Normalize(&vUp, &m_vInfo[INFO_POS]));
	float	fAngle = acos(fDot);


	D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);

	return &matRot;*/

	_vec3	vAxis, vUp;
	_matrix matRot;

	return D3DXMatrixRotationAxis(&matRot,
		D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir),
		acos(D3DXVec3Dot(D3DXVec3Normalize(&vDir, &vDir),
						 D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]))));
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform *	pInstance = new CTransform(pGraphicDev);

	if (FAILED(pInstance->Ready_Transform()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Transform Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent * CTransform::Clone()
{
	return new CTransform(*this);
}

void CTransform::Free()
{
	__super::Free();
}
