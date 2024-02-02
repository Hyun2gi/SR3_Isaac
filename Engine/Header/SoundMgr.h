#pragma once

#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)
private:
	explicit CSoundMgr();
	virtual ~CSoundMgr();

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
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound; 
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL]; 

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem; 

private:
	virtual void		Free(void);
};

END