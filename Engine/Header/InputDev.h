#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)			{ return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	
	{ 	return m_tMouseState.rgbButtons[eMouse]; 	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}

	bool Key_Pressing(_ubyte byKeyID);
	bool Key_Pressing(MOUSEKEYSTATE eMouse);
	bool Key_Down(_ubyte byKeyID);
	bool Key_Down(MOUSEKEYSTATE eMouse);
	bool Key_Down2(MOUSEKEYSTATE eMouse);
	bool Key_Up(_ubyte byKeyID);
	bool Key_Up(MOUSEKEYSTATE eMouse);
	
public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	_byte					m_byKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	_byte					m_byOldKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	
	DIMOUSESTATE			m_tMouseState;	
	DIMOUSESTATE			m_tOldMouseState;	
	DIMOUSESTATE			m_tOldMouseState2;	

public:
	virtual void	Free(void);

};
END
#endif // InputDev_h__
