//-----------------------------------------------------------------------------
//!\file player_mgr.h
//!\author Aslan
//!
//!\date 2008-11-30
//! last 2008-11-30
//!
//!\brief 玩家管理器
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Mutex.h"
#include "Singleton.h"
#include "player.h"

class Player;
class ProofPolicy;

//-----------------------------------------------------------------------------
// 玩家管理器
//-----------------------------------------------------------------------------
class PlayerMgr : public Singleton<PlayerMgr>  
{
public:
    //-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
    PlayerMgr();
    ~PlayerMgr();

    //--------------------------------------------------------------------------
	// 初始化，更新和销毁
	//--------------------------------------------------------------------------
    BOOL			Init();
    VOID			Update();
    VOID			Destroy();

	//--------------------------------------------------------------------------
	// 各种Get
	//--------------------------------------------------------------------------
	DWORD			GetCurrVer()		{ return m_dwCurrVer; }
	INT				GetPlayerNum()		{ return m_nPlayerNum; }
	INT				GetPlayerLoging()	{ return m_nPlayerLoging; }
	INT				GetProofResultNum()	{ return m_nProofResultNum; }
	EProofPolicy	GetProofPolicy()	{ return m_ePolicy; }
	ProofPolicy*	GetProofPolicyPtr()	{ return m_pPolicy; }
	bool            GetYYMinSerOn()      {return m_bYYMixSerOn;}
	bool			GetGMLoginOn()		{ return m_bGMLogin;}
	bool			GetWebLoginOn()		 { return m_bWebLoginModeOn; }

	//--------------------------------------------------------------------------
	// 网络包相关
	//--------------------------------------------------------------------------
    LPBYTE			RecvMsg(DWORD& dwSize, INT& nMsgNum, DWORD dwCDIndex);
	VOID			ReturnMsg(LPBYTE pMsg);
	VOID			SendMsg(DWORD dwCDIndex, LPBYTE pMsg, DWORD dwSize);

	VOID			StartSendAllMsg()	{ m_pNetSession->ActiveSend(); }

	//--------------------------------------------------------------------------
	// 踢人
	//--------------------------------------------------------------------------
    VOID			Kick(DWORD dwCDIndex)		{ m_pNetSession->Kick(dwCDIndex); }

	//--------------------------------------------------------------------------
	// ID生成 从1到0xFFFF循环 为了保证迅雷验证策略中的clientid不会出现赋值溢出
	//--------------------------------------------------------------------------
	DWORD			GenerateClientID()				
	{ 
		++m_dwClientIDGen; 
		if(m_dwClientIDGen > 0xFFFF)
		{
			m_dwClientIDGen = 1;
		}
		return m_dwClientIDGen; 
	}
    
	//--------------------------------------------------------------------------
	// 玩家相关
	//--------------------------------------------------------------------------
	VOID			AddPlayerToAll(Player* pPlayer)				{ m_mapAllPlayer.Add(pPlayer->GetClientID(), pPlayer); }
	VOID			AddPlayerToLogining(Player* pPlayer)		{ m_mapLoginingPlayer.Add(pPlayer->GetClientID(), pPlayer); }
	VOID			RemovePlayerFromAll(DWORD dwClientID)		{ m_mapAllPlayer.Erase(dwClientID); }
	VOID			RemovePlayerFromLogining(DWORD dwClientID)	{ m_mapLoginingPlayer.Erase(dwClientID); }
	VOID			PlayerLogout(Player* pPlayer);

	VOID			AddAccount(DWORD dwAccountID, DWORD dwClientID);
	VOID			RemoveAccount(DWORD dwAccountID);
	BOOL			IsAccountExist(DWORD dwAccountID)			{ return m_mapAccountIDClientID.IsExist(dwAccountID); }

	const tagAccountData*		GetCachedAccountData(DWORD dwAccountID)	{	return m_mapAccountData.Peek(dwAccountID);		}
	VOID			CacheAccountName(DWORD dwAccountID, const CHAR* szAccountName);
	VOID			CacheIpAddres( DWORD dwAccountID, DWORD dwIp);
	VOID			CacheGuard( DWORD dwAccountID, BOOL bGuard);
	VOID			EraseCachedAccountData(DWORD dwAccountID);
	VOID			CleanCachedAccountDatas();
	VOID			MapAccountName2AccountID(LPCSTR szAccountName, DWORD dwAccountID);
	DWORD			GetAccountIDByAccountName(LPCSTR szAccountName);
	DWORD			GetChannelNum() { return m_dwChannelNum; }

	//--------------------------------------------------------------------------
	// 验证相关
	//--------------------------------------------------------------------------
	VOID			Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, DWORD dwIP,DWORD dwWorldNameCrc,DWORD dwChannelID, const CHAR *pszMac,LPCSTR szUserID = NULL,LPCSTR szAppKey = NULL, LPCSTR szToken = NULL);

	VOID			CacheMac( DWORD dwAccountID, LPCSTR szMac);
	LPCSTR          RequestTokenID(DWORD dwConnID,string& strTokenID);
    bool            VerifyToken(LPVOID pLoginCmdInfo,DWORD dwCDIndex);


	//--------------------------------------------------------------------------
	// 获取下载地址
	//--------------------------------------------------------------------------
	bool			GetWebLoginInfo(LPVOID pWebLoginInfo,DWORD& dwMsgSize);
	bool			GetRechargeURL(OUT char* pLoginURLBuff ,DWORD& dwLoginURLSize,OUT char* pBuff,DWORD& dwURLSize,DWORD dwCropID);

private:
	//--------------------------------------------------------------------------
	// 初始化配置文件
	//--------------------------------------------------------------------------
	BOOL			InitConfig();
	BOOL			InitOrangeConfig();

	//--------------------------------------------------------------------------
	// 更新
	//--------------------------------------------------------------------------
	VOID			UpdateSession();
	VOID			UpdateProofResult();

	//--------------------------------------------------------------------------
	// 登陆登出回调
	//--------------------------------------------------------------------------
    DWORD			LoginCallBack(LPVOID, LPVOID);
    DWORD			LogoutCallBack(LPVOID);

	//--------------------------------------------------------------------------
	// 验证结果回调
	//--------------------------------------------------------------------------
	VOID			ProofCallBack(INT nRet, tagProofResult* pResult);

private:
    TSFPTrunk<PlayerMgr>		m_Trunk;
    TObjRef<Util>				m_pUtil;

	//--------------------------------------------------------------------------
	// 网络相关
	//--------------------------------------------------------------------------
    TObjRef<XServer>			m_pNetSession;
    INT							m_nPort;

	//--------------------------------------------------------------------------
	// ID生成器
	//--------------------------------------------------------------------------
    DWORD						m_dwClientIDGen;

	//--------------------------------------------------------------------------
	// 版本号和类型
	//--------------------------------------------------------------------------
    DWORD						m_dwCurrVer;

	//--------------------------------------------------------------------------
	// 统计信息
	//--------------------------------------------------------------------------
	volatile INT				m_nPlayerNum;
	volatile INT				m_nPlayerLoging;
	volatile INT				m_nProofResultNum;

	//--------------------------------------------------------------------------
	// 玩家相关
	//--------------------------------------------------------------------------
	TSafeMap<DWORD, Player*>			m_mapAllPlayer;			// 所有的Player,以ClientID 为 key
	TMap<DWORD, Player*>				m_mapLoginingPlayer;	// 正在登录的玩家
	TMap<DWORD, DWORD>					m_mapAccountIDClientID;	// 在线的AccountID和ClientID对应表
	TSafeList<tagProofResultFull*>		m_listProofResult;		// 返回成功的玩家

	TSafeMap<DWORD, tagAccountData*>	m_mapAccountData;		// 所有的AccountID的缓冲数据，包括AccountName，guard，ip 初始化加载，动态更新
	TSafeMap<DWORD, DWORD>				m_mapAccountNameCrc2AccountID;	// accountid -> namecrc	初始化时加载，并且动态添加

	//--------------------------------------------------------------------------
	// 验证方法
	//--------------------------------------------------------------------------
	EProofPolicy						m_ePolicy;				// 验证方式
	ProofPolicy*						m_pPolicy;				// 验证策略

	std::multimap<tstring, DWORD>		m_mapGMvsIP;				//gm指定ip
	BOOL								m_bUseGMAccess;				//是否使用gm地址限制
	bool								m_bGMLogin;					//GM登入免排队开关
	std::set<DWORD>						m_setGMIP;
	bool                                m_bYYMixSerOn;              //是否开启YY混服
	HINSTANCE                           m_hInstYYDLL;               //YY的动态库
	typedef   int (*PtrInitType)();     
	typedef   void (*PtrCleanUpType)(); 
	typedef const char* (*PtrGenRequestTypeEx)(int);  
	typedef int (*PtrVerifyTokenType)(const char*,int);
	typedef int (*PtrHexToStringType)(const char*, char*, int);
	typedef int (*PtrGetAttributeType)(const char*, const char*, char*, int);
	PtrInitType                                m_pYAuth_init;              //YY库初始化
	PtrCleanUpType                             m_pYAuth_cleanUp;           //YY库销毁
	PtrGenRequestTypeEx                        m_YAuth_genRequest;         //YY库获取TokenID的接口   
	PtrVerifyTokenType                         m_pYAuth_verifyToken;       //收到YY的登录信息后，进行校验
	PtrHexToStringType                         m_pYAuth_HexToString;       //转换16进制
	PtrGetAttributeType                        m_pYAuth_GetAttribute;      //获取信息



	//--------------------------------------------------------------------------
	// 上海Web登入方法
	//--------------------------------------------------------------------------
	struct tagWebNoteInfo
	{
		string  strURL;
		string  strRechargeURL;
		string  strLoginURL;
		tstring tstrDescName;
		DWORD   dwCropID;
	};
	VOID InitWebNoteInfo();
	VOID DestroyWebNoteInfo();
	bool								m_bWebLoginModeOn;			//Web登入是否开启
	DWORD								m_dwChannelNum;				//Web渠道个数
	TMap<DWORD,tagWebNoteInfo*>			m_MapURL;					//URL地址	

};

//----------------------------------------------------------------------------
// 收消息
//----------------------------------------------------------------------------
inline LPBYTE PlayerMgr::RecvMsg(DWORD& dwSize, INT& nMsgNum, DWORD dwCDIndex)
{
	return m_pNetSession->Recv(dwCDIndex, dwSize, nMsgNum);
}

//----------------------------------------------------------------------------
// 归还消息
//----------------------------------------------------------------------------
inline VOID PlayerMgr::ReturnMsg(LPBYTE pMsg)
{
	m_pNetSession->FreeRecved(pMsg);
}

//----------------------------------------------------------------------------
// 发送消息
//----------------------------------------------------------------------------
inline VOID PlayerMgr::SendMsg(DWORD dwCDIndex, LPBYTE pMsg, DWORD dwSize)
{
	m_pNetSession->Send(dwCDIndex, pMsg, dwSize);
}

//-----------------------------------------------------------------------------
// 加入一个帐号
//-----------------------------------------------------------------------------
inline VOID PlayerMgr::AddAccount(DWORD dwAccountID, DWORD dwClientID)
{
	if( !P_VALID(dwAccountID) || !P_VALID(dwClientID) )
		return;

	m_mapAccountIDClientID.Add(dwAccountID, dwClientID);
}

//------------------------------------------------------------------------------
// 移除一个帐号
//------------------------------------------------------------------------------
inline VOID PlayerMgr::RemoveAccount(DWORD dwAccountID)
{
	if( !P_VALID(dwAccountID) ) return;

	m_mapAccountIDClientID.Erase(dwAccountID);
}

#define sPlayerMgr PlayerMgr::getSingleton()

