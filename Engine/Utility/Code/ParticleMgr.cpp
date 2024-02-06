#include "..\..\Header\ParticleMgr.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}

CParticleMgr::~CParticleMgr()
{
	Free();
}


void CParticleMgr::Free(void)
{

}
