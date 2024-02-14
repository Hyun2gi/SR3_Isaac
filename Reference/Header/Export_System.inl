#include "Export_System.h"
// GraphicDev

HRESULT			Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _ulong& dwSizeX, const _ulong& dwSizeY, CGraphicDev** ppGraphicDev)
{
	return CGraphicDev::GetInstance()->Ready_GraphicDev(hWnd, eMode, dwSizeX, dwSizeY, ppGraphicDev);
}
void			Render_Begin(D3DXCOLOR Color)
{
	CGraphicDev::GetInstance()->Render_Begin(Color);
}
void			Render_End(void)
{
	CGraphicDev::GetInstance()->Render_End();
}

_float		Get_TimeDelta(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Get_TimeDelta(pTimerTag);
}
bool		Set_TimeDeltaScale(const _tchar* pTimerTag, _float fDeltaScale)
{
	return CTimerMgr::GetInstance()->Set_TimeDeltaScale(pTimerTag, fDeltaScale);
}
void		Update_TimeDelta(const _tchar* pTimerTag)
{
	CTimerMgr::GetInstance()->Update_TimeDelta(pTimerTag);
}
HRESULT		Ready_Timer(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Ready_Timer(pTimerTag);
}

// FrameMgr

_bool			IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	return CFrameMgr::GetInstance()->IsPermit_Call(pFrameTag, fTimeDelta);
}
HRESULT			Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	return CFrameMgr::GetInstance()->Ready_Frame(pFrameTag, fCallLimit);
}

// FontMgr
HRESULT		Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontType,
	const _tchar* pFontTag,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	return CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pFontType, pFontTag, iWidth, iHeight, iWeight);
}

void		Render_Font(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font(pFontTag, pString, pPos, Color);
}

_byte	Get_DIKeyState(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Get_DIKeyState(byKeyID);
}
_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Get_DIMouseState(eMouse);
}
_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return CInputDev::GetInstance()->Get_DIMouseMove(eMouseState);
}
inline bool Key_Pressing(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Key_Pressing(byKeyID);
}
inline bool Key_Pressing(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Key_Pressing(eMouse);
}
inline bool Key_Down(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Key_Down(byKeyID);
}
inline bool Key_Down2(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Key_Down2(eMouse);
}
inline bool Key_Down(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Key_Down(eMouse);
}
inline bool Key_Up(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Key_Up(byKeyID);
}
inline bool Key_Up(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Key_Up(eMouse);
}
HRESULT	Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	return CInputDev::GetInstance()->Ready_InputDev(hInst, hWnd);
}
void		Update_InputDev(void)
{
	CInputDev::GetInstance()->Update_InputDev();
}

inline HRESULT Ready_ImGuiMgr(HWND hWnd, LPDIRECT3DDEVICE9 pGraphicDev)
{
	return CImGuiMgr::GetInstance()->Ready_ImGuiMgr(hWnd, pGraphicDev);
}

inline void Update_ImGuiMgr()
{
	CImGuiMgr::GetInstance()->Update_ImGuiMgr();
}

inline void Render_ImGuiMgr()
{
	CImGuiMgr::GetInstance()->Render_ImGuiMgr();
}

inline void PlaySound(TCHAR* pSoundKey, CHANNEL_ID eID, _float fVolume)
{
	CSoundMgr::GetInstance()->PlaySound(pSoundKey, eID, fVolume);
}

inline void PlayEffect(TCHAR* pSoundKey, CHANNEL_ID eID, _float fVolume)
{
	CSoundMgr::GetInstance()->PlayEffect(pSoundKey, eID, fVolume);
}

inline void Ready_Sound()
{
	CSoundMgr::GetInstance()->Ready_Sound();
}

inline void PlayBGM(TCHAR* pSoundKey, _float fVolume)
{
	CSoundMgr::GetInstance()->PlayBGM(pSoundKey, fVolume);
}

inline void StopSound(CHANNEL_ID eID)
{
	CSoundMgr::GetInstance()->StopSound(eID);
}

inline void StopAll()
{
	CSoundMgr::GetInstance()->StopAll();
}

inline void SetChannelVolume(CHANNEL_ID eID, _float fVolume)
{
	CSoundMgr::GetInstance()->SetChannelVolume(eID, fVolume);
}

inline bool Check_Intersect(CTransform* pSrcTrans, CTransform* pDstTrans, _float fItv)
{
	return CCollisionMgr::Check_Intersect(pSrcTrans, pDstTrans, fItv);
}

inline void Check_Collision(CTransform* pSrcTrans, CTransform* pDstTrans, _float fItv)
{
	CCollisionMgr::Check_Collision(pSrcTrans, pDstTrans, fItv);
}

// Destroy
void			Release_System()
{
	CInputDev::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
	CSoundMgr::GetInstance()->DestroyInstance();
}