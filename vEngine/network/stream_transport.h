//-----------------------------------------------------------------------------
//!\file stream_transport.h
//!\author Lyp
//!
//!\date 2003-03-17
//! last 2008-04-17
//!
//!\brief 广域网客户端通讯底层
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum EStreamTransportMode
{
	ESTM_Package = 0,
	ESTM_Stream = 1,
	ESTM_Text = 2
};
#include   <openssl/crypto.h> 
#include   <openssl/x509.h> 
#include   <openssl/pem.h> 
#include   <openssl/ssl.h> 
#include   <openssl/err.h> 
#include   <openssl/bio.h> 

//-----------------------------------------------------------------------------
//! Provides sequenced, reliable, two-way, connection-based virtual package 
//! streams transmission mechanism. Uses TCP for the Internet address family.
//-----------------------------------------------------------------------------
class VENGINE_API StreamTransport
{
public:
	BOOL Init(BOOL bNagle=TRUE, EStreamTransportMode eMode=ESTM_Package,
		BOOL bSSL = FALSE, BOOL bSSLVerify = FALSE, CHAR* pCertName = NULL, CHAR* pKeyName = NULL, CHAR* pCACertName = NULL, CHAR* pKeyPasswd = NULL);
	VOID Destroy();

	//-----------------------------------------------------------------------------
	// 网络基本操作
	//-----------------------------------------------------------------------------
	VOID TryToConnect(LPCSTR szIP, INT nPort=0); // 尝试连接指定地址
	BOOL IsTryingToConnect();	// 是否正在尝试连接
	VOID StopTryingConnect();	// 放弃尝试连接
	VOID Disconnect(); // 断开连接
	BOOL IsConnected() ;//{ return m_bConnected; }

	// 如果返回FALSE,代表没有连接或没有内存空间存放所给的msg
	BOOL Send(LPVOID pMsg, DWORD dwMsgSize);
	// 如果返回NULL表示没有连接或未收到数据
	LPBYTE Recv(DWORD& dwMsgSize);
	// 对应于Recv(),因为内存是内部分配的，所以应该交还内部来释放
	VOID FreeRecved(LPBYTE pMsg);

	BOOL SetPackageSizeEndian(BOOL bBigEndian);	// 设置包模式下SIZE头的字节序

	//-----------------------------------------------------------------------------
	// 信息统计以及工具函数
	//-----------------------------------------------------------------------------
	INT	GetUnsendPackageNum();	// 得到未发送的包数量，可以以此来判断网络负荷
	INT	GetReceivedPackageNum(); // 得到已接受但未被上层处理的包数量
	DWORD GetTotalSendPackNum() { return m_dwSendPackNum; }	// 获取总发包数(含未发出的或发送失败的)
	DWORD GetTotalRecvPackNum() { return m_dwRecvPackNum; }	// 包含总收包数
	DWORD GetBytesFlowOut(){ return m_dwByteOut; } // 得到每秒流出量
	DWORD GetBytesFlowIn(){ return m_dwByteIn; } // 得到每秒流入量

	LPCSTR GetRemoteIPString() { return m_strIP.c_str(); }// 得到对方IP地址字符串
	BOOL GetHostName(std::string& strName);	// 得到本地主机名称
	BOOL GetIPByHostName(DWORD& dwIP, LPCSTR lpHostName);	// 得到指定主机的IP
	BOOL GetLocalLANIP(DWORD& dwIP);	// 得到本机局域网地址
	BOOL GetLocalWANIP(DWORD& dwIP);	// 得到本机广域网地址
	CHAR* IP2String(DWORD dwIP);	// 将真正IP地址转化为IP地址字符串
	DWORD StringIP2IP(CHAR* szIP);	// 将IP地址字符串转化为真正IP地址

	BOOL SwitchTransportMode(EStreamTransportMode eMode); // 文本模式和流模式之间切换，适合HTTP传输


	StreamTransport();
	~StreamTransport();

private:
	TSFPTrunk<StreamTransport>	m_Trunk;
	EStreamTransportMode		m_eTransportMode;	// 传输模式
	BOOL						m_bBigEndian;		// 包头字节序

	MemPool*				m_pMemPool;

	// 发送接收安全缓冲
	MsgQueueTS*				m_pSendMsgQueue;
	MsgQueueTS*				m_pRecvMsgQueue;
	
	// 网络配置
	std::string				m_strIP;
	INT						m_nPort;
	SOCKET					m_Sock;
	BOOL					m_bNagle;

	SSL_CTX*				m_pCtx;
	SSL*					m_pSSL;
	BIO*					m_pBIO;
	BOOL					m_bSSL;
	//CHAR					m_szCertName[256];
	//CHAR					m_szKeyName[256];
	//CHAR					m_szCACertName[256];
	//CHAR					m_szKeyPasswd[256];


	// 内部线程句柄
	HANDLE					m_hConnect;
	HANDLE					m_hSend;
	HANDLE					m_hReceive;

	// 内部线程控制
	volatile BOOL			m_bTerminateConnect;
	volatile BOOL			m_bTerminateSend;
	volatile BOOL			m_bTerminateReceive;
	volatile BOOL			m_bConnected;		// 是否连接上

	//! 每秒流量统计
	volatile DWORD			m_dwLastStatInTime;	// 上次In统计时间
	volatile DWORD			m_dwLastStatOutTime;// 上次Out统计时间
	volatile DWORD			m_dwByteOut;		// 当前输出
	volatile DWORD			m_dwByteIn;			// 当前输入
	volatile DWORD			m_dwByteOutCounter;
	volatile DWORD			m_dwByteInCounter;


	//收发包基本信息(连接开始--连接结束)
	volatile DWORD			m_dwSendPackNum;	// 总发包数(包含发送失败和还未发出去的)
	volatile DWORD			m_dwRecvPackNum;    // 总收包数(包含还未处理的)

	// 内部方法
	VOID Clear();
	BOOL GracefulCloseSocket(SOCKET& socket, INT nRetryTimes);
	BOOL GracefulCloseThread(HANDLE& thread, INT nRetryTime);

	void constructor();

	// 内部线程
	UINT ThreadConnect(LPVOID);	// connect thread function
	UINT ThreadSend(LPVOID);	// send thread function
	UINT ThreadReceive(LPVOID);	// receive thread function
	UINT ThreadStreamReceive(LPVOID);

	UINT ThreadSSLConnect(LPVOID);
	UINT ThreadSSLStreamReceive(LPVOID);
};




} // namespace vEngine {
