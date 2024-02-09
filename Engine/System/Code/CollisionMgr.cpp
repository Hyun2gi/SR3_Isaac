#include "..\..\Header\CollisionMgr.h"
#include "Transform.h"

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

bool CCollisionMgr::Check_Intersect(CTransform* pSrcTrans, CTransform* pDstTrans, _float fItv)
{
	_vec3 vSrcPos = pSrcTrans->m_vInfo[INFO_POS];
	_vec3 vDstPos = pDstTrans->m_vInfo[INFO_POS];

	_vec3 vSrcScale = pSrcTrans->m_vScale;
	_vec3 vDstScale = pDstTrans->m_vScale;

	_vec3 vSrcMin, vSrcMax;
	vSrcMin.x = vSrcPos.x - vSrcScale.x * 0.5f;
	vSrcMin.y = vSrcPos.y - vSrcScale.y * 0.5f;
	vSrcMin.z = vSrcPos.z - vSrcScale.z * 0.5f;
	vSrcMax.x = vSrcPos.x + vSrcScale.x * 0.5f;
	vSrcMax.y = vSrcPos.y + vSrcScale.y * 0.5f;
	vSrcMax.z = vSrcPos.z + vSrcScale.z * 0.5f;

	_vec3 vDstMin, vDstMax;
	vDstMin.x = vDstPos.x - (vDstScale.x * 0.5f + fItv);
	vDstMin.y = vDstPos.y - (vDstScale.y * 0.5f + fItv);
	vDstMin.z = vDstPos.z - (vDstScale.z * 0.5f + fItv);
	vDstMax.x = vDstPos.x + (vDstScale.x * 0.5f + fItv);
	vDstMax.y = vDstPos.y + (vDstScale.y * 0.5f + fItv);
	vDstMax.z = vDstPos.z + (vDstScale.z * 0.5f + fItv);

	if (vSrcMin.x <= vDstMax.x && vSrcMax.x >= vDstMin.x &&
		vSrcMin.y <= vDstMax.y && vSrcMax.y >= vDstMin.y &&
		vSrcMin.z <= vDstMax.z && vSrcMax.z >= vDstMin.z)
	{
		return true;
	}
	else
	{
		return false;
	}

}


void CCollisionMgr::Check_Collision(CTransform* pSrcTrans, CTransform* pDstTrans, _float fItv)
{
	_vec3 vSrcPos = pSrcTrans->m_vInfo[INFO_POS];
	_vec3 vDstPos = pDstTrans->m_vInfo[INFO_POS];

	_vec3 vSrcScale = pSrcTrans->m_vScale;
	_vec3 vDstScale = pDstTrans->m_vScale;

	_float		fWidth = abs(vDstPos.x - vSrcPos.x);
	_float		fHeight = abs(vDstPos.z - vSrcPos.z);

	_vec3 vSrcMin, vSrcMax;
	vSrcMin.x = vSrcPos.x - vSrcScale.x * 0.5f;
	vSrcMin.z = vSrcPos.z - vSrcScale.z * 0.5f;
	vSrcMax.x = vSrcPos.x + vSrcScale.x * 0.5f;
	vSrcMax.z = vSrcPos.z + vSrcScale.z * 0.5f;

	_vec3 vDstMin, vDstMax;
	vDstMin.x = vDstPos.x - vDstScale.x * 0.5f;
	vDstMin.z = vDstPos.z - vDstScale.z * 0.5f;
	vDstMax.x = vDstPos.x + vDstScale.x * 0.5f;
	vDstMax.z = vDstPos.z + vDstScale.z * 0.5f;

	
	if (Check_Intersect(pSrcTrans, pDstTrans))
	{
		//X Ãæµ¹
		if (fWidth > fHeight)
		{
			if (vSrcPos.x > vDstPos.x)
			{
				if (Check_Intersect(pSrcTrans, pDstTrans))
					pSrcTrans->Set_Pos_X(vDstMax.x - vSrcMin.x + fItv);
			}
			else if (vSrcPos.x <= vDstPos.x)
			{
				if (Check_Intersect(pSrcTrans, pDstTrans))
					pSrcTrans->Set_Pos_X(-(vSrcMax.x - vDstMin.x) - fItv);
			}
		}
		else
		{
			if (vSrcPos.z > vDstPos.z)
			{
				pSrcTrans->Set_Pos_Z(vDstMax.z - vSrcMin.z + fItv);
			}
			else if (vSrcPos.z <= vDstPos.z)
			{
				if (Check_Intersect(pSrcTrans, pDstTrans))
					pSrcTrans->Set_Pos_Z(-(vSrcMax.z - vDstMin.z) - fItv);
			}
			
		}

	}



}