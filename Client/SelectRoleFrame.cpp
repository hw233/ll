//-----------------------------------------------------------------------------
//!\file SelectRoleFrame.cpp
//!\author Lyp
//!
//!\date 2008-02-17
//! last 2008-04-15
//!
//!\brief select role
//!
//!	Copyright (c) 1985-2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "SelectRoleFrame.h"
#include "SeleRoleMgr.h"
#include ".\Player.h"
#include "RoleMgr.h"
#include "HackShield.h"
#include "ServerTime.h"
#include "CursorMgr.h"
#include "LoadingFrame.h"
#include "AudioSys.h"
#include "SaveAccount.h"
#include "SDOAIgw.h"
#include "ClientMain.h"
#include "SaveAccount.h"
#include "gameset.h"
#include "CameraSetData.h"
#include "SaveAccount.h"
#include "WorldFrame.h"

#pragma comment( lib, "Ws2_32.lib" )


#define MOUSE_DOBULEDOWN_TIME 300
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
SelectRoleFrame::SelectRoleFrame():m_Trunk(this)
{
	m_pWndSelct		= 0;
	m_pWndMain		= 0;
	m_pWndDel		= 0 ;
	m_pWndHintInfo	= 0;
	m_pStc_Sele_Name = NULL;
	m_pStc_Sele_Info1 = NULL;
	m_pStc_Sele_Info2 = NULL;
	m_pButtonCreate = 0;
	m_pButtonDelete = 0;
	m_pButtonEenter = 0;
	m_pButtonReturn = 0;
	m_pBtnDelOk		= 0;
	m_pBtnDelCancel	= 0;
	m_pEditSafeCode = 0;
	m_nCurSelRole	= GT_INVALID;
	m_nLastSelRole	= GT_INVALID;
	m_nSelectRoleID = GT_INVALID;
	m_eCurState		= ESRS_EnumRole;
	m_dwMouseDown	= 0;
	m_dwErrorTime	= 0;
	m_pBtnSafeCode	= NULL;
	m_pWndSD		= NULL;
	m_pStcSDHint1	= NULL;
	m_pStcSDHint2	= NULL;
	m_pEdtSafeCode1 = NULL;
	m_pEdtSafeCode2	= NULL;
	m_pBtnSDOK		= NULL;
	m_pBtnSDCancel	= NULL;
	m_eSafeCodeState= ESCS_NULL;
	m_pStcSDHint	= NULL;
	m_nPolicy		= GT_INVALID;
}


//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
SelectRoleFrame::~SelectRoleFrame()
{
	
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pCmdMgr->Register("NS_DeleteRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvDeleteRole), _T("NS_DeleteRole"));
	m_pCmdMgr->Register("NS_EnumRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvEnumRole), _T("NS_EnumRole"));
	m_pCmdMgr->Register("NS_SelectRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSelectRole), _T("NS_SelectRole"));
	m_pCmdMgr->Register("NS_SafeCode", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_SafeCode), _T("NS_SafeCode"));
	m_pCmdMgr->Register("NS_ResetSafeCode", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_ResetSafeCode), _T("NS_ResetSafeCode"));
	m_pCmdMgr->Register("NS_CancelSafeCodeReset", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_CancelSafeCodeReset), _T("NS_CancelSafeCodeReset"));

	m_pFrameMgr->RegisterEventHandle(_T("ResetSafeCodeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnResetSafeCodeEvent));
	m_pFrameMgr->RegisterEventHandle(_T("CancelSafeCodeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnCancelSafeCodeEvent));
	m_pFrameMgr->RegisterEventHandle(_T("SafeCodeIntroductionEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnSafeCodeIntroductionEvent));
	m_pFrameMgr->RegisterEventHandle(_T("DisconnectServer_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnDisconnectServer));
	m_pFrameMgr->RegisterEventHandle(_T("NeedToDeleteRole_MsgBoxEvent"),(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnAcceptDeleteRole));

	m_dwMouseDown = Kernel::Inst()->GetAccumTimeDW();
	m_bLoadXrMap = FALSE;
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::Destroy()
{
	GameFrame::Destroy();

	if( P_VALID(m_pWndSelct) )
		m_pGUI->DestroyWnd(m_pWndSelct);
	if( P_VALID(m_pWndMain) )
		m_pGUI->DestroyWnd(m_pWndMain);
	if( P_VALID(m_pWndDel) )
		m_pGUI->DestroyWnd(m_pWndDel);
	if( P_VALID(m_pWndHintInfo) )
		m_pGUI->DestroyWnd(m_pWndHintInfo);
	if( P_VALID(m_pWndSD) )
		m_pGUI->DestroyWnd(m_pWndSD);
	if( P_VALID(m_pStcSDHint) )
		m_pGUI->DestroyWnd(m_pStcSDHint);

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_DeleteRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvDeleteRole));
	m_pCmdMgr->UnRegister("NS_EnumRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvEnumRole));
	m_pCmdMgr->UnRegister("NS_SelectRole", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvSelectRole));
	m_pCmdMgr->UnRegister("NS_SafeCode", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_SafeCode));
	m_pCmdMgr->UnRegister("NS_ResetSafeCode", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_ResetSafeCode));
	m_pCmdMgr->UnRegister("NS_CancelSafeCodeReset", (NETMSGPROC)m_Trunk.sfp2(&SelectRoleFrame::NetRecvNS_CancelSafeCodeReset));
	
	//--注销游戏事件
	m_pFrameMgr->UnRegisterEventHandler(_T("ResetSafeCodeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnResetSafeCodeEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("CancelSafeCodeEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnCancelSafeCodeEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("SafeCodeIntroductionEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnSafeCodeIntroductionEvent));
	m_pFrameMgr->UnRegisterEventHandler(_T("DisconnectServer_Event"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnDisconnectServer));
	m_pFrameMgr->UnRegisterEventHandler(_T("NeedToDeleteRole_MsgBoxEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&SelectRoleFrame::OnAcceptDeleteRole));

	//--
	if( P_VALID(GetObj("ResetSafeCodeMsgBox")) )
		KillObj("ResetSafeCodeMsgBox");
	if( P_VALID(GetObj("CancelSafeCodeMsgBox")) )
		KillObj("CancelSafeCodeMsgBox");
	if( P_VALID(GetObj("SafeCodeIntroductionMsgBox")) )
		KillObj("SafeCodeIntroductionMsgBox");
	if( P_VALID(GetObj("SafeCodeErrorMsgBox")) )
		KillObj("SafeCodeErrorMsgBox");
	if( P_VALID(GetObj("SafeCodeSuccessMsgBox")) )
		KillObj("SafeCodeSuccessMsgBox");
	if( P_VALID(GetObj("DisconnectServer_MsgBox")) )
		KillObj("DisconnectServer_MsgBox");
	if( P_VALID(GetObj("NeedProof")) )
		KillObj("NeedProof");
	if( P_VALID(GetObj("NotAdult")) )
		KillObj("NotAdult");

	m_bLoadXrMap = FALSE;

	return TRUE;
}



//-----------------------------------------------------------------------------
//! 事件处理
//! 
//-----------------------------------------------------------------------------
DWORD SelectRoleFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		if( pWnd == m_pButtonReturn )
			return ReturnGame();
		else if( pWnd == m_pButtonEenter )//登录
		{
			SeleRoleMgr::Inst()->PlayReadyTrack(m_nCurSelRole);
			m_eCurState = ESRS_SelectedRole;
		}
		else if( pWnd == m_pButtonCreate )//创建角色
		{
			SeleRoleMgr::Inst()->SwitchToInitState();
			return CreateRole();
		}
		else if( pWnd == m_pButtonDelete )//删除角色
			return DeleteRole();
		else if( pWnd == m_pBtnDelOk )//确定删除角色
		{
			SetDeleteRoleUI(TRUE);
			m_pBtnDelOk->SetInvisible(FALSE);
			m_pBtnDelOk->SetEnable(true);

			SendDeleRoleMsg();
		}
		else if( pWnd == m_pBtnDelCancel )//取消删除角色
		{
			m_pEditSafeCode->SetText(_T(""));
			SetDeleteRoleUI(TRUE);
			m_pBtnDelOk->SetInvisible(FALSE);
			m_pBtnDelOk->SetEnable(true);
		}
		else if( pWnd == m_pBtnSafeCode )
		{
			JudeSafeCodeState();
		}
		else if( pWnd == m_pBtnSDOK )//创建安全码
		{
			SendSafeCodeMsg();
		}
		else if( pWnd == m_pBtnSDCancel )
		{
			m_pWndSD->SetInvisible(TRUE);
		}
		else if(pWnd == m_pButtonTWCG)
		{
			if(GameSet::Inst()->Get(EGS_TWCG) == 1 )
			{
				GameSet::Inst()->Set( EGS_TWCG, 0);
				m_pButtonTWCG->SetText(g_StrTable[_T("SkipTWCG")]);
			}
			else
			{
				GameSet::Inst()->Set( EGS_TWCG, 1);
				m_pButtonTWCG->SetText(g_StrTable[_T("PlayTWCG")]);
			}
			GameSet::Inst()->SaveTWCG();
			m_pButtonTWCG->SetRefresh(true);
		}
		break;
	}

	return FALSE;
}



//-----------------------------------------------------------------------------
// ReloadUI
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::ReloadUI()
{
	LoadLoginMode();				 //读取登录验证策略   
	// 创建
	XmlElement element;
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	tstring strPath =  g_strLocalPath + _T("\\ui\\optrole.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	//调整位置
	m_pWndSelct		= m_pGUI->CreateWnd(_T("\\desktop"), &element);

	tstring strPath1 = g_strLocalPath + _T("\\ui\\optbutt.xml");
	m_pWndMain		= m_pGUI->CreateFromFile("VFS_System", strPath1.c_str());

	
	tstring strPath2 = g_strLocalPath + _T("\\ui\\common5.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath2.c_str());
	//调整位置
	std::stringstream streamX, streamY;
	streamX << (pDesktop->GetSize().x/2 - 126);
	streamY << (pDesktop->GetSize().y - 262);
	m_pGUI->ChangeXml(&element, "comm5_win", "LogicPos_x", streamX.str().c_str());
	m_pGUI->ChangeXml(&element, "comm5_win", "LogicPos_y", streamY.str().c_str());
	m_pWndDel = m_pGUI->CreateWnd(_T("\\desktop"), &element);


	tstring strPath3 = g_strLocalPath + _T("\\ui\\hintinfo.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath3.c_str());
	std::stringstream streamX1, streamY1;
	streamX1 << (pDesktop->GetSize().x/2 - 150);
	streamY1 << (pDesktop->GetSize().y/2 - 10);
	m_pGUI->ChangeXml(&element, "hintinfo", "LogicPos_x", streamX1.str().c_str());
	m_pGUI->ChangeXml(&element, "hintinfo", "LogicPos_y", streamY1.str().c_str());
	m_pWndHintInfo	= (GUIStatic*)m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndHintInfo->SetInvisible(TRUE);

	//安全码
	tstring strPath4 = g_strLocalPath + _T("\\ui\\common9.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath4.c_str());
	m_pWndSD = m_pGUI->CreateWnd(_T("\\desktop"), &element);
	m_pWndSD->SetInvisible(TRUE);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndSelct->GetFullName().c_str(), m_Trunk.sfp1(&SelectRoleFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWndMain->GetFullName().c_str(), m_Trunk.sfp1(&SelectRoleFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWndDel->GetFullName().c_str(), m_Trunk.sfp1(&SelectRoleFrame::EventHandler));
	m_pGUI->RegisterEventHandler(m_pWndSD->GetFullName().c_str(), m_Trunk.sfp1(&SelectRoleFrame::EventHandler));

	m_pStc_Sele_Name = (GUIStatic*)m_pWndSelct->GetChild(_T("optpic\\playername"));
	m_pStc_Sele_Info1 = (GUIStatic*)m_pWndSelct->GetChild(_T("optpic\\playermessage"));
	m_pStc_Sele_Info2 = (GUIStatic*)m_pWndSelct->GetChild(_T("optpic\\playermessage0"));

	m_pButtonCreate = (GUIButton*)m_pWndMain->GetChild(_T("optback\\cr_butt"));
	m_pButtonDelete = (GUIButton*)m_pWndMain->GetChild(_T("optback\\dr_butt"));
	m_pButtonEenter = (GUIButton*)m_pWndMain->GetChild(_T("optback\\enterbutt"));
	m_pButtonReturn = (GUIButton*)m_pWndMain->GetChild(_T("optback\\backbutt"));
	m_pButtonTWCG = (GUIButton*)m_pWndMain->GetChild(_T("optback\\cgbutt"));
	m_pButtonTWCG->SetInvisible(TRUE);
	if ( m_nPolicy == 5  || m_nPolicy == 9 )
	{
		m_pButtonReturn->SetEnable(FALSE);
		m_pButtonReturn->SetInvisible(TRUE);
	}
	if(m_nPolicy == 7)
	{
// 		if(GameSet::Inst()->Get(EGS_TWCG) == 1 )
// 			m_pButtonTWCG->SetText(g_StrTable[_T("PlayTWCG")]);
// 		else
// 			m_pButtonTWCG->SetText(g_StrTable[_T("SkipTWCG")]);
// 		m_pButtonTWCG->SetInvisible(FALSE);
	}
	m_pBtnSafeCode	= (GUIButton*)m_pWndMain->GetChild(_T("optback\\pinbutt"));
	m_pButtonEenter->SetEnable(false);
	m_pButtonDelete->SetEnable(false);

	m_pBtnDelOk		= (GUIButton*)m_pWndDel->GetChild(_T("comm_pic\\surebutt"));
	m_pBtnDelCancel = (GUIButton*)m_pWndDel->GetChild(_T("comm_pic\\canclebutt"));
	m_pEditSafeCode = (GUIEditBox*)m_pWndDel->GetChild(_T("comm_pic\\putinpic\\putin"));
	m_pStcDelHint	= (GUIEditBox*)m_pWndDel->GetChild(_T("comm_pic\\comm_words"));
	m_pStcDelHint->SetText(g_StrTable[_T("DelRole_Hint")]);

	//加载安全码控件
	m_pStcSDHint1	= (GUIStatic*)m_pWndSD->GetChild(_T("comm_pic\\words"));
	m_pStcSDHint2	= (GUIStatic*)m_pWndSD->GetChild(_T("comm_pic\\words2"));
	m_pEdtSafeCode1	= (GUIEditBox*)m_pWndSD->GetChild(_T("comm_pic\\inputpic1\\input1"));
	m_pEdtSafeCode2	= (GUIEditBox*)m_pWndSD->GetChild(_T("comm_pic\\inputpic2\\input2"));
	m_pBtnSDOK		= (GUIButton*)m_pWndSD->GetChild(_T("comm_pic\\surebutt"));
	m_pBtnSDCancel	= (GUIButton*)m_pWndSD->GetChild(_T("comm_pic\\canclebutt")); 
	CreateSafeCodeTime();

	m_pWndDel->SetInvisible(TRUE);
	m_pGUI->SetActive(m_pGUI->GetDesktop());

	if(7 == SaveAccount::Inst()->GetLoginMode() || 8 == SaveAccount::Inst()->GetLoginMode())
		m_pWndSelct->SetInvisible(TRUE);

	return TRUE;
}

//-----------------------------------------------------------------------------
// 读取登录验证策略
//-----------------------------------------------------------------------------
VOID SelectRoleFrame::LoadLoginMode()
{
	TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
	TCHAR szTemp[X_LONG_NAME*2];

	_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
	tstring strAppPath = szTemp;

	tstring strIniFileFullName = strAppPath + szIniFileName;
	::GetPrivateProfileString(_T("Launcher"), _T("LoginMode"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
	tstring policy = szTemp;
	if( policy == _T("") )
	{
		MessageBox(NULL, _T("Don't find LoginMode !"), _T(""), MB_OK);
		return;
	}
	m_nPolicy = _ttoi(policy.c_str());
}

//-----------------------------------------------------------------------------
// EnterGame
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::EnterGame(DWORD dwLocalRoleID)
{
	if( SDOAIgw::Inst()->GetSDOAFlag() )
	{
		tagSimRole role = SeleRoleMgr::Inst()->GetRoleInfo(m_nCurSelRole);
		SDOAIgw::Inst()->SendRoleInfo( role.szRoleName, role.Avatar.bySex );
	}
	m_pFrameMgr->CreateFrame(_T("Root"), _T("World"), _T("WorldFrame"), dwLocalRoleID);
	SeleRoleMgr::Inst()->Destroy(SeleRoleMgr::Inst()->GetHugeSG());
	m_pFrameMgr->AddToDestroyList(this);
	return TRUE;
}

VOID SelectRoleFrame::SendEnterGameMsg()
{
	if(m_pSession->IsConnect())
	{
		string strCommandLine = m_pUtil->UnicodeToAnsi( GetCommandLine() );
		if( strCommandLine.empty() || strCommandLine.length() > 1024 )
			return;
		const char *pCommandLine = strstr( strCommandLine.c_str(), ".exe" );
		if( pCommandLine == NULL )
			return;
		INT nPolicy = atoi(&pCommandLine[6]);
		if(nPolicy != 6)
		{
			TCHAR szIniFileName[X_LONG_NAME] = _T("launcher\\launcher.ini");
			TCHAR szTemp[X_LONG_NAME*2];

			_tcscpy( szTemp, Kernel::Inst()->GetWorkPath() );
			tstring strAppPath = szTemp;

			tstring strIniFileFullName = strAppPath + szIniFileName;
			::GetPrivateProfileString(_T("Config"), _T("Distribution"), NULL,  szTemp, X_LONG_NAME*2, strIniFileFullName.c_str());
			tstring policy = szTemp;
			if( policy == _T("") )
			{
				nPolicy = 0;
				return;
			}

			nPolicy = _ttoi(policy.c_str());
		}

		tagNC_SelectRole seleRoleMsg;
		seleRoleMsg.dwRoleID = m_nSelectRoleID;
		seleRoleMsg.nDistributionID = nPolicy;
		m_pSession->Send(&seleRoleMsg);
		m_pButtonEenter->SetEnable(false);
		m_pWndHintInfo->SetInvisible(FALSE);
		m_pWndHintInfo->SetText(g_StrTable[_T("EnterGame")]);
		SetCurState(ESRS_SentSelectRole);
		m_pWndMain->SetInvisible(TRUE);
	}
}


//-----------------------------------------------------------------------------
// ReturnGame
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::ReturnGame()
{
	if ( m_nPolicy == 7)
	{
		SaveAccount::Inst()->GoBack(true);
		SeleRoleMgr::Inst()->Destroy(SeleRoleMgr::Inst()->GetHugeSG());
		SeleRoleMgr::Inst()->CloseMap();
		SeleRoleMgr::Inst()->Destroy();
		m_pSession->Disconnect();
		m_pFrameMgr->AddToDestroyList(this);	
		HackShield::Inst()->StopApexClient();
		GameFrame* pFrame = m_pFrameMgr->CreateFrame(_T("Root"), _T("ServerSelect"), _T("ServerSelectFrame"), 0);
		return TRUE;
	}
	SaveAccount::Inst()->GoBack(true);
	SeleRoleMgr::Inst()->Destroy(SeleRoleMgr::Inst()->GetHugeSG());
	m_pFrameMgr->CreateFrame(_T("Root"), _T("Login"), _T("LoginFrame"), 0);
	m_pSession->Disconnect();
	m_pFrameMgr->AddToDestroyList(this);	
	HackShield::Inst()->StopApexClient();
	LoadingFrame* pFrame = (LoadingFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("Loading"), _T("LoadingFrame"), 0);
	if(P_VALID(pFrame))
		pFrame->SetLoadingType(LoadingFrame::ELT_Login);
	return TRUE;
}


//-----------------------------------------------------------------------------
// CreateRole
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::CreateRole()
{
	LoadingFrame* pFrame = (LoadingFrame*)m_pFrameMgr->CreateFrame(_T("Root"), _T("Loading"), _T("LoadingFrame"), 0);
	if( P_VALID(pFrame) )
		pFrame->SetLoadingType(LoadingFrame::ELT_CreateRole);
	m_pFrameMgr->AddToDestroyList(this);
	SeleRoleMgr::Inst()->DetachSceneNodeFromSG(SeleRoleMgr::Inst()->GetHugeSG());
	SeleRoleMgr::Inst()->CloseMap();
	GameFrame* crFrame = m_pFrameMgr->CreateFrame(_T("Root"), _T("CreateRole"), _T("CreateRoleFrame"), 0);
	
	return TRUE;
}


//-----------------------------------------------------------------------------
// DeleteRole
//-----------------------------------------------------------------------------
BOOL SelectRoleFrame::DeleteRole()
{
	if( P_VALID(m_pEditSafeCode) )
		m_pGUI->SetActive(m_pEditSafeCode);
	SetDeleteRoleUI(FALSE);
	return TRUE;
}

VOID SelectRoleFrame::SendDeleRoleMsg()
{
	if( P_VALID(GetObj("DeleteRoleAsk_MsgBox")) )
		KillObj( "DeleteRoleAsk_MsgBox" );

	CreateObj( "DeleteRoleAsk_MsgBox", "MsgBox" );

	TCHAR szTip[X_HUGE_NAME] = {0};
	tagSimRole role = SeleRoleMgr::Inst()->GetRoleInfo( m_nCurSelRole );
	_stprintf( szTip, g_StrTable[_T("NeedToDeleteRole_MsgBoxEvent")], role.byLevel, role.szRoleName );
	TObjRef<MsgBox>("DeleteRoleAsk_MsgBox")->Init( _T(""), szTip, _T("NeedToDeleteRole_MsgBoxEvent"), MsgBoxFlag(MBF_OK|MBF_Cancel), TRUE );
}

DWORD SelectRoleFrame::OnAcceptDeleteRole( tagMsgBoxEvent* pEvent )
{
	if( pEvent->eResult == MBF_Cancel )
		return 0;

	if(m_pSession->IsConnect())
	{
		tagNC_DeleteRole msgDelRole;
		msgDelRole.dwRoleID = m_nSelectRoleID;
		tstring szSafeCode = m_pEditSafeCode->GetText();
		msgDelRole.dwSafeCodeCrc = m_pUtil->Crc32(szSafeCode.c_str());
		m_pSession->Send(&msgDelRole);

		m_pBtnDelOk->SetEnable(false);
	}
	return 0;
}

// 接收玩家的角色信息
DWORD SelectRoleFrame::NetRecvEnumRole(tagNS_EnumRole* pMsg, DWORD pPrama)
{
	if(pMsg->dwErrorCode == E_Success)
	{	
		tagSimRole* pRole = (tagSimRole *)pMsg->bySimpleInfo;
		for(int i=0; i<pMsg->nNum; i++)
		{
			Player* pPlayer = new Player;
			SeleRoleMgr::Inst()->CreateRole(pRole[i], pPlayer);
		}
		SeleRoleMgr::Inst()->AttachSceneNodeToSG(SeleRoleMgr::Inst()->GetHugeSG());
		SeleRoleMgr::Inst()->RefreshTrack();
		SeleRoleMgr::Inst()->SetDWORDTime(pMsg->dwTimeReset);
		SeleRoleMgr::Inst()->SetSafeCodeCrc(pMsg->dwSafeCodeCrc);
		SeleRoleMgr::Inst()->SetInitSafeCode(TRUE);
		SeleRoleMgr::Inst()->SetLastLoginInfo( pMsg->dwTimeLastLogin, pMsg->dwIPLast, pMsg->dwIP );
		SeleRoleMgr::Inst()->SetSelRoleID(pMsg->dwRoleID);
		m_bDefault = true;
		UpdateUI();
		UpdateSafeCodeUI();
	}
	else
	{
		switch(pMsg->dwErrorCode)
		{
		case E_EnumRole_RoleEnumed:
			m_pWndHintInfo->SetInvisible(FALSE);
			m_pWndHintInfo->SetText(g_StrTable[_T("EnumRole_RoleEnumed")]);
			m_dwErrorTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		case E_EnumRole_RoleEnuming:
			m_pWndHintInfo->SetInvisible(FALSE);
			m_pWndHintInfo->SetText(g_StrTable[_T("EnumRole_RoleEnuming")]);
			m_dwErrorTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		case E_EnumRole_InWorld:
			m_pWndHintInfo->SetInvisible(FALSE);
			m_pWndHintInfo->SetText(g_StrTable[_T("EnumRole_InWorld")]);
			m_dwErrorTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		default:
			m_pWndHintInfo->SetInvisible(FALSE);
			m_pWndHintInfo->SetText(g_StrTable[_T("SystemError")]);
			m_dwErrorTime = Kernel::Inst()->GetAccumTimeDW();
			break;
		}
		SetCurState(ESRS_Back);
	}
	return 0;
}

//接收服务器删除角色消息
DWORD SelectRoleFrame::NetRecvDeleteRole(tagNS_DeleteRole* pMsg, DWORD pPrama)
{
	if( P_VALID(GetObj("ShowDeleteRoleReply_MsgBox")) )
		KillObj( "ShowDeleteRoleReply_MsgBox" );
	CreateObj( "ShowDeleteRoleReply_MsgBox", "MsgBoxEx" );

	if(pMsg->dwErrorCode == E_Success)
	{
		SeleRoleMgr::Inst()->DeleteRole(pMsg->dwRoleID, SeleRoleMgr::Inst()->GetHugeSG());
		//m_pEditSafeCode->SetText(_T(""));	
		m_nSelectRoleID = GT_INVALID;
		m_nCurSelRole = GT_INVALID;
		//m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_Succ")]);
		TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DeleRole_Succ")] );
		m_pStc_Sele_Name->SetText(_T(""));
		m_pButtonEenter->SetEnable(false);
		m_pButtonDelete->SetEnable(false);
		m_pWndMain->SetRefresh(TRUE);
		SetCurState(ESRS_Deling);
		SeleRoleMgr::Inst()->RefreshTrack();
	}
	else
	{
		switch(pMsg->dwErrorCode)
		{
		case E_DelRole_RoleNotExist:		//角色不存在
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_RoleNotExist")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_RoleNotExist")] );
			break;
		case E_DelRole_RoleDeleting:		//正在删除
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_RoleDeleting")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_RoleDeleting")] );
			break;
		case E_DelRole_RoleEnumNotDone:		//初始化错误
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_RoleEnumNotDone")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_RoleEnumNotDone")] );
			break;
		case E_DelRole_RoleCreating:		//正在创建
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_RoleCreating")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_RoleCreating")] );
			break;
		case E_DelRole_SafeCodeIncorrect:	//安全码错误
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_SafeCodeIncorrect")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_SafeCodeIncorrect")] );
			break;
		case E_DelRole_InWorld:				//角色已进入游戏世界，无法删除
			//m_pStcDelHint->SetText(g_StrTable[_T("DelRole_InWorld")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DelRole_InWorld")] );
			break;
		case E_DelRole_RoleLoading:
			//m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_RoleLoading")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DeleRole_RoleLoading")] );
			break;
		case E_DelRole_LeaveGuild1st:
			//m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_LeaveGuild1st")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DeleRole_LeaveGuild1st")] );
			break;
        case E_DelRole_BreakMarriage1st:
            //m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_BreakMarriage1st")]);
            TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DeleRole_BreakMarriage1st")] );
            break;
		case E_DelRole_BreakMaster1st:
			//m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_BreakMarriage1st")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("DeleRole_BreakMaster1st")] );
			break;
		case E_DelRole_LeaveFamily1st:
			//m_pStcDelHint->SetText(g_StrTable[_T("DeleRole_BreakMarriage1st")]);
			TObjRef<MsgBoxEx>("ShowDeleteRoleReply_MsgBox")->Init( _T(""), g_StrTable[_T("E_DelRole_LeaveFamily1st")] );
			break;
		}
	}
	m_pBtnDelOk->SetEnable(true);
	m_pWndDel->SetRefresh(TRUE);
	return 0;
}

// 接收玩家的选择角色信息
DWORD SelectRoleFrame::NetRecvSelectRole(tagNS_SelectRole* pMsg, DWORD pPrama)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			EnterGame(pMsg->dwRoleID);			
#ifdef ON_X_TRAP
			tstring strAccount = SaveAccount::Inst()->GetAccount();
			tstring szWorldName = SaveAccount::Inst()->GetWorldName();
			tstring szClass = _T("");
			tstring szName = SeleRoleMgr::Inst()->GetRoleNameByID(pMsg->dwRoleID);
			LPCSTR laccount = m_pUtil->UnicodeToAnsi(strAccount.c_str());
			LPCSTR lworldname = m_pUtil->UnicodeToAnsi(szWorldName.c_str());
			LPCSTR lclass = m_pUtil->UnicodeToAnsi(szClass.c_str());
			LPCSTR name = m_pUtil->UnicodeToAnsi(szName.c_str());
			XTrap_C_SetUserInfoEx(laccount,lworldname,name,lclass,NULL,m_pStream->GetSocket());
#endif
		}
		break;
	case E_SelectRole_RoleEnumNotDone:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_RoleEnumNotDone")]);
		break;
	case E_SelectRole_RoleCreating:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_RoleCreating")]);
		break;
	case E_SelectRole_RoleDeleting:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_RoleDeleting")]);
		break;
	case E_SelectRole_RoleNotExist:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_RoleNotExist")]);
		break;
	case E_SelectRole_InWorld:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_InWorld")]);
		break;
	case E_SelectRole_RoleLoading:
		m_pWndHintInfo->SetText(g_StrTable[_T("SelectRole_RoleLoading")]);
		break;
	default:

		break;
	}
	if(pMsg->dwErrorCode!=E_Success)
	{
		m_pWndMain->SetInvisible(FALSE);
		m_pButtonEenter->SetEnable(true);
		SetCurState(ESRS_RecvSelectRole);
		m_pWndHintInfo->SetInvisible(FALSE);
		SeleRoleMgr::Inst()->PlayStandTrack(m_nCurSelRole);
	}
	return 0;
}

VOID SelectRoleFrame::Update()
{
	float deltaTime = Kernel::Inst()->GetDeltaTime();
	switch(m_eCurState)
	{
	case ESRS_EnumRole:
		{
			if(m_pSession->IsConnect())
			{
				if ( m_nPolicy == 2 && m_bLoadXrMap==FALSE ) //迅雷模式在这里加载场景
				{
					LoadMap();
				}
				tagNC_EnumRole enumRoleMsg;
				m_pSession->Send(&enumRoleMsg);
			}
			else
			{
				if( P_VALID(GetObj("DisconnectServer_MsgBox")) )
					KillObj("DisconnectServer_MsgBox");
				CreateObj("DisconnectServer_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("DisconnectServer_MsgBox")->Init(_T(""), g_StrTable[_T("DisconnectServer")], 
					_T("DisconnectServer_Event"), MsgBoxFlag(MBF_OK), TRUE, TRUE);
				SetCurState(ESRS_Disconnect);
				return;
			}
		}
	case ESRS_Loading:
		if ( m_nPolicy != 2 ) //非迅雷模式在这里加载场景
		{
			LoadMap();
		}
		SetCurState(ESRS_Inputing);
		break;
	case ESRS_Inputing:
		{
			if( !m_pSession->IsConnect() )
			{
				if( P_VALID(GetObj("DisconnectServer_MsgBox")) )
					KillObj("DisconnectServer_MsgBox");
				CreateObj("DisconnectServer_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("DisconnectServer_MsgBox")->Init(_T(""), g_StrTable[_T("DisconnectServer")], 
					_T("DisconnectServer_Event"), MsgBoxFlag(MBF_OK), TRUE, TRUE);
				SetCurState(ESRS_Disconnect);
				return;
			}
			if(m_nCurSelRole!=GT_INVALID)
				SeleRoleMgr::Inst()->IsStandUpTrack(m_nCurSelRole);
			if(m_nLastSelRole!=GT_INVALID)
				SeleRoleMgr::Inst()->IsSitDownTrack(m_nLastSelRole);

			DisplayResetSafeCodeTime();

			if(m_pWndHintInfo->IsInvisible()==FALSE)
			{
				if(Kernel::Inst()->GetAccumTimeDW()-m_dwErrorTime > 2000)
					m_pWndHintInfo->SetInvisible(TRUE);
			}

			if(SeleRoleMgr::Inst()->GetRoleNum()>=MAX_ROLE_NUM)			//如果达到最大人数，不能创建
				m_pButtonCreate->SetEnable(false);
			else
				m_pButtonCreate->SetEnable(true);

			if( m_pInput->IsMouseDownOnce(0)
				&& m_pGUI->GetActive()==m_pGUI->GetDesktop() )
			{
				const tagPoint& ptCursor = CursorMgr::Inst()->GetCursorPos();
				int nSelect = SeleRoleMgr::Inst()->GetMouseSelectRole((int)ptCursor.x, (int)ptCursor.y);
				if(nSelect<0 || nSelect>SeleRoleMgr::Inst()->GetRoleNum())
					return;
				ReSetPButtonState(nSelect);
				//小于MOUSE_DOBULEDOWN_TIME认为是双击,进入游戏
				DWORD dwTime = Kernel::Inst()->GetAccumTimeDW();	
				if(dwTime - m_dwMouseDown <= MOUSE_DOBULEDOWN_TIME)
				{
					SeleRoleMgr::Inst()->PlayReadyTrack(m_nCurSelRole);
					m_eCurState = ESRS_SelectedRole;
					return;
				}
				m_dwMouseDown = dwTime;	
			}
			else if( m_bDefault && m_pGUI->GetActive()==m_pGUI->GetDesktop() )
			{
				m_bDefault = false;
				DWORD dwRoleID = SeleRoleMgr::Inst()->GetSelRoleID();
				int nSelect = SeleRoleMgr::Inst()->GetPickupRole( dwRoleID );
				if(nSelect<0 || nSelect>SeleRoleMgr::Inst()->GetRoleNum())
					return;
				ReSetPButtonState(nSelect);
			}
		}
		break;
	case ESRS_Deling:
		{
			if(timeGetTime() - m_dwStateTime >= 1000)			//删除成功后，停留在删除界面1秒中
			{
				SetDeleteRoleUI(TRUE);
				SetCurState(ESRS_Inputing);
			}
		}
		break;
	case ESRS_SentSelectRole:
		{
			if( !m_pSession->IsConnect() )
			{
				if( P_VALID(GetObj("DisconnectServer_MsgBox")) )
					KillObj("DisconnectServer_MsgBox");
				CreateObj("DisconnectServer_MsgBox", "MsgBoxEx");

				TObjRef<MsgBoxEx>("DisconnectServer_MsgBox")->Init(_T(""), g_StrTable[_T("DisconnectServer")], 
					_T("DisconnectServer_Event"), MsgBoxFlag(MBF_OK), TRUE, TRUE);
				SetCurState(ESRS_Disconnect);
			}
		}
		break;
	case ESRS_RecvSelectRole:
		{
			if( timeGetTime()-m_dwStateTime >= 3000 )
			{
				m_pWndHintInfo->SetInvisible(TRUE);
				SetCurState(ESRS_Inputing);
			}
		}
		break;
	case ESRS_SelectedRole:
		{
			if( SeleRoleMgr::Inst()->IsReadyTrackEnd(m_nCurSelRole) )
			{
				int nLoaded = 0, nTotal = 0;
				TObjRef<Client>()->GetLoadCount(Client::E_Level_BeforeEnterWorld, nLoaded, nTotal);
				if(nLoaded>=nTotal)
					SendEnterGameMsg();
				else
					Sleep(20);
			}
		}
		break;
	case ESRS_Back:
		{
			if(timeGetTime() - m_dwStateTime >= 2000)			//
			{
				ReturnGame();
			}
		}
		break;

	default:
		break;
	}

	if(!TObjRef<AudioSys>()->MusicIsPlaying())  
	{
		INT nMusic = SaveAccount::Inst()->GetCurMusic() ? 0 : 1;
		SaveAccount::Inst()->SetCurMusic(nMusic);
		TObjRef<AudioSys>()->StopMusic();
		TCHAR szBuff[X_SHORT_NAME] = {0};
		_sntprintf( szBuff, X_SHORT_NAME, _T("music_login%d"), nMusic );
		TObjRef<AudioSys>()->PlayMusic(szBuff, FALSE, SOUND_LOUD);
	}
}

void SelectRoleFrame::ReSetPButtonState(int nSele)
{
	//已经选择过了
	if(nSele==m_nCurSelRole)
		return;
	m_nLastSelRole = m_nCurSelRole;
	m_nCurSelRole = nSele;
	SeleRoleMgr::Inst()->PlayStandUpTrack(m_nCurSelRole, m_nLastSelRole);

	m_nSelectRoleID = SeleRoleMgr::Inst()->GetRoleId(m_nCurSelRole);

	tagSimRole role;	
	tstringstream strInfo;
	role = SeleRoleMgr::Inst()->GetRoleInfo(nSele);
	strInfo << role.szRoleName ;
	m_pStc_Sele_Name->SetText(strInfo.str().c_str());
	tstringstream strInfo1;
	strInfo1<< g_StrTable[_T("SelectRole_Lvl")] << role.byLevel;
	m_pStc_Sele_Info1->SetText(strInfo1.str().c_str());

	m_pWndSelct->SetRefresh();

	if(GT_INVALID!=m_nSelectRoleID && IS_PLAYER(m_nSelectRoleID))		//没有选择角色不能进入游戏,不能删除
	{
		m_pButtonEenter->SetEnable(true);
		m_pButtonDelete->SetEnable(true);
	}
	else
	{
		m_pButtonEenter->SetEnable(false);
		m_pButtonDelete->SetEnable(false);
	}
	strInfo.clear();
	strInfo1.clear();

	m_pWndMain->SetRefresh(TRUE);
}

VOID SelectRoleFrame::Render3D()
{
	float deltaTime=Kernel::Inst()->GetDeltaTime();

	SeleRoleMgr::Inst()->Update();
	SeleRoleMgr::Inst()->UpdateSG(deltaTime);
	SeleRoleMgr::Inst()->DrawSelectedRoleFlag(m_nCurSelRole);
	SeleRoleMgr::Inst()->DrawOldPlayerRewardFlag();
}

//const Vector3 Lookat = Vector3(5000, 1200, 8000);
const float CamYaw=_DegToRad(0.0f);
const float CamPitch=_DegToRad(2.0f);
const float CamRoll=0.0f;
const float MAXCamDist=4000.0f;
//const Vector3 LookFrom = Vector3(8300,800,4400);

VOID SelectRoleFrame::LoadMap()
{
	const tagCameraData* pData = CameraSetData::Inst()->GetData();
	SeleRoleMgr::Inst()->OpenMap(_T("select"), SeleRoleMgr::Inst()->GetCamera());
	SeleRoleMgr::Inst()->AttachSceneNodeToSG(SeleRoleMgr::Inst()->GetHugeSG());
	SeleRoleMgr::Inst()->RefreshTrack();
	//SeleRoleMgr::Inst()->SetCamera(Lookat, CamYaw, CamPitch, CamRoll, MAXCamDist);
	Vector3 LookAt = Vector3(pData->dwSelectRoleCameraLookAt_x, pData->dwSelectRoleCameraLookAt_y, pData->dwSelectRoleCameraLookAt_z);
	Vector3 LookFrom = Vector3(pData->dwSelectRoleCameraPos_x, pData->dwSelectRoleCameraPos_y, pData->dwSelectRoleCameraPos_z);
	SeleRoleMgr::Inst()->SetCamera(LookFrom,LookAt);

	
	UpdateUI();
	UpdateSafeCodeUI();

	m_bLoadXrMap = true;

}

VOID SelectRoleFrame::SetDeleteRoleUI(BOOL bVal)
{
	if(7 == SaveAccount::Inst()->GetLoginMode() || 8 == SaveAccount::Inst()->GetLoginMode())
		m_pWndSelct->SetInvisible(TRUE);
	else
		m_pWndSelct->SetInvisible(!bVal);

	m_pWndDel->SetInvisible(bVal);
	m_pWndMain->SetInvisible(!bVal);
	m_pStcDelHint->SetText(g_StrTable[_T("DelRole_Hint")]);
}

void SelectRoleFrame::JudeSafeCodeState()
{
	switch(m_eSafeCodeState)
	{
	case ESCS_NULL:
	case ESCS_Reseted:
		{
			//安全码介绍
			if( P_VALID(GetObj("SafeCodeIntroductionMsgBox")) )
				KillObj("SafeCodeIntroductionMsgBox");
			CreateObj("SafeCodeIntroductionMsgBox", "MsgBox");

			TObjRef<MsgBox>("SafeCodeIntroductionMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_7")], 
				_T("SafeCodeIntroductionEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	case ESCS_HasSafeCode:
		{
			if( P_VALID(GetObj("ResetSafeCodeMsgBox")) )
				KillObj("ResetSafeCodeMsgBox");
			CreateObj("ResetSafeCodeMsgBox", "MsgBox");

			TObjRef<MsgBox>("ResetSafeCodeMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_1")], 
				_T("ResetSafeCodeEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	case ESCS_Reseting:
		{
			if( P_VALID(GetObj("CancelSafeCodeMsgBox")) )
				KillObj("CancelSafeCodeMsgBox");
			CreateObj("CancelSafeCodeMsgBox", "MsgBox");

			TObjRef<MsgBox>("CancelSafeCodeMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_2")], 
				_T("CancelSafeCodeEvent"), (MsgBoxFlag)(MBF_OK|MBF_Cancel), TRUE);
		}
		break;
	}
}

VOID SelectRoleFrame::SendSafeCodeMsg()
{
	if( !P_VALID(m_pEdtSafeCode1)
		|| !P_VALID(m_pEdtSafeCode2)) 
	{
		return;
	}

	tstring strSD1 = m_pEdtSafeCode1->GetText();
	tstring strSD2 = m_pEdtSafeCode2->GetText();
	
	if(strSD1!=strSD2)
	{
		if( P_VALID(GetObj("SafeCodeErrorMsgBox")) )
			KillObj("SafeCodeErrorMsgBox");
		CreateObj("SafeCodeErrorMsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("SafeCodeErrorMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_3")], 
			_T("SafeCodeError_Event"), (MsgBoxFlag)(MBF_OK), TRUE);

		m_pEdtSafeCode1->SetText(_T(""));
		m_pEdtSafeCode2->SetText(_T(""));
		m_pWndSD->SetRefresh(TRUE);
		return;
	}

	if( _tcslen(strSD1.c_str()) > 14 )
	{
		if( P_VALID(GetObj("SafeCodeErrorMsgBox")) )
			KillObj("SafeCodeErrorMsgBox");
		CreateObj("SafeCodeErrorMsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("SafeCodeErrorMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_8")], 
			_T("SafeCodeError_Event"), (MsgBoxFlag)(MBF_OK), TRUE);

		m_pEdtSafeCode1->SetText(_T(""));
		m_pEdtSafeCode2->SetText(_T(""));
		m_pWndSD->SetRefresh(TRUE);
		return;
	}

	if( IsValidSafeCode(strSD1.c_str()) != 0 )
	{
		if( P_VALID(GetObj("SafeCodeErrorMsgBox")) )
			KillObj("SafeCodeErrorMsgBox");
		CreateObj("SafeCodeErrorMsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("SafeCodeErrorMsgBox")->Init(_T(""), g_StrTable[_T("CreateRole_SafeCodeHint")], 
			_T("SafeCodeError_Event"), (MsgBoxFlag)(MBF_OK), TRUE);

		m_pEdtSafeCode1->SetText(_T(""));
		m_pEdtSafeCode2->SetText(_T(""));
		return;
	}

	tagNC_SafeCode msg;
	msg.dwSafeCode = m_pUtil->Crc32(strSD1.c_str());
	m_pSession->Send(&msg);
}

int SelectRoleFrame::IsValidSafeCode(const TCHAR* szSafeCode)
{
	if(szSafeCode == NULL)
		return 1;
	int len = _tcslen(szSafeCode);
	if(len>14)
		return 1;
	
	for(int i=0; i<len; i++)			//安全码必须为大小写字母或数字
	{
		TCHAR ch = szSafeCode[i];
		if(ch>='0' && ch<='9')
			continue;
		if(ch>='a' && ch<='z')
			continue;
		if(ch>='A' && ch<='Z')
			continue;
		return 1;
	}
	
	return 0;
}

DWORD SelectRoleFrame::NetRecvNS_SafeCode( tagNS_SafeCode* pMsg, DWORD pParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			m_pWndSD->SetInvisible(TRUE);
			tstring strSD1 = m_pEdtSafeCode1->GetText();
			SeleRoleMgr::Inst()->SetSafeCodeCrc(m_pUtil->Crc32(strSD1.c_str()));
			m_eSafeCodeState = ESCS_HasSafeCode;

			if( P_VALID(GetObj("SafeCodeSuccessMsgBox")) )
				KillObj("SafeCodeSuccessMsgBox");
			CreateObj("SafeCodeSuccessMsgBox", "MsgBoxEx");

			TObjRef<MsgBoxEx>("SafeCodeSuccessMsgBox")->Init(_T(""), g_StrTable[_T("SafeCode_9")], 
				_T("SafeCodeSuccess_Event"), (MsgBoxFlag)(MBF_OK), TRUE);

		}
		break;
	case E_SystemError:
		{
			m_pStcSDHint1->SetText(g_StrTable[_T("SafeCode_4")]);
		}
		break;
	}

	return 0;
}

DWORD SelectRoleFrame::NetRecvNS_ResetSafeCode( tagNS_ResetSafeCode* pMsg, DWORD pParam )
{
	SeleRoleMgr::Inst()->SetDWORDTime(pMsg->dwTimeReset);
	m_eSafeCodeState = ESCS_Reseting;
	return 0;
}

DWORD SelectRoleFrame::NetRecvNS_CancelSafeCodeReset( tagNS_CancelSafeCodeReset* pMsg, DWORD pParam )
{
	switch(pMsg->dwErrorCode)
	{
	case E_Success:
		{
			m_eSafeCodeState = ESCS_HasSafeCode;
			DWORD dwTime = GT_INVALID;
			SeleRoleMgr::Inst()->SetDWORDTime(tagDWORDTime(dwTime));
		}
		break;
	case E_SystemError:
		{
			m_pWndHintInfo->SetText(g_StrTable[_T("SafeCode_5")]);
			m_pWndHintInfo->SetInvisible(FALSE);
			m_dwErrorTime = Kernel::Inst()->GetAccumTimeDW();
		}
		break;
	}
	return 0;
}

void SelectRoleFrame::CreateSafeCodeTime()
{
	GUIWnd* pDesktop = m_pGUI->GetDesktop();
	int nX=0, nY=0;
	XmlElement ele;

	nX = 20;

	ele.SetAttribute("Name", "SafeCodeTime");
	ele.SetAttribute("ClassName", "CGUIStatic");
	ele.SetAttribute("Size_x", 200);
	ele.SetAttribute("Size_y", 40);
	ele.SetAttribute("FontWidth", 8);
	ele.SetAttribute("FontHeight", 16);
	ele.SetAttribute("Align_Type", "InnerLeftTop");
	ele.SetAttribute("LogicPos_x", nX);
	ele.SetAttribute("LogicPos_y", 50);
	ele.SetAttribute("FontWidth", 8);
	ele.SetAttribute("FontHeight", 16);
	ele.SetAttribute("TextRect_left", 0);
	ele.SetAttribute("TextRect_top", 0);
	ele.SetAttribute("TextRect_right", 200);
	ele.SetAttribute("TextRect_bottom", 60);
	ele.SetAttribute("PicColor", 0xFFFFFF);

	m_pStcSDHint = (GUIStatic*)m_pGUI->CreateWnd(_T("\\desktop"), &ele);
	if( P_VALID(m_pStcSDHint) )
		m_pStcSDHint->SetInvisible(TRUE);
}

DWORD SelectRoleFrame::OnResetSafeCodeEvent( tagMsgBoxEvent* pEvent )
{
	if(pEvent->strEventName == _T("ResetSafeCodeEvent"))
	{
		if(pEvent->eResult == MBF_OK)
		{
			tagNC_ResetSafeCode msg;
			m_pSession->Send(&msg);
		}
	}
	return 0;
}

DWORD SelectRoleFrame::OnCancelSafeCodeEvent( tagMsgBoxEvent* pEvent )
{
	if(pEvent->strEventName == _T("CancelSafeCodeEvent"))
	{
		if(pEvent->eResult == MBF_OK)
		{
			tagNC_CancelSafeCodeReset msg;
			m_pSession->Send(&msg);
		}
	}
	return 0;
}

DWORD SelectRoleFrame::OnSafeCodeIntroductionEvent(tagMsgBoxEvent* pEvent)
{
	if(pEvent->eResult == MBF_OK)
	{
		m_pWndSD->SetInvisible(FALSE);
		m_pStcSDHint1->SetText(g_StrTable[_T("CreateRole_SafeCodeHint")]);
		m_pStcSDHint2->SetText(g_StrTable[_T("SafeCode_0")]);
		m_pEdtSafeCode1->SetText(_T(""));
		m_pEdtSafeCode2->SetText(_T(""));
	}
	return 0;
}

const DWORD dwMinSec = 60 * 60;
const DWORD dwHourSec = 72 * 60;

void SelectRoleFrame::DisplayResetSafeCodeTime()
{
	if( !SeleRoleMgr::Inst()->GetInitSafeCode() )
		return;

	if(m_eSafeCodeState == ESCS_Reseting)
	{
		tagDWORDTime dwCurTimeTemp = ServerTime::Inst()->CalCurrentServerDwordTime();
		DWORD dwTimeDiffTemp = CalcTimeDiff(dwCurTimeTemp, SeleRoleMgr::Inst()->GetDWORDTime());
		if( dwTimeDiffTemp > MAX_SAFECODE_RESET_TIME )
		{
			if( m_pStcSDHint->IsInvisible() == FALSE)
				m_pStcSDHint->SetInvisible(TRUE);
			m_eSafeCodeState =  ESCS_Reseted;
			return;
		}
		DWORD dwTimeDiff = MAX_SAFECODE_RESET_TIME - dwTimeDiffTemp;
		DWORD dwHour = dwTimeDiff / dwMinSec;
		DWORD dwMin =0;
		if(dwHour != 0)
			dwMin= dwTimeDiff % (dwHour*60) / 60;
		else
			dwMin= dwTimeDiff/ 60;

		DWORD dwSec = dwTimeDiff - (dwHour*dwMinSec+dwMin*60);
		TCHAR szBuff[X_LONG_NAME] = {0};
		_stprintf(szBuff, g_StrTable[_T("SafeCode_6")], dwHour/24, dwHour%24, dwMin*60+dwSec);
		if( P_VALID(m_pStcSDHint) )
		{
			m_pStcSDHint->SetText(szBuff);
			if(m_pStcSDHint->IsInvisible())
				m_pStcSDHint->SetInvisible(FALSE);
		}
	}
	else 
	{
		if( m_pStcSDHint->IsInvisible() == FALSE)
			m_pStcSDHint->SetInvisible(TRUE);
	}
}

VOID SelectRoleFrame::UpdateSafeCodeUI()
{
	DWORD dwSafeCodeCrc = SeleRoleMgr::Inst()->GetSafeCodeCrc();
	tagDWORDTime tagTime = SeleRoleMgr::Inst()->GetDWORDTime();
	tagDWORDTime dwCurTimeTemp = ServerTime::Inst()->CalCurrentServerDwordTime();
	DWORD dwTimeDiffTemp = CalcTimeDiff(dwCurTimeTemp, SeleRoleMgr::Inst()->GetDWORDTime());

	if( dwSafeCodeCrc!=GT_INVALID
		&& (tagTime)!=GT_INVALID 
		&& dwTimeDiffTemp<MAX_SAFECODE_RESET_TIME )
		m_eSafeCodeState = ESCS_Reseting;
	else if( dwSafeCodeCrc!=GT_INVALID
		&& (tagTime)==GT_INVALID )
		m_eSafeCodeState = ESCS_HasSafeCode;
	else 
		m_eSafeCodeState = ESCS_NULL;
}

DWORD SelectRoleFrame::OnDisconnectServer( tagMsgBoxEvent* pGameEvent )
{
	if( pGameEvent->eResult == MBF_OK )
	{
		if(SaveAccount::Inst()->GetLoginMode() != 11)
		{
			SetCurState(ESRS_Back);
		}
		else
		{
			m_pMgr->AddToDestroyList( GetWorldFrm() );
			m_pSession->Disconnect();
			HackShield::Inst()->StopApexClient();
			PostQuitMessage( 0 );
		}
	}
	return 0;
}

VOID SelectRoleFrame::UpdateUI()
{
	if( GT_INVALID!=m_nCurSelRole && IS_PLAYER(m_nSelectRoleID) )
	{
		ReSetPButtonState(m_nCurSelRole);
		return;
	}

	tagDWORDTime time  = SeleRoleMgr::Inst()->GetTimeLastLogin();
	if( 0==time.year )
		return;
	TCHAR szBuff[X_LONG_NAME] = {0};
	tstringstream info;
	_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("SelectRole_LastLoginTime")], time.year+2000, time.month, time.day, time.hour, time.min );
	info << szBuff << _T("\n");
	DWORD dwLastIP = SeleRoleMgr::Inst()->GetIPLastLogin();
	if( GT_INVALID!=dwLastIP && 0!=dwLastIP )
	{
		sockaddr_in address;
		ZeroMemory(&address, sizeof(address));
		address.sin_addr.s_addr = dwLastIP;
		string ip = inet_ntoa(address.sin_addr);
		_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("SelectRole_LastIP")], m_pUtil->AnsiToUnicode(ip.c_str()) );
		info << szBuff << _T("\n");
	}
	DWORD dwIP = SeleRoleMgr::Inst()->GetIPCurrent();
	if( GT_INVALID!=dwIP && 0!=dwIP )
	{
		sockaddr_in address;
		ZeroMemory(&address, sizeof(address));
		address.sin_addr.s_addr = dwIP;
		string ip = inet_ntoa(address.sin_addr);
		_sntprintf( szBuff, X_LONG_NAME, g_StrTable[_T("SelectRole_CurrentIP")], m_pUtil->AnsiToUnicode(ip.c_str()) );
		info << szBuff << _T("\n");
	}
	m_pStc_Sele_Info2->SetText(info.str().c_str());
	info.clear();
	m_pWndSelct->SetRefresh();
}