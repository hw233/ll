// Enter.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "Enter.h"
#include <IOSTREAM>    
#include "..\WorldDefine\login.h"
#include "..\WorldDefine\selectrole.h"
#include "..\WorldDefine\role_info.h"
#include "..\WorldDefine\chat.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\map_issue.h"
#include "..\WorldDefine\remote_role.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\quest.h"
#include "..\WorldDefine\msg_exchange.h"
#include "..\WorldDefine\ItemDefine.h"
#include "..\WorldDefine\QuestDef.h"
#include "..\WorldDefine\action.h"
//引用STL
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
using namespace std;
#include <afxtempl.h> 
#include "..\Cool3D\Cool3D.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;
#include "..\vEngine\vEngine.h"
using namespace vEngine;

#include "..\WorldBase\WorldBaseDefine.h"
using namespace WorldBase;

#ifdef _DEBUG
#pragma comment(lib,"..\\vsout\\vengine\\debug\\vengine.lib")
#pragma comment(lib,"..\\vsout\\cool3d\\debug\\cool3d.lib")
#pragma comment(lib,"..\\vsout\\worldbase\\debug\\worldbase.lib")
#pragma comment(lib,"..\\vsout\\WorldDefine\\Debug\\WorldDefine.lib")
#else
#pragma comment(lib,"..\\vsout\\vengine\\release\\vengine.lib")
#pragma comment(lib,"..\\vsout\\cool3d\\release\\cool3d.lib")
#pragma comment(lib,"..\\vsout\\worldbase\\release\\worldbase.lib")
#endif

#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "WinMM.lib")



CString Type[10];
int size[10];
int j=0;
int i;
int MAX=12;
CString name;
int CS[14]={IDC_STATIC1,IDC_STATIC2,IDC_STATIC3,IDC_STATIC4,IDC_STATIC5,IDC_STATIC6,IDC_STATIC7,IDC_STATIC8,IDC_STATIC9,IDC_STATIC10,IDC_STATIC11,IDC_STATIC12,IDC_STATIC13,IDC_STATIC14};
int CE[14]={IDC_EDIT1,IDC_EDIT2,IDC_EDIT3,IDC_EDIT4,IDC_EDIT5,IDC_EDIT6,IDC_EDIT7,IDC_EDIT8,IDC_EDIT9,IDC_EDIT10,IDC_EDIT11,IDC_EDIT12,IDC_EDIT13,IDC_EDIT14};

int CS_S[32]={IDC_STATIC_s0,IDC_STATIC_s1,IDC_STATIC_s2,IDC_STATIC_s3,IDC_STATIC_s4,IDC_STATIC_s5,IDC_STATIC_s6,IDC_STATIC_s7,IDC_STATIC_s8,IDC_STATIC_s9,IDC_STATIC_s10,IDC_STATIC_s11,IDC_STATIC_s12,IDC_STATIC_s13,IDC_STATIC_s14,IDC_STATIC_s15,IDC_STATIC_s16,IDC_STATIC_s17,IDC_STATIC_s18,IDC_STATIC_s19,IDC_STATIC_s20,IDC_STATIC_s21,IDC_STATIC_s22,IDC_STATIC_s23,IDC_STATIC_s24,IDC_STATIC_s25,IDC_STATIC_s26,IDC_STATIC_s27,IDC_STATIC_s28,IDC_STATIC_s29,IDC_STATIC_s30,IDC_STATIC_s31};
int CE_S[32]={IDC_EDIT_s0,IDC_EDIT_s1,IDC_EDIT_s2,IDC_EDIT_s3,IDC_EDIT_s4,IDC_EDIT_s5,IDC_EDIT_s6,IDC_EDIT_s7,IDC_EDIT_s8,IDC_EDIT_s9,IDC_EDIT_s10,IDC_EDIT_s11,IDC_EDIT_s12,IDC_EDIT_s13,IDC_EDIT_s14,IDC_EDIT_s15,IDC_EDIT_s16,IDC_EDIT_s17,IDC_EDIT_s18,IDC_EDIT_s19,IDC_EDIT_s20,IDC_EDIT_s21,IDC_EDIT_s22,IDC_EDIT_s23,IDC_EDIT_s24,IDC_EDIT_s25,IDC_EDIT_s26,IDC_EDIT_s27,IDC_EDIT_s28,IDC_EDIT_s29,IDC_EDIT_s30,IDC_EDIT_s31};

/////////////////////////////////////////////////////////////////////////////
// CEnter dialog


CEnter::CEnter(CWnd* pParent /*=NULL*/)
: CDialog(CEnter::IDD, pParent)
{
}


void CEnter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);


}

BEGIN_MESSAGE_MAP(CEnter, CDialog)
	//{{AFX_MSG_MAP(CEnter)
	ON_BN_CLICKED(IDCANCEL2, OnCancel2)
	ON_BN_CLICKED(IDCANCEL, OnTJ)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelchangeCombo4)
	ON_WM_CLOSE()



	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CEnter::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK2, &CEnter::OnBnClickedOk2)
	ON_BN_CLICKED(IDOK3, &CEnter::OnBnClickedOk3)

	ON_BN_CLICKED(IDOK4, &CEnter::OnBnClickedOk4)


	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CEnter::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEnter::OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnter message handlers


TSFPTrunk<CEnter> zm_Trunk;
NetCmdMgr *zm_pCmdMgr=new NetCmdMgr();
StreamTransport *zm_pStream=new StreamTransport();
StreamTransport * zm_pSession=new StreamTransport();
Util *zm_pUtil=new Util();
CTreeCtrl * pTreeCtrl;
void CEnter::OnOK() 
{//如果连接上，显示连接成功的信息，用户点击确定后接着操作
	//初始状态将控件隐藏
	for (int cc=0;cc<14;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	for (int cc=0;cc<32;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE_S[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE_S[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	//注册连接时消息函数
	zm_pCmdMgr->Register("NLS_Response",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::AAWaitResponse),_T("NLS_Response"));
	zm_pCmdMgr->Register("NS_JoinGame", (NETMSGPROC)zm_Trunk.sfp2(&CEnter::NetRecvJoinGame), _T("NS_JoinGame"));
	zm_pCmdMgr->Register("NLS_ProofResult",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::AAWaitProofResult),_T("NLS_ProofResult"));
	zm_pCmdMgr->Register("NS_StopKeyboardWalk",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::ShowResult),_T("NS_StopKeyboardWalk"));
	//开始连接。。。
	zm_pStream->TryToConnect("172.17.1.138", 4200);
	DWORD OldTime=timeGetTime();
	BOOL a=zm_pStream->IsTryingToConnect();
	if(a==1)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(IDC_STATIC1); 	
		Static_time1->SetWindowText(_T("Waiting"));	
		while (!zm_pStream->IsConnected())
		{
			if(timeGetTime()-OldTime>10000)
			{//如果连接超时，显示失败信息
				CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC2); 	
				Static_time2->SetWindowText(_T("超时"));	
				return;
			}	
		}
		if( zm_pStream->IsConnected() )
		{
			tagNLC_Challenge send;
			//发送NLC_Challenge消息
			zm_pStream->Send(&send, sizeof(send));
			//获取消息，并调用相应的函数
			LPBYTE pMsg = NULL;
			DWORD dwMsgSize = 0;
			DWORD OldTime1=timeGetTime();
			pMsg = zm_pStream->Recv(dwMsgSize);
			while( !P_VALID(pMsg) )
			{
				pMsg = zm_pStream->Recv(dwMsgSize);
				if (timeGetTime()-OldTime1>10000)
				{
					CStatic *Static_time3=(CStatic*)GetDlgItem(IDC_STATIC5); 	
					Static_time3->SetWindowText(_T("消息超时！"));	
					zm_pStream->Disconnect();
					return;
				}
			}
			CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC2); 	
			Static_time2->SetWindowText(_T("连接并得到消息"));	
			this->AAWaitResponse((tagNetCmd *)pMsg,0);
		}
	}

}


//-----------------------------------------------------------------------------
// WaitResponse
//-----------------------------------------------------------------------------
VOID CEnter::AAWaitResponse(tagNetCmd *pM, DWORD pPrama)
{
	if( !P_VALID(pM) )
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
		Static_time2->SetWindowText(_T("失败"));	
		return;
	}
	tagNLS_Response * pMsg=(tagNLS_Response *)pM;
	if( pMsg->dwID == zm_pUtil->Crc32("NLS_Response") )
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC3); 	
		Static_time2->SetWindowText(_T("验证开始"));	
		TCHAR ch[20];
		GetDlgItem(IDC_COMBO3)->GetWindowText(ch,20);
		tstring strAccount =((CString)ch).AllocSysString();
		DWORD dwRandCrc = zm_pUtil->Crc32(((CString)ch).AllocSysString());
		tstringstream strPassword;
		strPassword << pMsg->szRandomString;
		strPassword << dwRandCrc;
		tagNLC_Proof send;
		send.dwPsdCrc = zm_pUtil->Crc32(strPassword.str().c_str());
		//设置游戏世界名
		tstring szWorldName = _T("test");
		send.dwWorldNameCrc = zm_pUtil->Crc32(szWorldName.c_str());
		_tcsncpy(send.szUserName, strAccount.c_str(), X_SHORT_NAME);
		send.dwCurVersionID = 0;
		zm_pStream->Send(&send, sizeof(send));
		DWORD OldTime=timeGetTime();
		LPBYTE pMsg = NULL;
		DWORD dwMsgSize = 0;
		pMsg = zm_pStream->Recv(dwMsgSize);
		while( !P_VALID(pMsg) )
		{
			pMsg = zm_pStream->Recv(dwMsgSize);
			if (timeGetTime()-OldTime>10000)
			{	
				CStatic *Static_time3=(CStatic*)GetDlgItem(IDC_STATIC4); 	
				Static_time3->SetWindowText(_T("消息超时！"));	
				zm_pStream->Disconnect();
				return;
			}
		}
		CStatic *Static_time3=(CStatic*)GetDlgItem(IDC_STATIC4); 	
		Static_time3->SetWindowText(_T("得到消息"));	
		this->AAWaitProofResult((tagNetCmd *)pMsg,0);

	}
}
//-----------------------------------------------------------------------------
// 处理NLS_ProofResult函数
//-----------------------------------------------------------------------------
VOID CEnter::AAWaitProofResult(tagNetCmd *pM, DWORD pPrama)
{   
	if( !P_VALID(pM) )
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
		Static_time2->SetWindowText(_T("失败"));	
		return;
	}
	tagNLS_ProofResult * pMsg=(tagNLS_ProofResult *)pM;
	if( pMsg->dwID == zm_pUtil->Crc32("NLS_ProofResult") )
	{
		if( pMsg->dwErrorCode != E_Success )
		{
			zm_pStream->Disconnect();
			CStatic *Static_time1=(CStatic*)GetDlgItem(IDC_STATIC5); 	
			Static_time1->SetWindowText(_T("验证失败"));
			zm_pStream->Disconnect();
			CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
			switch(pMsg->dwErrorCode)
			{
			case E_ProofResult_Account_No_Match:
				Static_time2->SetWindowText(_T("Account_No_Match"));
				break;
			case E_ProofResult_Account_In_Use:
				Static_time2->SetWindowText(_T("Account_In_Use"));
				break;
			case E_ProofResult_Account_Freezed:
				Static_time2->SetWindowText(_T("ProofResult_Account_Freezed"));
				break;
			case E_ProofResult_Wrong_Build_Number:
				Static_time2->SetWindowText(_T("ProofResult_Wrong_Build_Number"));
				break;
			case E_SelectWorld_Server_Full:
				Static_time2->SetWindowText(_T("SelectWorld_Server_Full"));
				break;
			case E_SelectWorld_Server_Suspended:
				Static_time2->SetWindowText(_T("SelectWorld_Server_Suspended"));
				break;
			case E_SelectWorld_Server_Maintenance:
				Static_time2->SetWindowText(_T("SelectWorld_Server_Maintenance"));
				break;
			case E_SelectWorld_Failed:
				Static_time2->SetWindowText(_T("SelectWorld_Failed"));
				break;
			case E_SelectWorld_Short_Time:
				Static_time2->SetWindowText(_T("SelectWorld_Short_Time"));
				break;
			case E_SelectWorld_No_Select:
				Static_time2->SetWindowText(_T("SelectWorld_No_Select"));
				break;
			case E_SelectWorld_GameWorldName_Wrong:
				Static_time2->SetWindowText(_T("SelectWorld_GameWorldName_Wrong"));
				break;
			case E_SystemError:
				Static_time2->SetWindowText(_T("SystemError"));
				break;
			default:
				Static_time2->SetWindowText(_T("SystemError"));
				break;
			}
			return;
		}
		CHAR* szIP = zm_pStream->IP2String(pMsg->dwIP);
		zm_pSession->TryToConnect(szIP, pMsg->dwPort);	
		//与zone服务器建立连接
		DWORD OldTime=timeGetTime();
		while (!zm_pSession->IsConnected())
		{
			if(timeGetTime()-OldTime>10000)
			{
				CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC5); 	
				Static_time2->SetWindowText(_T("区域超时"));	
				zm_pStream->Disconnect();
				return;
			}

		}

		if(zm_pSession->IsConnected())
		{
			CStatic *Static_time1=(CStatic*)GetDlgItem(IDC_STATIC5); 	
			Static_time1->SetWindowText(_T("区域连接"));	
			zm_pStream->Disconnect();						//断开与login服务器的连接
			tagNC_JoinGame joinGameMsg;
			joinGameMsg.dwAccountID =pMsg->dwAccountID;
			joinGameMsg.dwVerifyCode =pMsg->dwVerifyCode;
			zm_pSession->Send(&joinGameMsg,sizeof(joinGameMsg));
			LPBYTE pMsg = NULL;
			DWORD dwMsgSize = 0;
			DWORD OldTime1=timeGetTime();
			pMsg = zm_pSession->Recv(dwMsgSize);
			while( !P_VALID(pMsg) )
			{
				pMsg = zm_pSession->Recv(dwMsgSize);
				if (timeGetTime()-OldTime1>100000)
				{
					CStatic *Static_time3=(CStatic*)GetDlgItem(IDC_STATIC5); 	
					Static_time3->SetWindowText(_T("消息超时！"));	
					zm_pStream->Disconnect();
					zm_pSession->Disconnect();
					return;
				}
			}
			CStatic *Static_time3=(CStatic*)GetDlgItem(IDC_STATIC5); 	
			Static_time3->SetWindowText(_T("连接并得到消息"));	
			this->NetRecvJoinGame((tagNetCmd *)pMsg,0);

		}				

	}
}

// 处理JoinGame消息
VOID CEnter::NetRecvJoinGame(tagNetCmd *pM, DWORD pPrama)
{
	if( !P_VALID(pM) )
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
		Static_time2->SetWindowText(_T("失败"));	
		return;
	}
	tagNS_JoinGame* pMsg=(tagNS_JoinGame*)pM;
	if( pMsg->dwID != zm_pUtil->Crc32("NS_JoinGame") )	
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
		Static_time2->SetWindowText(_T("失败"));	
		return;
	}
	CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC6); 	
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		Static_time2->SetWindowText(_T("成功进入游戏"));
		break;
	case E_JoinGame_AccountOnline: 	
		Static_time2->SetWindowText(_T("JoinGame_AccountOnline"));
		break;
	case E_JoinGame_VerifyCodeError:
		Static_time2->SetWindowText(_T("JoinGame_VerifyCodeError"));
		break;
	}
	if(pMsg->dwErrorCode!=E_Success)
	{
		CStatic *Static_time2=(CStatic*)GetDlgItem(IDC_STATIC4); 	
		Static_time2->SetWindowText(_T("失败"));
	}
	//		CutOffZone();
}

void CEnter::OnTJ() 
{
	//将信息封装成消息，提交服务器端
	{//根据XML结果得出消息每个字段的大小，分配空间，将相应的字段拷贝到内存
		//对前三个字段赋值
		tagNetCmd * p;
		p=(tagNetCmd *)malloc(MAX);	
		p->dwID=(DWORD)zm_pUtil->Crc32((LPCSTR)(zm_pUtil->UnicodeToAnsi(name)));
		wchar_t *WStr =name.GetBuffer();
		size_t len = wcslen(WStr) + 1;
		size_t converted = 0;
		char *CStr;
		CStr=(char*)malloc(len*sizeof(char));
		wcstombs_s(&converted, CStr, len, WStr, _TRUNCATE);
		p->szName=CStr;
		p->dwSize=(DWORD)(MAX);
		//针对不同的消息根据Type类型进行赋值
		for (int k=0,f=0;k<j&&f<i;k++,f++)
		{
			int sizeadd=0;
			for(int pq=0;pq<k;pq++)
			{
				sizeadd=sizeadd+size[pq];			
			}
			//如果类型为DWORD
			if(_tcscmp(Type[k],_T("DWORD"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				//将TCHAR转化为CHAR
				int iLen = 2*wcslen(ch);
				char* buff = new char[iLen+1]; 
				wcstombs(buff,ch,iLen+1);
				DWORD cc=_atoi64(buff);
				*(DWORD *)((char *)p+12+sizeadd)=(DWORD)(cc);
			}
			//如果类型为INT16
			if(_tcscmp(Type[k],_T("INT16"))==0)
			{
				TCHAR ch[16];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				//将TCHAR转化为CHAR
				int iLen = 2*wcslen(ch);
				char* buff = new char[iLen+1]; 
				wcstombs(buff,ch,iLen+1);
				INT16 sx=_atoi64(buff);
				*(INT16 *)((char *)p+12+sizeadd)=sx;
			}
			//如果类型为INT64
			if(_tcscmp(Type[k],_T("INT64"))==0)
			{
				TCHAR ch[64];
				GetDlgItem(CE[f])->GetWindowText(ch,64);
				//将TCHAR转化为CHAR
				int iLen = 2*wcslen(ch);
				char* buff = new char[iLen+1]; 
				wcstombs(buff,ch,iLen+1);
				INT64 sx=_atoi64(buff);
				*(INT64 *)((char *)p+12+sizeadd)=sx;
			}
			//如果类型为BYTE
			if (_tcscmp(Type[k],_T("BYTE"))==0)
			{
				TCHAR ch7[20];
				GetDlgItem(CE[f])->GetWindowText(ch7,20);
				int iLen7 = 2*wcslen(ch7);
				char* buff7 = new char[iLen7+1]; 
				wcstombs(buff7,ch7,iLen7+1);
				BYTE by=(BYTE)*buff7;
				*(BYTE *)((char *)p+12+sizeadd)=by;
			}
			//如果类型为Float
			if(_tcscmp(Type[k],_T("FLOAT"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				int iLen = 2*wcslen(ch);
				char* buff = new char[iLen+1]; 
				wcstombs(buff,ch,iLen+1);
				*(FLOAT *)((char *)p+12+sizeadd)=(FLOAT)atof(buff);
			}
			//如果类型为TCHAR
			if(_tcscmp(Type[k],_T("TCHAR"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);	
				for (int num=0;num<20;num++)
				{
					*(TCHAR *)((char *)p+12+sizeadd+num*2)=*(ch+num);	
				}
			}
			//如果类型为EPreChapStatus
			if(_tcscmp(Type[k],_T("EPreChapStatus"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				CString s;
				s.Format(ch,20);
				EPreChapStatus em=EPCS_OutKungSword;
				if(_tcscmp(s,_T("EPCS_OutKungSword")))
				{em=EPCS_OutKungSword;}
				if(_tcscmp(s,_T("EPCS_InKungEeriness")))
				{em=EPCS_InKungEeriness;}
				if(_tcscmp(s,_T("EPCS_OutKungBow")))
				{em=EPCS_OutKungBow;}
				if(_tcscmp(s,_T("EPCS_InKungSing")))
				{em=EPCS_InKungSing;}
				if(_tcscmp(s,_T("EPCS_End")))
				{em=EPCS_End;}
				*(EPreChapStatus *)((char *)p+12+sizeadd)=em;
			}
			//如果类型为EEquipPos
			if(_tcscmp(Type[k],_T("EEquipPos"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				CString s;
				s.Format(ch,20);
				EEquipPos em=EEP_Head;
				if(_tcscmp(s,_T("EEP_Head")))
				{em=EEP_Head;}
				if(_tcscmp(s,_T("EEP_Face")))
				{em=EEP_Face;}
				if(_tcscmp(s,_T("EEP_Body")))
				{em=EEP_Body;}
				if(_tcscmp(s,_T("EEP_Legs")))
				{em=EEP_Legs;}
				if(_tcscmp(s,_T("EEP_Back")))
				{em=EEP_Back;}
				if(_tcscmp(s,_T("EEP_Wrist")))
				{em=EEP_Wrist;}
				if(_tcscmp(s,_T("EEP_Feet")))
				{em=EEP_Feet;}
				if(_tcscmp(s,_T("EEP_Finger1")))
				{em=EEP_Finger1;}
				if(_tcscmp(s,_T("EEP_Finger2")))
				{em=EEP_Finger2;}
				if(_tcscmp(s,_T("EEP_Waist")))
				{em=EEP_Waist;}
				if(_tcscmp(s,_T("EEP_Neck")))
				{em=EEP_Neck;}
				if(_tcscmp(s,_T("EEP_RightHand")))
				{em=EEP_RightHand;}
				if(_tcscmp(s,_T("EEP_LeftHand")))
				{em=EEP_LeftHand;}
				if(_tcscmp(s,_T("EEP_FashionHead")))
				{em=EEP_FashionHead;}
				if(_tcscmp(s,_T("EEP_FashionFace")))
				{em=EEP_FashionFace;}
				if(_tcscmp(s,_T("EEP_FashionBody")))
				{em=EEP_FashionBody;}
				if(_tcscmp(s,_T("EEP_FashionLegs")))
				{em=EEP_FashionLegs;}
				if(_tcscmp(s,_T("EEP_FashionBack")))
				{em=EEP_FashionBack;}
				if(_tcscmp(s,_T("EEP_FashionWrist")))
				{em=EEP_FashionWrist;}
				if(_tcscmp(s,_T("EEP_FashionFeet")))
				{em=EEP_FashionFeet;}
				*(EEquipPos *)((char *)p+12+sizeadd)=em;
			}
			//如果类型为EItemConType
			if(_tcscmp(Type[k],_T("EItemConType"))==0)
			{
				TCHAR ch[20];
				GetDlgItem(CE[f])->GetWindowText(ch,20);
				CString s;
				s.Format(ch,20);
				EItemConType em=EICT_Bag;
				if(_tcscmp(s,_T("EICT_Null")))
				{em=EICT_Null;}
				if(_tcscmp(s,_T("EICT_Bag")))
				{em=EICT_Bag;}
				if(_tcscmp(s,_T("EICT_Quest")))
				{em=EICT_Quest;}
				if(_tcscmp(s,_T("EICT_Baibao")))
				{em=EICT_Baibao;}
				if(_tcscmp(s,_T("EICT_RoleWare")))
				{em=EICT_RoleWare;}
				if(_tcscmp(s,_T("EICT_Equip")))
				{em=EICT_Equip;}
				if(_tcscmp(s,_T("EICT_Shop")))
				{em=EICT_Shop;}
				if(_tcscmp(s,_T("EICT_Ground")))
				{em=EICT_Ground;}
				*(EItemConType *)((char *)p+12+sizeadd)=em;
			}
			//如果类型为tagAvatarAtt
			if(_tcscmp(Type[k],_T("tagAvatarAtt"))==0)
			{
				tagAvatarAtt att;
				f++;
				TCHAR ch1[20];
				GetDlgItem(CE[f])->GetWindowText(ch1,20);	
				//将TCHAR转化为CHAR
				int iLen1 = 2*wcslen(ch1);
				char* buff1 = new char[iLen1+1]; 
				wcstombs(buff1,ch1,iLen1+1);
				DWORD cc1=_atoi64(buff1);
				att.wHairMdlID=cc1;
				f++;
				TCHAR ch2[20];
				GetDlgItem(CE[f])->GetWindowText(ch2,20);	
				//将TCHAR转化为CHAR
				int iLen2 = 2*wcslen(ch2);
				char* buff2 = new char[iLen2+1]; 
				wcstombs(buff2,ch2,iLen2+1);
				DWORD cc2=_atoi64(buff2);
				att.wHairTexID=cc2;
				f++;
				TCHAR ch3[20];
				GetDlgItem(CE[f])->GetWindowText(ch3,20);	
				//将TCHAR转化为CHAR
				int iLen3 = 2*wcslen(ch3);
				char* buff3 = new char[iLen3+1]; 
				wcstombs(buff3,ch3,iLen3+1);
				DWORD cc3=_atoi64(buff3);
				att.wFaceMdlID=cc3;
				f++;
				TCHAR ch4[20];
				GetDlgItem(CE[f])->GetWindowText(ch4,20);	
				//将TCHAR转化为CHAR
				int iLen4 = 2*wcslen(ch4);
				char* buff4 = new char[iLen4+1]; 
				wcstombs(buff4,ch4,iLen4+1);
				DWORD cc4=_atoi64(buff4);
				att.wFaceDetailTexID=cc4;
				f++;
				TCHAR ch5[20];
				GetDlgItem(CE[f])->GetWindowText(ch5,20);	
				//将TCHAR转化为CHAR
				int iLen5 = 2*wcslen(ch5);
				char* buff5 = new char[iLen5+1]; 
				wcstombs(buff5,ch5,iLen5+1);
				DWORD cc5=_atoi64(buff5);
				att.wDressMdlID=cc5;
				f++;
				TCHAR ch6[20];
				GetDlgItem(CE[f])->GetWindowText(ch6,20);	
				int iLen6 = 2*wcslen(ch6);
				char* buff6 = new char[iLen6+1];  
				wcstombs(buff6,ch6,iLen6+1);
				BYTE by=(BYTE)*buff6;
				att.bySex=by;
				f++;
				TCHAR ch7[20];
				GetDlgItem(CE[f])->GetWindowText(ch7,20);
				int iLen7 = 2*wcslen(ch7);
				char* buff7 = new char[iLen7+1]; 
				wcstombs(buff7,ch7,iLen7+1);
				BYTE by1=(BYTE)*buff7;
				att.byReserved=by1;
				*(tagAvatarAtt *)((char *)p+12+sizeadd)=att;
			}
			//如果类型为Vector3
			if(_tcscmp(_T("Vector3"),Type[k])==0)
			{
				Vector3 VC;
				f++;
				TCHAR ch1[20];
				GetDlgItem(CE[f])->GetWindowText(ch1,20);
				int iLen1 = 2*wcslen(ch1);
				char* buff1 = new char[iLen1+1]; 
				wcstombs(buff1,ch1,iLen1+1);
				(FLOAT)VC.x=(FLOAT)atof(buff1);
				f++;
				TCHAR ch2[20];
				GetDlgItem(CE[f])->GetWindowText(ch2,20);
				int iLen2 = 2*wcslen(ch2);
				char* buff2 = new char[iLen2+1]; 
				wcstombs(buff2,ch2,iLen2+1);
				(FLOAT)VC.y=(FLOAT)atof(buff2);
				f++;
				TCHAR ch3[20];
				GetDlgItem(CE[f])->GetWindowText(ch3,20);
				int iLen3 = 2*wcslen(ch3);
				char* buff3 = new char[iLen3+1]; 
				wcstombs(buff3,ch3,iLen3+1);
				(FLOAT)VC.z=(FLOAT)atof(buff3);
				*(Vector3 *)((char *)p+12+sizeadd)=VC;
			}
		}


		// 调试代码

		//tagNC_Unequip *asdsd=(tagNC_Unequip *)p;



		zm_pSession->Send(p,(DWORD)(MAX));
		DWORD OldTime=timeGetTime();
		LPBYTE pMsg = NULL;
		DWORD dwMsgSize = 0;
		pMsg = zm_pSession->Recv(dwMsgSize);
		//接收消息
		while( !P_VALID(pMsg) )
		{
			pMsg = zm_pSession->Recv(dwMsgSize);
			if(timeGetTime()-OldTime>1000)
			{
				((CStatic*)GetDlgItem(CS_S[0]))->SetWindowText(_T("接收消息超时"));
				return;
			}
		}
		free(p);
		free(CStr);
		this->ShowResult((tagNetCmd *)pMsg,0);
	}
	//返回成功与否信息
}

//显示结果
VOID CEnter::ShowResult(tagNetCmd * pMsg, DWORD pPrama)
{   //调试 
	//tagNS_RoleGetID * testtt=(tagNS_RoleGetID *)pMsg;
	//初始状态将控件隐藏
	if( !P_VALID(pMsg) )
	{
		((CStatic*)GetDlgItem(CS_S[0]))->SetWindowText(_T("接收消息失败"));
		return;
	}
	CString types[10];
	int sizes[10];
	int a=0;
	int b=0;
	int maxs=12;
	for (int cc=0;cc<32;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE_S[cc])->ShowWindow(SW_HIDE);
	}
	//读取存储消息格式的XML文档
	vEngine::XmlDocument doc;    
	doc.LoadFile(_T("tts.xml"));   
	XmlElement *root = doc.RootElement();    
	//读取根节点
	//const char * a=root->Value();
	//读取要输入的消息类型	
	DWORD ch;
	//从消息中获取名字
	ch=pMsg->dwID;
	if (ch!=NULL)
	{
		XmlNode *node;
		//在XML文档中找到要显示的消息	
		node=root->FirstChild();
		CString cc= node->ToElement()->Attribute("name");
		LPCSTR aas=zm_pUtil->UnicodeToAnsi(cc);
		DWORD dd=zm_pUtil->Crc32(aas);
		while(pMsg->dwID!=dd&&node->NextSiblingElement()!=NULL)    
		{ node = node->NextSiblingElement();
		cc= node->ToElement()->Attribute("name");
		aas=zm_pUtil->UnicodeToAnsi(cc);
		dd=zm_pUtil->Crc32(aas);
		}
		if (pMsg->dwID==dd)
		{//读取孩子并显示
			name=((CString)node->ToElement()->Attribute("name")).AllocSysString();
			XmlNode *child = node->FirstChild();    
			const char * ca=child->Value();		
			while(child)    
			{ types[b]=((CString)child->ToElement()->Attribute("type")).AllocSysString();
			if (_tcscmp(_T("Vector3"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
			{
				CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
				Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
				a++;
				CStatic *Static_time2=(CStatic*)GetDlgItem(CS_S[a]); 
				Static_time2->SetWindowText(_T("X"));

				GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
				a++;
				CStatic *Static_time3=(CStatic*)GetDlgItem(CS_S[a]); 
				Static_time3->SetWindowText(_T("Y"));

				GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
				a++;
				CStatic *Static_time4=(CStatic*)GetDlgItem(CS_S[a]); 
				Static_time4->SetWindowText(_T("Z"));

				GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);

				maxs=maxs+sizeof(Vector3);
				sizes[b]=sizeof(Vector3);
			}
			else{
				if (_tcscmp(_T("tagRoleData"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
				{
					CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
					a++;
					CStatic *Static_time2=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time2->SetWindowText(_T("玩家ID"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time3=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time3->SetWindowText(_T("名称CRC"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time4=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time4->SetWindowText(_T("名称"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time40=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time40->SetWindowText(_T("所在的地图的ID"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time5=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time5->SetWindowText(_T("位置X"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time6=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time6->SetWindowText(_T("位置Y"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time7=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time7->SetWindowText(_T("位置Z"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time8=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time8->SetWindowText(_T("朝向X"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time9=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time9->SetWindowText(_T("朝向Y"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time10=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time10->SetWindowText(_T("朝向Z"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_发型"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_发色"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_面部五官"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_面部细节"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_服装样式"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_性别"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_保留字段"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time12=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time12->SetWindowText(_T("当前人物属性"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time13=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time13->SetWindowText(_T("玩家投放到各个一级属性中的值"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time14=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time14->SetWindowText(_T("职业"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time15=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time15->SetWindowText(_T("扩展职业"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time16=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time16->SetWindowText(_T("信用度"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time17=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time17->SetWindowText(_T("身份"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time18=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time18->SetWindowText(_T("会员积分"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time19=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time19->SetWindowText(_T("背包大小"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time20=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time20->SetWindowText(_T("账号仓库大小"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time21=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time21->SetWindowText(_T("技能个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time22=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time22->SetWindowText(_T("状态个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time23=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time23->SetWindowText(_T("天资列表"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time24=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time24->SetWindowText(_T("称号个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time25=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time25->SetWindowText(_T("物品个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time26=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time26->SetWindowText(_T("装备个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time27=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time27->SetWindowText(_T("好友个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time28=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time28->SetWindowText(_T("仇敌个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time29=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time29->SetWindowText(_T("任务列表个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					a++;
					CStatic *Static_time30=(CStatic*)GetDlgItem(CS_S[a]); 
					Static_time30->SetWindowText(_T("已完成的任务个数"));
					GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
					maxs=maxs+sizeof(tagRoleData);
					sizes[b]=sizeof(tagRoleData);
				}
				else{
					if (_tcscmp(_T("tagRemoteRoleData"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
						a++;
						CStatic *Static_time2=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time2->SetWindowText(_T("玩家ID"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						CStatic *Static_time3=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time3->SetWindowText(_T("所在地图ID"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						CStatic *Static_time4=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time4->SetWindowText(_T("位置X"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;					
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("位置Y"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("位置Z"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						CStatic *Static_time5=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time5->SetWindowText(_T("朝向X"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;					
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("朝向Y"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("朝向Z"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;					
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_发型"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_发色"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_面部五官"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_面部细节"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_服装样式"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_性别"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						((CStatic*)GetDlgItem(CS_S[a]))->SetWindowText(_T("外观_保留字段"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						a++;
						CStatic *Static_time7=(CStatic*)GetDlgItem(CS_S[a]); 
						Static_time7->SetWindowText(_T("远程玩家基本属性"));
						GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
						maxs=maxs+sizeof(tagRemoteRoleData);
						sizes[b]=sizeof(tagRemoteRoleData);
					}
					else{
						if(_tcscmp(_T("DWORD[]"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
						{
							CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
							Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());

							for(int jb=0;jb<QUEST_MAX_COUNT;jb++)
							{   a++;
							char * js;
							itoa(jb,js,10);
							CStatic *Static_time2=(CStatic*)GetDlgItem(CS_S[a]); 
							Static_time2->SetWindowText(((CString)js).AllocSysString());
							GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
							sizes[b]=sizeof(DWORD);
							maxs=maxs+sizeof(DWORD);
							}
						}
						else
						{
							if(_tcscmp(_T("tagQuestDoneSave"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
								Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
								a++;
								CStatic *Static_time2=(CStatic*)GetDlgItem(CS_S[a]); 
								Static_time2->SetWindowText(_T("任务ID"));
								GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
								a++;
								CStatic *Static_time3=(CStatic*)GetDlgItem(CS_S[a]); 
								Static_time3->SetWindowText(_T("接受任务的时间"));
								GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
								a++;
								CStatic *Static_time4=(CStatic*)GetDlgItem(CS_S[a]); 
								Static_time4->SetWindowText(_T("完成的任务次数"));
								GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
							}
							else
							{types[b]=((CString)child->ToElement()->Attribute("type")).AllocSysString();
							CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[a]); 
							Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
							GetDlgItem(CE_S[a])->ShowWindow(SW_SHOW);
							if (_tcscmp(_T("DWORD"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EPreChapStatus"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EEquipPos"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EItemConType"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(DWORD);
								maxs=maxs+sizeof(DWORD);
							}
							if (_tcscmp(_T("FLOAT"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(FLOAT);
								maxs=maxs+sizeof(FLOAT);
							}
							if (_tcscmp(_T("INT64"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(INT64);
								maxs=maxs+sizeof(INT64);		
							}
							if (_tcscmp(_T("INT16"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(INT16);
								maxs=maxs+sizeof(INT16);		
							}
							if (_tcscmp(_T("TCHAR"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(TCHAR[X_SHORT_NAME]);
								maxs=maxs+sizeof(TCHAR[X_SHORT_NAME]);		
							}
							if (_tcscmp(_T("BYTE"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(BYTE);
								maxs=maxs+sizeof(BYTE);		
							}
							if (_tcscmp(_T("INT"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(INT);
								maxs=maxs+sizeof(INT);		
							}
							if (_tcscmp(_T("BOOL"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(BOOL);
								maxs=maxs+sizeof(BOOL);		
							}
							if (_tcscmp(_T("INT32"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(INT32);
								maxs=maxs+sizeof(INT32);		
							}
							if (_tcscmp(_T("BYTE[]"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								sizes[b]=sizeof(BYTE)*SIZE_EQUIP;
								maxs=maxs+sizeof(BYTE)*SIZE_EQUIP;		
							}
						 }
						}
					}
				}
			}
			a++;
			b++;
			child = child->NextSiblingElement();    
			}    
			//分析消息结构，将对应字段赋值给对应的文本框
			tagNetCmd *p=pMsg;	
			for(int j=0,i=0;j<b&&i<a;i++,j++)
			{   int sizeadd=0;
			for (int pq=0;pq<j;pq++)
			{
				sizeadd=sizeadd+sizes[pq];
			}
			//类型为DWORD
			if(_tcscmp(types[j],_T("DWORD"))==0)
			{
				DWORD ch;
				ch=*(DWORD *)((char *)p+12+sizeadd);
				char  buff[64];   
				_i64toa(ch,buff,10);
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(((CString)buff).AllocSysString());
			}
			//类型为DWORD[]
			if(_tcscmp(types[j],_T("DWORD[]"))==0)
			{
				DWORD ch[QUEST_MAX_COUNT];
				for (int jb=0;jb<QUEST_MAX_COUNT;jb++)
				{
					ch[jb]=*(DWORD *)((char *)p+12+sizeadd+jb*sizeof(DWORD));
					char  buff[64];   
					_i64toa(ch[jb],buff,10);
					CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
					Static_time1->SetWindowText(((CString)buff).AllocSysString());
				}	
			}

			//类型为INT
			if(_tcscmp(types[j],_T("INT"))==0)
			{
				INT ch;
				ch=*(INT *)((char *)p+12+sizeadd);
				char  buff[16];   
				itoa(ch,buff,10);
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(((CString)buff).AllocSysString());
			}
			//类型为INT16
			if(_tcscmp(types[j],_T("INT16"))==0)
		 {
			 INT16 ch;
			 ch=*(INT16 *)((char *)p+12+sizeadd);
			 char  buff[16];   
			 _i64toa(ch,buff,10); 
			 CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time1->SetWindowText(((CString)buff).AllocSysString());
			}
			//类型为INT32
			if(_tcscmp(types[j],_T("INT32"))==0)
		 {
			 INT32 ch;
			 ch=*(INT32 *)((char *)p+12+sizeadd);
			 char  buff[16];   
			 _i64toa(ch,buff,10); 
			 CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time1->SetWindowText(((CString)buff).AllocSysString());
		 }

			//类型为INT64
			if(_tcscmp(types[j],_T("INT64"))==0)
			{
				INT64 ch;
				ch=*(INT64 *)((char *)p+12+sizeadd);
				char  buff[64];   
				_i64toa(ch,buff,10); 
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(((CString)buff).AllocSysString());
			}

			//类型为BYTE
			if(_tcscmp(types[j],_T("BYTE"))==0)
			{
				BYTE * ch;
				ch=(BYTE *)((char *)p+12+sizeadd);
				LPCTSTR ss=zm_pUtil->AnsiToUnicode((LPCSTR)ch);
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(ss);
			}
			//类型为BYTE[]
			if(_tcscmp(types[j],_T("BYTE[]"))==0)
			{
				BYTE ch[SIZE_EQUIP];
				for (int jb=0;jb<SIZE_EQUIP;jb++)
				{
					ch[jb]=*(BYTE *)((char *)p+12+sizeadd+jb*sizeof(BYTE));
				}
				LPCTSTR ss=zm_pUtil->AnsiToUnicode((LPCSTR)ch);
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(ss);
			}
			//类型为FLOAT
			if(_tcscmp(types[j],_T("FLOAT"))==0)
			{
				FLOAT ch;
				ch=*(FLOAT *)((char *)p+12+sizeadd);
				char  buff[16];   
				sprintf(buff,"%f",ch); 
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText((CString(buff)).AllocSysString());
			}
			//如果类型为BOOL型
			if(_tcscmp(_T("BOOL"),types[j])==0)
			{
				BOOL ch;
				ch=*(BOOL *)((char *)p+12+sizeadd);
				if(ch=true)
				{ 
					CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
					Static_time1->SetWindowText(_T("true"));
				}
				else
				{
					CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
					Static_time1->SetWindowText(_T("false"));
				}
			}
			//如果类型为EItemConType型
			if(_tcscmp(_T("EItemConType"),types[j])==0)
		 {
			 EItemConType ch;
			 ch=*(EItemConType *)((char *)p+12+sizeadd);
			 CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
			 switch (ch)
			 {
			 case EICT_Null:
				 Static_time1->SetWindowText(_T("EICT_Null"));
				 break;
			 case EICT_Bag:
				 Static_time1->SetWindowText(_T("EICT_Bag"));
				 break;
			 case EICT_Quest:
				 Static_time1->SetWindowText(_T("EICT_Quest"));
				 break;
			 case EICT_Baibao:
				 Static_time1->SetWindowText(_T("EICT_Baibao"));
				 break;
			 case EICT_RoleWare:
				 Static_time1->SetWindowText(_T("EICT_RoleWare"));
				 break;
			 case EICT_Equip:
				 Static_time1->SetWindowText(_T("EICT_Equip"));
				 break;
			 case EICT_Shop:
				 Static_time1->SetWindowText(_T("EICT_Shop"));
				 break;
			 case EICT_Ground:
				 Static_time1->SetWindowText(_T("EICT_Ground"));
				 break;
			 case EICT_End:
				 Static_time1->SetWindowText(_T("EICT_End"));
				 break;
			 default:
				 Static_time1->SetWindowText(_T("ERROR"));
				 break;
			 }
		 }
			//如果类型为TCHAR
			if(_tcscmp(types[j],_T("TCHAR"))==0)
		 {
			 TCHAR *ch;
			 ch=(TCHAR *)((char *)p+12+sizeadd);
			 CString asd;
			 asd.Format(ch,_tcslen(ch));
			 CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time1->SetWindowText(asd.AllocSysString());
		 }
			//如果类型为Vector3
			if(_tcscmp(_T("Vector3"),types[j])==0)
			{
				Vector3 VC=*(Vector3*)((char *)p+12+sizeadd);
				i++;
				FLOAT ch1=(FLOAT)VC.x;
				CString str1;
				char  buff1[16];   
				sprintf(buff1,"%f",ch); 
				str1=(CString)buff1;
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(str1.AllocSysString());
				i++;
				FLOAT ch2=(FLOAT)VC.y;
				CString str2;
				char  buff2[16];   
				sprintf(buff2,"%f",ch); 
				str2=(CString)buff2;
				CStatic *Static_time2=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time2->SetWindowText(str2.AllocSysString());
				i++;
				FLOAT ch3=(FLOAT)VC.z;
				CString str3;
				char  buff3[16];   
				sprintf(buff3,"%f",ch); 
				str3=(CString)buff3;
				CStatic *Static_time3=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time3->SetWindowText(str3.AllocSysString());
		 }

			//如果类型为tagQuestDoneSave
			if(_tcscmp(_T("tagQuestDoneSave"),types[j])==0)
		 {
			 tagQuestDoneSave VC=*(tagQuestDoneSave*)((char *)p+12+sizeadd);
			 i++;
			 DWORD ch1=(DWORD)VC.dwQuestID;
			 CString str1;
			 char  buff1[16];   
			 _i64toa(ch1,buff1,10);
			 str1=(CString)buff1;
			 CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time1->SetWindowText(str1.AllocSysString());
			 i++;
			 DWORD ch2=(DWORD)VC.dwStartTime;
			 CString str2;
			 char  buff2[16];   
			 _i64toa(ch2,buff2,10);
			 str2=(CString)buff2;
			 CStatic *Static_time2=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time2->SetWindowText(str2.AllocSysString());
			 i++;
			 BYTE ch3=(BYTE)VC.byTimes;
			 LPCTSTR ss=zm_pUtil->AnsiToUnicode((LPCSTR)ch3);
			 CStatic *Static_time3=(CStatic*)GetDlgItem(CE_S[i]); 
			 Static_time3->SetWindowText(ss);
		 }

			//如果类型为tagRoleData
			if(_tcscmp(_T("tagRoleData"),types[j])==0)
			{
				tagRoleData VC=*(tagRoleData*)((char *)p+12+sizeadd);
				i++;
				char  buff1[16];   
				sprintf(buff1,"%d",VC.dwID); 
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(((CString)buff1).AllocSysString());
				i++;
				char  buff2[16];   
				sprintf(buff2,"%d",VC.dwRoleNameCrc); 
				CStatic *Static_time2=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time2->SetWindowText(((CString)buff2).AllocSysString());
				i++;
				CStatic *Static_time3=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time3->SetWindowText((VC.szRoleName));
				i++;
				char  buff3[16];   
				sprintf(buff3,"%d",VC.dwMapID); 
				CStatic *Static_time4=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time4->SetWindowText(((CString)buff3).AllocSysString());
				//对FLOAT[3]的处理
				i++;
				CString str_Posx;
				char  buff4[16];   
				sprintf(buff4,"%f",VC.fPos[0]); 
				str_Posx=(CString)buff4;
				CStatic *Static_time5=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time5->SetWindowText(str_Posx.AllocSysString());
				i++;
				CString str_Posy;
				char  buff5[16];   
				sprintf(buff5,"%f",VC.fPos[1]); 
				str_Posy=(CString)buff5;
				CStatic *Static_time6=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time6->SetWindowText(str_Posy.AllocSysString());
				i++;
				CString str_Posz;
				char  buff6[16];   
				sprintf(buff6,"%f",VC.fPos[2]); 
				str_Posz=(CString)buff6;
				CStatic *Static_time7=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time7->SetWindowText(str_Posz.AllocSysString());
				i++;
				CString str_Posxf;
				char  buff7[16];   
				sprintf(buff7,"%f",VC.fFaceTo[0]); 
				str_Posxf=(CString)buff7;
				CStatic *Static_time8=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time8->SetWindowText(str_Posxf.AllocSysString());
				i++;
				CString str_Posyf;
				char  buff8[16];   
				sprintf(buff8,"%f",VC.fFaceTo[1]); 
				str_Posyf=(CString)buff8;
				CStatic *Static_time9=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time9->SetWindowText(str_Posyf.AllocSysString());
				i++;
				CString str_Poszf;
				char  buff9[16];   
				sprintf(buff9,"%f",VC.fFaceTo[2]); 
				str_Poszf=(CString)buff9;
				CStatic *Static_time10=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time10->SetWindowText(str_Poszf.AllocSysString());
			}
			//如果类型为tagRemoteRoleData
			if(_tcscmp(_T("tagRemoteRoleData"),types[j])==0)
			{
				tagRemoteRoleData VC=*(tagRemoteRoleData*)((char *)p+12+sizeadd);
				i++;
				char  buff1[16];   
				sprintf(buff1,"%d",VC.dwID); 
				CStatic *Static_time1=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time1->SetWindowText(((CString)buff1).AllocSysString());
				i++;
				char  buff3[16];   
				sprintf(buff3,"%d",VC.dwMapID); 
				CStatic *Static_time4=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time4->SetWindowText(((CString)buff3).AllocSysString());
				//对FLOAT[3]的处理
				i++;
				CString str_Posx;
				char  buff4[16];   
				sprintf(buff4,"%f",VC.fPos[0]); 
				str_Posx=(CString)buff4;
				CStatic *Static_time5=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time5->SetWindowText(str_Posx.AllocSysString());
				i++;
				CString str_Posy;
				char  buff5[16];   
				sprintf(buff5,"%f",VC.fPos[1]); 
				str_Posy=(CString)buff5;
				CStatic *Static_time6=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time6->SetWindowText(str_Posy.AllocSysString());
				i++;
				CString str_Posz;
				char  buff6[16];   
				sprintf(buff6,"%f",VC.fPos[2]); 
				str_Posz=(CString)buff6;
				CStatic *Static_time7=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time7->SetWindowText(str_Posz.AllocSysString());
				i++;	
				CString str_Posxf;
				char  buff7[16];   
				sprintf(buff7,"%f",VC.fFaceTo[0]); 
				str_Posxf=(CString)buff7;
				CStatic *Static_time8=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time8->SetWindowText(str_Posxf.AllocSysString());
				i++;
				CString str_Posyf;
				char  buff8[16];   
				sprintf(buff8,"%f",VC.fFaceTo[1]); 
				str_Posyf=(CString)buff8;
				CStatic *Static_time9=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time9->SetWindowText(str_Posyf.AllocSysString());
				i++;
				CString str_Poszf;
				char  buff9[16];   
				sprintf(buff9,"%f",VC.fFaceTo[2]); 
				str_Poszf=(CString)buff9;
				CStatic *Static_time10=(CStatic*)GetDlgItem(CE_S[i]); 
				Static_time10->SetWindowText(str_Poszf.AllocSysString());
				i++;
				char  buff10[16];   
				sprintf(buff10,"%d",VC.Avatar.wHairMdlID); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff10).AllocSysString());
				i++;
				char  buff11[16];   
				sprintf(buff11,"%d",VC.Avatar.wHairTexID); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff11).AllocSysString());
				i++;
				char  buff12[16];   
				sprintf(buff12,"%d",VC.Avatar.wFaceMdlID); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff12).AllocSysString());
				i++;
				char  buff13[16];   
				sprintf(buff13,"%d",VC.Avatar.wFaceDetailTexID); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff13).AllocSysString());
				i++;
				char  buff14[16];   
				sprintf(buff14,"%d",VC.Avatar.wDressMdlID); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff14).AllocSysString());
				i++;
				char  buff15[16];   
				sprintf(buff15,"%d",VC.Avatar.bySex); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff15).AllocSysString());
				i++;
				char  buff16[16];   
				sprintf(buff16,"%d",VC.Avatar.byReserved); 
				((CStatic*)GetDlgItem(CE_S[i]))->SetWindowText(((CString)buff16).AllocSysString());
			}
			}    
		}
	}
}

void CEnter::OnSelchangeCombo4() 
{
	//初始状态将控件隐藏
	for (int cc=0;cc<14;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	for (int cc=0;cc<32;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE_S[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE_S[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	//读取存储消息格式的XML文档
	vEngine::XmlDocument doc;    
	doc.LoadFile(_T("tt.xml"));   
	XmlElement *root = doc.RootElement();    
	//读取根节点
	const char * a=root->Value();
	//读取要输入的消息类型	
	TCHAR ch[20];
	GetDlgItem(IDC_COMBO4)->GetWindowText(ch,20);
	if (ch!=NULL)
	{
		XmlNode *node;
		//在XML文档中找到要输入的消息	
		for(node = root->FirstChild();(_tcscmp(ch,((CString)node->Value()).AllocSysString()))&&node->NextSiblingElement()!=NULL; node = node->NextSiblingElement())    
		{ }
		if (_tcscmp(ch,((CString)node->Value()).AllocSysString())==0)
		{      //读取孩子并显示
			name=((CString)node->ToElement()->Attribute("name")).AllocSysString();	   
			if(node->FirstChild()!=NULL)
			{  
				XmlNode *child = node->FirstChild(); 
				const char * ca=child->Value();
				i=0;
				j=0;
				while(child)    
				{ 
					Type[j]=((CString)child->ToElement()->Attribute("type")).AllocSysString();
					if (_tcscmp(_T("Vector3"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
						Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
						i++;
						CStatic *Static_time2=(CStatic*)GetDlgItem(CS[i]); 
						Static_time2->SetWindowText(_T("X"));
						GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
						i++;
						CStatic *Static_time3=(CStatic*)GetDlgItem(CS[i]); 
						Static_time3->SetWindowText(_T("Y"));
						GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
						i++;
						CStatic *Static_time4=(CStatic*)GetDlgItem(CS[i]); 
						Static_time4->SetWindowText(_T("Z"));
						GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
						size[j]=sizeof(Vector3);
						MAX=MAX+sizeof(Vector3);
					}
					else{
						if (_tcscmp(_T("tagAvatarAtt"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
						{
							CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
							Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
							i++;
							CStatic *Static_time2=(CStatic*)GetDlgItem(CS[i]); 
							Static_time2->SetWindowText(_T("发型"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time3=(CStatic*)GetDlgItem(CS[i]); 
							Static_time3->SetWindowText(_T("发色"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time4=(CStatic*)GetDlgItem(CS[i]); 
							Static_time4->SetWindowText(_T("面部五官"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time5=(CStatic*)GetDlgItem(CS[i]); 
							Static_time5->SetWindowText(_T("面部细节"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time6=(CStatic*)GetDlgItem(CS[i]); 
							Static_time6->SetWindowText(_T("服装样式"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time7=(CStatic*)GetDlgItem(CS[i]); 
							Static_time7->SetWindowText(_T("性别"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							i++;
							CStatic *Static_time8=(CStatic*)GetDlgItem(CS[i]); 
							Static_time8->SetWindowText(_T("保留字段"));
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							size[j]=sizeof(tagAvatarAtt);
							MAX=MAX+sizeof(tagAvatarAtt);
						}
						else{
							Type[j]=((CString)child->ToElement()->Attribute("type")).AllocSysString();				 
							CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
							Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
							GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
							if (_tcscmp(_T("DWORD"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EPreChapStatus"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EEquipPos"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EItemConType"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(DWORD);
								MAX=MAX+sizeof(DWORD);
							}
							if (_tcscmp(_T("FLOAT"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(FLOAT);
								MAX=MAX+sizeof(FLOAT);
							}
							if (_tcscmp(_T("INT64"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(INT64);
								MAX=MAX+sizeof(INT64);				
							}
							if (_tcscmp(_T("INT16"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(INT16);
								MAX=MAX+sizeof(INT16);		
							}
							if (_tcscmp(_T("TCHAR"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(TCHAR[X_SHORT_NAME]);
								MAX=MAX+sizeof(TCHAR[X_SHORT_NAME]);		
							}

							if (_tcscmp(_T("BYTE"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
							{
								size[j]=sizeof(BYTE);
								MAX=MAX+sizeof(BYTE);		
							}
						} 
					}

					i++;
					j++;
					child = child->NextSiblingElement();    
				}    		   	
			}
		}
	}
}


void CEnter::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	exit(0);
	CDialog::OnClose();
}

void CEnter::OnCancel2() 
{
	// TODO: Add your control notification handler code here
	exit(0);	
}

void CEnter::OnBnClickedButton1()
{
	//断开登陆服务器和区域服务器
	zm_pStream->Disconnect();
	zm_pSession->Disconnect();
	if (!zm_pStream->IsConnected()&&!zm_pSession->IsConnected())
	{
		for (int cc=0;cc<14;cc++)
		{
			CStatic *Static_time1=(CStatic*)GetDlgItem(CS[cc]); 
			Static_time1->SetWindowText(((CString)"").AllocSysString());
			GetDlgItem(CE[cc])->ShowWindow(SW_HIDE);
			((CStatic*)GetDlgItem(CE[cc]))->SetWindowText(((CString)"").AllocSysString());
		}
		for (int cc=0;cc<32;cc++)
		{
			CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[cc]); 
			Static_time1->SetWindowText(((CString)"").AllocSysString());
			GetDlgItem(CE_S[cc])->ShowWindow(SW_HIDE);
			((CStatic*)GetDlgItem(CE_S[cc]))->SetWindowText(((CString)"").AllocSysString());
		}
	}
	zm_pCmdMgr->UnRegister("NLS_Response",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::AAWaitResponse));
	zm_pCmdMgr->UnRegister("NS_JoinGame", (NETMSGPROC)zm_Trunk.sfp2(&CEnter::NetRecvJoinGame));
	zm_pCmdMgr->UnRegister("NLS_ProofResult",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::AAWaitProofResult));
	zm_pCmdMgr->UnRegister("NS_StopKeyboardWalk",(NETMSGPROC)zm_Trunk.sfp2(&CEnter::ShowResult));
}


void CEnter::OnBnClickedOk2()
{
	// TODO: Add your control notification handler code here
}


//显示消息列表
void CEnter::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	pTreeCtrl=(CTreeCtrl *)GetDlgItem(IDC_TREE1);

	//TreeView_InsertItem(this,   &tv);   

	//读取存储消息格式的XML文档
	vEngine::XmlDocument doc;    
	doc.LoadFile(_T("tt.xml"));   
	XmlElement *root = doc.RootElement();    
	if (root!=NULL)
	{
		XmlNode *node;
		//在XML文档中找到要输入的消息	
		for(node = root->FirstChild();node->NextSiblingElement()!=NULL; node = node->NextSiblingElement())    
		{

			pTreeCtrl->InsertItem(((CString)node->Value()).AllocSysString(),1,1, TVI_ROOT);

		}


	}
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);

}

//选择某个消息类型
void CEnter::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM node=pTreeCtrl->GetSelectedItem(); 
	//读取要输入的消息类型	
	CString MN=pTreeCtrl->GetItemText(node);
	//初始状态将控件隐藏
	for (int cc=0;cc<14;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	for (int cc=0;cc<32;cc++)
	{
		CStatic *Static_time1=(CStatic*)GetDlgItem(CS_S[cc]); 
		Static_time1->SetWindowText(((CString)"").AllocSysString());
		GetDlgItem(CE_S[cc])->ShowWindow(SW_HIDE);
		((CStatic*)GetDlgItem(CE_S[cc]))->SetWindowText(((CString)"").AllocSysString());
	}
	//读取存储消息格式的XML文档
	vEngine::XmlDocument doc;    
	doc.LoadFile(_T("tt.xml"));   
	XmlElement *root = doc.RootElement();    
	//读取根节点
	const char * a=root->Value();
	if (MN!="")
	{
		XmlNode *node;
		//在XML文档中找到要输入的消息	
		for(node = root->FirstChild();(_tcscmp(MN.AllocSysString(),((CString)node->Value()).AllocSysString()))&&node->NextSiblingElement()!=NULL; node = node->NextSiblingElement())    
		{ }
		//if (_tcscmp(ch,((CString)node->ToElement()->Attribute("name")).AllocSysString()))
		if (_tcscmp(MN.AllocSysString(),((CString)node->Value()).AllocSysString())==0)
		{//读取孩子并显示
			name=((CString)node->ToElement()->Attribute("name")).AllocSysString();

			if(node->FirstChild()!=NULL)
			{  XmlNode *child = node->FirstChild(); 
			const char * ca=child->Value();
			i=0;
			j=0;
			while(child)    
			{ Type[j]=((CString)child->ToElement()->Attribute("type")).AllocSysString();
			if (_tcscmp(_T("Vector3"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
			{
				CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
				Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
				i++;
				CStatic *Static_time2=(CStatic*)GetDlgItem(CS[i]); 
				Static_time2->SetWindowText(_T("X"));

				GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
				i++;
				CStatic *Static_time3=(CStatic*)GetDlgItem(CS[i]); 
				Static_time3->SetWindowText(_T("Y"));

				GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
				i++;
				CStatic *Static_time4=(CStatic*)GetDlgItem(CS[i]); 
				Static_time4->SetWindowText(_T("Z"));

				GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
				size[j]=sizeof(Vector3);
				MAX=MAX+sizeof(Vector3);

			}
			else{
				if (_tcscmp(_T("tagAvatarAtt"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
				{
					CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
					Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
					i++;
					CStatic *Static_time2=(CStatic*)GetDlgItem(CS[i]); 
					Static_time2->SetWindowText(_T("发型"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time3=(CStatic*)GetDlgItem(CS[i]); 
					Static_time3->SetWindowText(_T("发色"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time4=(CStatic*)GetDlgItem(CS[i]); 
					Static_time4->SetWindowText(_T("面部五官"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time5=(CStatic*)GetDlgItem(CS[i]); 
					Static_time5->SetWindowText(_T("面部细节"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time6=(CStatic*)GetDlgItem(CS[i]); 
					Static_time6->SetWindowText(_T("服装样式"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time7=(CStatic*)GetDlgItem(CS[i]); 
					Static_time7->SetWindowText(_T("性别"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					i++;
					CStatic *Static_time8=(CStatic*)GetDlgItem(CS[i]); 
					Static_time8->SetWindowText(_T("保留字段"));
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					size[j]=sizeof(tagAvatarAtt);
					MAX=MAX+sizeof(tagAvatarAtt);
				}
				else{
					Type[j]=((CString)child->ToElement()->Attribute("type")).AllocSysString();
					CStatic *Static_time1=(CStatic*)GetDlgItem(CS[i]); 
					Static_time1->SetWindowText(((CString)child->Value()).AllocSysString());
					GetDlgItem(CE[i])->ShowWindow(SW_SHOW);
					if (_tcscmp(_T("DWORD"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EPreChapStatus"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EEquipPos"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0||_tcscmp(_T("EItemConType"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(DWORD);
						MAX=MAX+sizeof(DWORD);
					}
					if (_tcscmp(_T("FLOAT"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(FLOAT);
						MAX=MAX+sizeof(FLOAT);
					}
					if (_tcscmp(_T("INT64"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(INT64);
						MAX=MAX+sizeof(INT64);		
					}

					if (_tcscmp(_T("INT16"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(INT16);
						MAX=MAX+sizeof(INT16);		
					}
					if (_tcscmp(_T("TCHAR"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(TCHAR[X_SHORT_NAME]);
						MAX=MAX+sizeof(TCHAR[X_SHORT_NAME]);		
					}

					if (_tcscmp(_T("BYTE"),((CString)child->ToElement()->Attribute("type")).AllocSysString())==0)
					{
						size[j]=sizeof(BYTE);
						MAX=MAX+sizeof(BYTE);		
					}

				} 

			}

			i++;
			j++;
			child = child->NextSiblingElement();    
			}    
			}
		}
	}
}

//多线程部分

HANDLE   m_hThread;
#include "afxmt.h"
CCriticalSection criticalSection; 
tagNetCmd * TT[6];
int TestNum=1;
int JoinNum=0;
int KK=1;
//多线程控制函数
DWORD  RecvThreadFunc(LPVOID pParam)   
{
	TSFPTrunk<CEnter> zm_Trunk;
	NetCmdMgr *zm_pCmdMgr=new NetCmdMgr();
	StreamTransport *zm_pStream=new StreamTransport();
	StreamTransport * zm_pSession=new StreamTransport();
	Util *zm_pUtil=new Util();
	//开始连接。。。
	criticalSection.Lock(); 
	zm_pStream->TryToConnect("172.17.1.138", 4200);
	DWORD OldTime=timeGetTime();
	BOOL a=zm_pStream->IsTryingToConnect();
	while (!zm_pStream->IsConnected())
	{
		if(timeGetTime()-OldTime>10000)
		{//如果连接超时，显示失败信息	
			return 0;
		}
	}
	if( zm_pStream->IsConnected() )
	{
		tagNLC_Challenge send;
		//发送NLC_Challenge消息
		zm_pStream->Send(&send, sizeof(send));
		//获取消息，并调用相应的函数
		LPBYTE pM= NULL;
		DWORD dwMsgSize = 0;
		DWORD OldTime1=timeGetTime();
		pM = zm_pStream->Recv(dwMsgSize);
		while( !P_VALID(pM) )
		{
			pM = zm_pStream->Recv(dwMsgSize);
			if (timeGetTime()-OldTime1>10000)
			{
				return 0;
			}
		}
		tagNLS_Response * pMsg=(tagNLS_Response *)pM;
		if( pMsg->dwID != zm_pUtil->Crc32("NLS_Response") )
		{
			return 0;
		}
		else
		{
			char buff[6];   
			CString buff1="test";
			TestNum++;
			sprintf(buff,"%d",TestNum);
			CString account=buff1+(CString)buff;
			tstring strAccount =account.AllocSysString();
			DWORD dwRandCrc = zm_pUtil->Crc32(account.AllocSysString());	
			tstringstream strPassword;
			strPassword << pMsg->szRandomString;
			strPassword << dwRandCrc;
			tagNLC_Proof send;
			send.dwPsdCrc = zm_pUtil->Crc32(strPassword.str().c_str());
			//设置游戏世界名
			tstring szWorldName = _T("test");
			send.dwWorldNameCrc = zm_pUtil->Crc32(szWorldName.c_str());
			_tcsncpy(send.szUserName, strAccount.c_str(), X_SHORT_NAME);
			send.dwCurVersionID = 0;
			zm_pStream->Send(&send, sizeof(send));
			DWORD OldTime=timeGetTime();
			LPBYTE pM1 = NULL;
			DWORD dwMsgSize = 0;
			pM1 = zm_pStream->Recv(dwMsgSize);
			while( !P_VALID(pM1) )
			{
				pM1= zm_pStream->Recv(dwMsgSize);
				if (timeGetTime()-OldTime>10000)
				{	
					return 0;
				}
			}
			tagNLS_ProofResult * pMsg=(tagNLS_ProofResult *)pM1;		
			if( pMsg->dwID != zm_pUtil->Crc32("NLS_ProofResult") )
			{
				return 0;
			}	
			else
			{
				if( pMsg->dwErrorCode != E_Success )
				{
					return 0;
				}
				CHAR* szIP = zm_pStream->IP2String(pMsg->dwIP);
				zm_pSession->TryToConnect(szIP, pMsg->dwPort);	
				//与zone服务器建立连接
				DWORD OldTime=timeGetTime();
				while (!zm_pSession->IsConnected())
				{
					if(timeGetTime()-OldTime>10000)
					{
						return 0;
					}
				}
				if(zm_pSession->IsConnected())
				{
					zm_pStream->Disconnect();						//断开与login服务器的连接
					tagNC_JoinGame joinGameMsg;
					joinGameMsg.dwAccountID =pMsg->dwAccountID;
					joinGameMsg.dwVerifyCode =pMsg->dwVerifyCode;
					zm_pSession->Send(&joinGameMsg,sizeof(joinGameMsg));
					LPBYTE pM3 = NULL;
					DWORD dwMsgSize = 0;
					DWORD OldTime1=timeGetTime();
					pM3 = zm_pSession->Recv(dwMsgSize);
					while( !P_VALID(pM3) )
					{
						pM3 = zm_pSession->Recv(dwMsgSize);
						if (timeGetTime()-OldTime1>10000)
						{
							return 0;
						}
					}
					tagNS_JoinGame* pMsg=(tagNS_JoinGame*)pM3;
					if( pMsg->dwID != zm_pUtil->Crc32("NS_JoinGame")||pMsg->dwErrorCode!=E_Success)
					{return 0;}
					else
					{
						JoinNum++;
						criticalSection.Unlock();
						CStdioFile   file;   
						CString   filename="log.txt";   
						if(   !file.Open(_T("log.txt"),   CFile::modeRead|CFile::modeWrite)   )   
						{   
						}   
						file.SeekToEnd();   
						CString login=(CString)"Join Game "+(CString)account+(CString)" successfully!\n";
						file.WriteString(login.AllocSysString());   
						file.Close();   
						while (KK)
						{int ss=rand()%6;
						zm_pSession->Send(TT[ss],sizeof(TT[ss]));
						LPBYTE pMsg = NULL;
						DWORD dwMsgSize = 0;
						DWORD OldTime1=timeGetTime();
						pMsg = zm_pStream->Recv(dwMsgSize);
						while( !P_VALID(pMsg) )
						{
							pMsg = zm_pStream->Recv(dwMsgSize);
							if (timeGetTime()-OldTime1>1000)
							{
								break;
							}
						}
						if( !P_VALID(pMsg) )
						{free(pMsg);
						CStdioFile   file;   
						CString ilename="log.txt";   
						if(   !file.Open(_T("log.txt"),   CFile::modeRead|CFile::modeWrite)   )   
						{   

							break;   
						}   
						file.SeekToEnd();   
						CString login=(CString)"Send message "+(CString)TT[ss]->szName+(CString)" successfully!\n";
						file.WriteString(login.AllocSysString());   
						file.Close();   
						}
						}	
					}
				}
			}
		}
	}

	return 1;

}

//退出多线程测试
void CEnter::OnBnClickedOk3()
{
	// TODO: Add your control notification handler code here
	char buff2[6];   
	sprintf(buff2,"%d",JoinNum);
	CString buff6="加入游戏的人数:";
	CString JoinNumString=buff6+(CString)buff2;
	CStatic * Static_time=(CStatic*)GetDlgItem(IDC_STATIC_J); 
	Static_time->SetWindowText(JoinNumString.AllocSysString());
	KK=0;
	zm_pStream->Disconnect();
	zm_pSession->Disconnect();
	DWORD   code;   
	GetExitCodeThread(m_hThread,&code);  
	if(code==STILL_ACTIVE)   
	{   
		TerminateThread(m_hThread,0);   
		CloseHandle(m_hThread);   
	}
}

//开启多个线程
void CEnter::OnBnClickedOk4()
{
	// TODO: Add your control notification handler code here
	int NUM=0;
	TCHAR ch4[20];
	GetDlgItem(IDC_EDIT_NUM)->GetWindowText(ch4,20);	
	//将TCHAR转化为CHAR
	int iLen4 = 2*wcslen(ch4);
	char* buff4 = new char[iLen4+1]; 
	wcstombs(buff4,ch4,iLen4+1);
	NUM=atoi(buff4);
	tagNC_RoleGetID p;
	TT[0]=(tagNetCmd *)malloc(sizeof(tagNC_RoleGetID));
	*(tagNC_RoleGetID *)TT[0]=p;
	tagNC_RoleGetName p1;
	TT[1]=(tagNetCmd *)malloc(sizeof(tagNC_RoleGetName));
	*(tagNC_RoleGetName *)TT[1]=p1;
	tagNC_EnumRole p2;
	TT[2]=(tagNetCmd *)malloc(sizeof(tagNC_EnumRole));
	*(tagNC_EnumRole *)TT[2]=p2;
	tagNC_CreateRole p3;
	TT[3]=(tagNetCmd *)malloc(sizeof(tagNC_CreateRole));
	*(tagNC_CreateRole *)TT[3]=p3;
	tagNC_DeleteRole p4;
	TT[4]=(tagNetCmd *)malloc(sizeof(tagNC_DeleteRole));
	*(tagNC_DeleteRole *)TT[4]=p4;
	tagNC_SelectRole p5;
	TT[5]=(tagNetCmd *)malloc(sizeof(tagNC_SelectRole));
	*(tagNC_SelectRole *)TT[5]=p5;
	JoinNum=0;
	TestNum=1;
	KK=1;
	for (int mu=0;mu<NUM;mu++)
	{
		m_hThread   =   CreateThread(NULL,20480,(LPTHREAD_START_ROUTINE)RecvThreadFunc,(LPVOID)this,0,NULL);

	}

}



