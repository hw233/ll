//-----------------------------------------------------------------------------
//!\file loongworld_mgr.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 管理所有连接进来LoongWorld,拥有StreamServer,发送消息，处理LoongWorld发送的包
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Mutex.h"
#include "Singleton.h"
#include <hash_map>


class Player;
class LoongWorld;

class LoongWorldMgr: public Singleton<LoongWorldMgr>
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
    LoongWorldMgr();
    ~LoongWorldMgr();

	//-------------------------------------------------------------------------
	// 初始化，更新和销毁
	//-------------------------------------------------------------------------
    BOOL		Init();
    VOID		Update();
    VOID		Destroy();
	VOID		UpdateWorldState(LoongWorld* pLoongWorld);
	//-------------------------------------------------------------------------
	// 网络消息相关
	//-------------------------------------------------------------------------
	LPBYTE		RecvMsg(DWORD dwID, DWORD& dwMsgSize, INT& nUnRecved)	{ return m_pSession->Recv(dwID, dwMsgSize, nUnRecved); }
	VOID		ReturnMsg(DWORD dwID, LPBYTE pMsg)						{ m_pSession->FreeRecved(dwID, pMsg); }
	VOID		SendMsg(DWORD dwID, LPVOID pMsg, DWORD dwSize)			{ m_pSession->Send(dwID, pMsg, dwSize); }
	VOID		HandleCmd(LPVOID pMsg, DWORD dwSize, DWORD dwParam)		{ m_pMsgCmdMgr->HandleCmd((tagNetCmd*)pMsg, dwSize, dwParam); }

	//-------------------------------------------------------------------------
	// 得到游戏世界
	//-------------------------------------------------------------------------
    LoongWorld* GetLoongWorld(DWORD dwNameCrc)	{ return m_mapLoongWorld.Peek(dwNameCrc); }
	DWORD		GetLoongWorldNum()				{ return m_mapLoongWorld.Size();		}
	//-----------------------------------------------------------------------
	// 玩家相关
	//-----------------------------------------------------------------------
	VOID		AddToWorld(Player* pPlayer, DWORD dwWorldNameCrc);

private:
	//-----------------------------------------------------------------------
	// 网络消息相关
	//-----------------------------------------------------------------------
	VOID		RegisterLoongWorldMsg();
	VOID		UnRegisterLoongWorldMsg();

	//-----------------------------------------------------------------------
	// 登入登出回调
	//-----------------------------------------------------------------------
	DWORD		LoginCallBack(LPBYTE pByte, DWORD dwSize);
	DWORD		LogoutCallBack(DWORD dwParam);

	//-----------------------------------------------------------------------
	// 消息处理
	//-----------------------------------------------------------------------
	DWORD		HandleCreateGuild(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleCreateFamily(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleCreateRole(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleCertification(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleZoneServerStatus(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandlePlayerLogin(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandlePlayerLogout(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandlePlayerWillLogin(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleKickLog(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleForbidAccount(tagNetCmd* pMsg, LoongWorld* pWorld);
	DWORD		HandleWorldClosed(tagNetCmd* pMsg, LoongWorld* pWorld);
	// Jason 2010-4-27 v2.1.0
	DWORD		HandlePlayerPKRunaway(tagNetCmd*pMsg,LoongWorld*pWord);
	DWORD		HandleFatigueTimer(tagNetCmd*pMsg,LoongWorld*pWord);

	// Jason 2010-11-24 v1.2.2
	DWORD		HandleGetNetbarInfo(tagNetCmd * pMsg, LoongWorld * pWorld);

	// 保存玩家手机号
	DWORD		HandleSaveTelNum(tagNetCmd * pMsg, LoongWorld * pWorld);

	// 玩家不存储手机号
	DWORD		HandleNotSaveTelNum(tagNetCmd * pMsg, LoongWorld * pWorld);
	
private:
	TSFPTrunk<LoongWorldMgr>		m_Trunk;
    TObjRef<Util>					m_pUtil;

	TObjRef<StreamServer>			m_pSession;					// 网络底层
	TObjRef<NetCmdMgr>				m_pMsgCmdMgr;				// 网络消息管理器

    TMap<DWORD, LoongWorld*>		m_mapLoongWorld;			// 游戏世界列表

	DWORD							m_dwLoongWorldGoldenCode;	// LoongWorld的金色代码
	DWORD							m_dwLoginServerGoldenCode;	// LoginServer的金色代码
	// Jason 2010-4-27 v2.1.0，没有与游戏世界相关，而是全部的，比较严格吧
	stdext::hash_map<DWORD, DWORD>		m_hmapPlyPKPunishmentState;	// 处于pk惩罚状态的玩家列表
private:
	VOID UpdatePKPunishmentMap(VOID);
public:
	BOOL	IsInPKPunishmentState(DWORD dwAccountID);
};

#define sLoongWorldMgr LoongWorldMgr::getSingleton()


