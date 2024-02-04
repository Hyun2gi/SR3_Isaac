#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CParticleMgr : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr();

public:
	void PlaySound(TCHAR* pSoundKey, CHANNEL_ID eID, _float fVolume);
	void PlayBGM(TCHAR* pSoundKey, _float fVolume);
	void StopSound(CHANNEL_ID eID);
	void StopAll();
	void SetChannelVolume(CHANNEL_ID eID, _float fVolume);

	void	Ready_Sound();

private:
	void LoadSoundFile(); 

private:

private:
	virtual void		Free(void);
};

END