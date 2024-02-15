#pragma once

#include "Base.h"
#include "UI.h"

#include "MapParts.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CMiniMap : public Engine::CUI
{
private:
	explicit CMiniMap(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMiniMap(const CMiniMap& rhs);
	virtual ~CMiniMap();

public:
	void			Set_RoomTypeNow(string strRoomType) { m_strRoomTypeNow = strRoomType; }
	void			Set_NowRoom(_int iStageKey) { m_iNowRoomNum = iStageKey; }
	CMapParts*		Get_MapParts(_int iIndex);

public:
	virtual HRESULT Ready_GameObject()						 override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject()					 override;
	virtual void Render_GameObject()						 override;

private:
	HRESULT			Add_Component();

	void			Create_RoomParts();
	void			Setting_NowRoom();		// ���� ��
	void			Setting_CheckRoom();	// ���� ��
	void			Setting_FrontRoom();	// �� ���� ��

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTransform* m_pPlayerTransformCom;
	Engine::CTexture* m_pTextureCom;

	_bool				m_bRender;

	_float				m_fAnimSpeed = 1.f;
	_float				m_fCurFrame = 0.f;

	_int				m_iStartFrame;
	_int				m_iEndFrame;

	_int				m_iAnimFrameCount;
	_int				m_iMaxFrameCount;

	_int				m_iNowRoomNum; // ���� �� ��ȣ

	string				m_strRoomTypeNow; // ���� �� ����

	_vec3				m_vecOriginLook; // ó�� ���� Look����

	vector<CMapParts*>	m_vecRoomParts;

public:
	static CMiniMap* Create(LPDIRECT3DDEVICE9	pGraphicDev,
		_float fSizeX, _float fSizeY,
		_float fPosX, _float fPosY,
		_int iAnimFrameCount, _int iMaxFrameCount,
		_float fWinCX = WINCX, _float fWinCY = WINCY);

private:
	virtual void Free() override;

};

