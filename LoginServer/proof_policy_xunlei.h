#pragma once
#include "proof_policy.h"

//-----------------------------------------------------------------------------
// 该验证策略的数据库
//-----------------------------------------------------------------------------
class ProofPolicyXunLeiDB
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyXunLeiDB() {}
	~ProofPolicyXunLeiDB() {}

	//-------------------------------------------------------------------------
	// 初始化
	//-------------------------------------------------------------------------
	BOOL	Init();

	//-------------------------------------------------------------------------
	// 查询
	//-------------------------------------------------------------------------
	BOOL	QueryAccount(LPCSTR szAccountName, string& strPsd, tagProofResult* pResult);
	BOOL	InsertAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);
	BOOL	UpdateAccout(LPCSTR szAccountName, LPCSTR szPsd, LPCSTR szMiBao, BOOL bGuard);


private:
	DataBase		m_DB;
};
//-----------------------------------------------------------------------------
// xunlei验证策略
//-----------------------------------------------------------------------------
class ProofPolicyXunLei :
	public ProofPolicy
{
public:
	//-------------------------------------------------------------------------
	// CONSTRUCT
	//-------------------------------------------------------------------------
	ProofPolicyXunLei(void): m_Trunk(this), m_nPort(0), m_bTerminateUpdate(FALSE), m_bTerminateConnect(FALSE){}
	~ProofPolicyXunLei(void){}

public:
	//-------------------------------------------------------------------------
	// 初始化和销毁
	//-------------------------------------------------------------------------
	BOOL	Init(PROOFCALLBACK fn);
	VOID	Destroy();

	//-------------------------------------------------------------------------
	// 验证接口
	//-------------------------------------------------------------------------
	VOID	Proof(DWORD dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, DWORD dwIP, DWORD dwWorldNameCrc,DWORD dwChannelID/*混服运营商ID*/,LPCSTR szMac,LPCSTR szUserID = NULL,LPCSTR szAppKey = NULL, LPCSTR szToken = NULL);

	//-------------------------------------------------------------------------
	// 验证服务器连接状态
	//-------------------------------------------------------------------------
	INT		GetProofServerStatus();

	//-------------------------------------------------------------------------
	// 玩家登出事件
	//-------------------------------------------------------------------------
	VOID	SigPlayerLogoutEvent(DWORD dwAccountID){}

private:
	//-------------------------------------------------------------------------
	// 更新
	//-------------------------------------------------------------------------
	VOID	ThreadUpdate();

	//-------------------------------------------------------------------------
	// 连接线程
	//-------------------------------------------------------------------------
	VOID	ThreadConnectServer();

	//-------------------------------------------------------------------------
	// 初始化配置
	//-------------------------------------------------------------------------
	BOOL	InitConfig();

	//-------------------------------------------------------------------------
	// 更新验证列表
	//-------------------------------------------------------------------------
	VOID	UpdateProofList();

	//-------------------------------------------------------------------------
	// 更新消息
	//-------------------------------------------------------------------------
	VOID	UpdateSession();


	//-------------------------------------------------------------------------
	// 处理xunlei验证信息
	//-------------------------------------------------------------------------
	VOID	HandleXunLeiLogin(LPBYTE pRecv);

	//-------------------------------------------------------------------------
	// 处理字符串
	//-------------------------------------------------------------------------
	BOOL DeleteRightSideBlankOfString(string &str);
		
private:
	HINSTANCE					m_hXunLeiDll; //xunlei dll

	TSFPTrunk<ProofPolicyXunLei>	m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<Thread>				m_pThread;
	//TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<StreamTransport>	m_pTransport;

	//TMap<string, DWORD>		m_mapAccountVsClientID;	// 玩家账号和ClientID的对照表

	//-------------------------------------------------------------------------
	// 线程结束标志
	//-------------------------------------------------------------------------
	volatile BOOL				m_bTerminateUpdate;
	volatile BOOL				m_bTerminateConnect;

	//-------------------------------------------------------------------------
	// 连接参数
	//-------------------------------------------------------------------------
	CHAR						m_szIP[X_IP_LEN];		// 连接xunlei认证服务器的IP
	INT							m_nPort;				// 连接xunlei认证服务器的端口
	//-------------------------------------------------------------------------
	// 验证数据
	//-------------------------------------------------------------------------
	struct tagPlayerProofData
	{
		DWORD		dwClientID;
		string		strAccoutName;
		string		strPsd;
		BYTE		byAccountData[100];		//加密包	由迅雷公司提供的登录器产生
		string		strMac;

		tagPlayerProofData(DWORD _dwClientID, LPCSTR szAccoutName, LPCSTR szPsd, LPCSTR szGUID, LPCSTR szMac)
		{
			dwClientID		=	_dwClientID;
			strAccoutName	=	szAccoutName;
			strPsd			=	szPsd;
			memcpy(byAccountData,szGUID,100);
			strMac			=	szMac;
		}
	};

	TSafeList<tagPlayerProofData*>		m_listProofData;		// 验证用户数据

	//--------------------------------------------------------------------------
	// 数据库
	//--------------------------------------------------------------------------
	ProofPolicyXunLeiDB					m_ProofDB;				// 数据库接口
};
