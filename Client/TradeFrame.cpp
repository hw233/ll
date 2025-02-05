#include "StdAfx.h"
#include "TradeFrame.h"
#include "..\WorldDefine\msg_exchange.h"
#include "ItemFrame.h"
#include "ShopShelfFrame.h"
#include "PlayerTradeFrame.h"
#include "RoleMgr.h"
#include "PlayerNameTab.h"
#include "CombatEvent.h"
#include "NetSession.h"
#include "CombatSysUtil.h"
#include "AudioSys.h"
#include "..\WorldDefine\msg_pet_exchange.h"
#include "PetTradeUIFrame.h"
#include "SpritePartnershipFrame.h"
#include "..\WorldDefine\msg_combat.h"
#include "SkillMgr.h"

const DWORD MAX_EXCHANGE_REQTIME	=	15 * 1000;	// 最大等待请求时间，毫秒
const DWORD SAME_MSG_ALTERNATE_TIME		=	3 * 1000;	// 相同消息显示间隔，单位：毫秒

TradeFrame::TradeFrame(void) 
: m_Trunk(this),
  m_dwStartTime(0),
  m_bIsInAlternateTime(FALSE)
{
	m_dwTargetID = GT_INVALID;
	m_dwReqTime	 = GT_INVALID;	
}

TradeFrame::~TradeFrame(void)
{
}

BOOL TradeFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	// 注册网络消息
	m_pCmdMgr->Register("NS_ExchangeReq",		(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeReq),	_T("NS_ExchangeReq"));
	m_pCmdMgr->Register("NS_ExchangeReqRes",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeReqRes),	_T("NS_ExchangeReqRes"));
	m_pCmdMgr->Register("NS_ExchangeCancel",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeCancel),	_T("NS_ExchangeCancel"));
	m_pCmdMgr->Register("NS_ExchangeFinish",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeFinish),	_T("NS_ExchangeFinish"));
	m_pCmdMgr->Register("NS_Skill",						(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNS_Skill),				 _T("NS_Skill"));

	// 注册游戏事件
	m_pMgr->RegisterEventHandle(_T("Open_Shop"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventOpenShop));
	m_pMgr->RegisterEventHandle(_T("Close_Shop"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventCloseShop));
	m_pMgr->RegisterEventHandle(_T("Close_Shop_Now"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventCloseShopNow));
	m_pMgr->RegisterEventHandle(_T("TargetEvent_ReqTrade"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventReqTrade));
	m_pMgr->RegisterEventHandle(_T("MsgBox_ExchangeReqCheck"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventReqReply));

	return bRet;
}

BOOL TradeFrame::Destroy()
{
	BOOL bRet = GameFrame::Destroy();
	
	// 注销网络消息
	m_pCmdMgr->UnRegister("NS_ExchangeReq",		(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeReq));
	m_pCmdMgr->UnRegister("NS_ExchangeReqRes",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeReqRes));
	m_pCmdMgr->UnRegister("NS_ExchangeCancel",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeCancel));
	m_pCmdMgr->UnRegister("NS_ExchangeFinish",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNetExchangeFinish));

	m_pCmdMgr->UnRegister("NS_Skill",	(NETMSGPROC)m_Trunk.sfp2(&TradeFrame::OnNS_Skill));

	// 注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("Open_Shop"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventOpenShop));
	m_pMgr->UnRegisterEventHandler(_T("Close_Shop"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventCloseShop));
	m_pMgr->UnRegisterEventHandler(_T("Close_Shop_Now"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventCloseShopNow));
	m_pMgr->UnRegisterEventHandler(_T("TargetEvent_ReqTrade"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventReqTrade));
	m_pMgr->UnRegisterEventHandler(_T("MsgBox_ExchangeReqCheck"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&TradeFrame::OnEventReqReply));

	if(P_VALID(GetObj("ExchangeReq")))
	{
		KillObj("ExchangeReq");
	}
	return bRet;
}


VOID TradeFrame::Update()
{
	GameFrame::Update();
	if(GT_VALID(m_dwReqTime))
	{
		m_dwReqTime -= Kernel::Inst()->GetDeltaTimeDW();
		if(m_dwReqTime > 0x7FFFFFFF)// 如果m_dwReqTime为负值则关闭请求交易窗口
		{
			if(P_VALID(GetObj("ExchangeReq")))
			{
				KillObj("ExchangeReq");
				tagNC_ExchangeReqRes cmd;
				cmd.dwDstID		= m_dwTargetID;
				cmd.dwErrorCode = E_Exchange_NotAnswer;
				m_pSession->Send(&cmd);
				m_dwReqTime = GT_INVALID;
			}
		}
	}
	if ( Kernel::Inst()->GetAccumTimeDW() - m_dwStartTime > SAME_MSG_ALTERNATE_TIME )
	{
		m_dwStartTime = Kernel::Inst()->GetAccumTimeDW();
		m_bIsInAlternateTime = FALSE;
	}
}

DWORD TradeFrame::OnNetExchangeReq( tagNS_ExchangeReq* pNetCmd, DWORD )
{
	if (pNetCmd->dwSrcID != RoleMgr::Inst()->GetLocalPlayerID())
	{
		m_dwTargetID = pNetCmd->dwSrcID;
		m_dwReqTime	 = MAX_EXCHANGE_REQTIME;

		if(P_VALID(GetObj("ExchangeReq")))
		{
			KillObj("ExchangeReq");
		}
		if(P_VALID(GetObj("PetExchangeReq")))
		{
			KillObj("PetExchangeReq");
			tagTradeMutex e(_T("tagTradeMutex"), NULL);
			m_pMgr->SendEvent(&e);
		}
		TCHAR szTemp[X_LONG_NAME] = {0};
		_sntprintf(szTemp, sizeof(szTemp)/sizeof(TCHAR), g_StrTable[_T("Exchange_ReqCheck")], PlayerNameTab::Inst()->FindNameByID(pNetCmd->dwSrcID));
		CreateObj("ExchangeReq", "MsgBox");
		TObjRef<MsgBox>("ExchangeReq")->Init(_T(""), szTemp, _T("MsgBox_ExchangeReqCheck"));
	}
	return 0;
}

DWORD TradeFrame::OnNetExchangeReqRes( tagNS_ExchangeReqRes* pNetCmd, DWORD )
{
	DWORD dwLPID = RoleMgr::Inst()->GetLocalPlayerID();
	PetTradeUIFrame* pFrame = (PetTradeUIFrame*)m_pMgr->GetFrame(_T("PetTradeUIFrame"));
	if (pNetCmd->dwSrcID == dwLPID)
	{
		if (pNetCmd->dwErrorCode == E_Exchange_Accept)
		{
			OpenPlayerTrade(pNetCmd->dwDstID);
			//若宠物交易打开则取消宠物交易，进行物品交易
			if (P_VALID(pFrame)) 
			{
				tagNC_PetExchangeCancel msg;
				m_pSession->Send(&msg);
			}
		}
		else
		{
			ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
		}
	}
	else if (pNetCmd->dwDstID == dwLPID)
	{
		if (pNetCmd->dwErrorCode == E_Exchange_Accept)
		{
			OpenPlayerTrade(pNetCmd->dwSrcID);
			//若宠物交易打开则取消宠物交易，进行物品交易
			if (P_VALID(pFrame)) 
			{
				tagNC_PetExchangeCancel msg;
				m_pSession->Send(&msg);
			}
		}
		else
		{
			ShowExchangeErrorMsg(pNetCmd->dwErrorCode);
		}
	}
	return 0;
}

DWORD TradeFrame::OnNetExchangeCancel( tagNS_ExchangeCancel* pNetCmd, DWORD )
{
	if ( m_bIsInAlternateTime == FALSE ) //防止多条该消息重复出现
	{
		// 显示提示信息
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Exchange_Cancel")]);
		m_bIsInAlternateTime = TRUE;
		m_dwStartTime = Kernel::Inst()->GetAccumTimeDW();
	}

	// 关闭交易窗口
	ClosePlayerTrade();
	
	return 0;
}

DWORD TradeFrame::OnNetExchangeFinish( tagNS_ExchangeFinish* pNetCmd, DWORD )
{
	if(E_Success == pNetCmd->dwErrorCode)
	{
		// 显示提示信息
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Exchange_Finish")]);

		// 播放交易完成音效
		TObjRef<AudioSys>()->Play2DSound(_T("trade"), SOUND_NORMAL);
	}
	else// 显示错误提示
	{
		// 显示交易失败
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Exchange_Failed")]);
		ShowExchangeErrorMsg(pNetCmd->dwErrorCode, pNetCmd->dwFailedRoleID != RoleMgr::Inst()->GetLocalPlayerID());
	}

	// 如果是背包空间不足，则不关闭交易窗口，其他情况需要关闭
	if(E_Exchange_NotEnough_BagSpace == pNetCmd->dwErrorCode)
	{
		UnlockPlayerTrade();
	}
	else
	{
		ClosePlayerTrade();
	}

	return 0;
}


DWORD TradeFrame::OnEventOpenShop( tagGameEvent* pGameEvent )
{
	GameFrame* pShop = m_pMgr->GetFrame(_T("ShopShelf"));
	if (!P_VALID(pShop))
	{
		pShop = (ShopShelfFrame*)m_pMgr->CreateFrame(m_strName.c_str(), _T("ShopShelf"), _T("ShopShelfFrame"), 0);
	}

    if(P_VALID(pShop))
        SetStrategy(EIST_ItemShopVend);

    ShopShelfFrame* pShopFrame = static_cast<ShopShelfFrame*>(pShop);

    pShopFrame->ShowWnd(TRUE);

	//代开NPC商店，同时自动打开行囊
	m_pMgr->SendEvent( &tagGameEvent( _T("ForceOpen_Package_UI"), this ) );

	return 0;
}

DWORD TradeFrame::OnEventCloseShop( tagGameEvent* pGameEvent )
{
	// 关闭商店窗口
	GameFrame* pFrame = m_pMgr->GetFrame(_T("ShopShelf"));
	if(P_VALID(pFrame))
	{
        ShopShelfFrame* pShopFrame = static_cast<ShopShelfFrame*>(pFrame);
        if (pShopFrame->IsOpen())
        {
            pShopFrame->ShowWnd(FALSE);
            SetStrategy(EIST_ItemPocket);
        }
	}
	return 0;
}

DWORD TradeFrame::OnEventCloseShopNow( tagGameEvent* pGameEvent )
{
	// 关闭商店窗口
	GameFrame* pFrame = m_pMgr->GetFrame(_T("ShopShelf"));
	if(P_VALID(pFrame))
	{
        ShopShelfFrame* pShopFrame = static_cast<ShopShelfFrame*>(pFrame);
        if (pShopFrame->IsOpen())
        {
            pShopFrame->ShowWnd(FALSE);
		    SetStrategy(EIST_ItemPocket);
        }
	}
	return 0;
}

DWORD TradeFrame::OnEventReqTrade( tagFollowRoleEvent* pGameEvent )
{
	if(pGameEvent->dwTargetRoleID != RoleMgr::Inst()->GetLocalPlayerID())
	{
		if(RoleMgr::Inst()->IsOutValidDist(pGameEvent->dwTargetRoleID))
		{
			ShowExchangeErrorMsg(E_Exchange_OutOfRange);
			return 0;
		}

		tagNC_ExchangeReq m;
		m.dwDstID = pGameEvent->dwTargetRoleID;
		m_pSession->Send(&m);
	}
	return 0;
}

DWORD TradeFrame::OnEventReqReply( tagMsgBoxEvent* pGameEvent )
{
	tagNC_ExchangeReqRes m;
	m.dwDstID = m_dwTargetID;
	if(MBF_OK == pGameEvent->eResult)
	{
		m.dwErrorCode = E_Exchange_Accept;
		if(RoleMgr::Inst()->IsOutValidDist(m_dwTargetID))
		{
			m.dwErrorCode = E_Exchange_OutOfRange;
			ShowExchangeErrorMsg(E_Exchange_OutOfRange);
		}
	}
	else if(MBF_Cancel == pGameEvent->eResult)
	{
		m.dwErrorCode = E_Exchange_Refuse;
	}
	else
	{
		m.dwErrorCode = E_Exchange_NotAnswer;
	}
	m_pSession->Send(&m);
	m_dwReqTime = GT_INVALID;
	return 0;
}

VOID TradeFrame::OpenPlayerTrade( DWORD dwTargetID )
{
	if(P_VALID(GetObj("PetExchangeReq")))
	{
		KillObj("PetExchangeReq");
		tagTradeMutex e(_T("tagTradeMutex"), NULL);
		m_pMgr->SendEvent(&e);
	}

	// 创建交易界面
	PlayerTradeFrame* pFrame = (PlayerTradeFrame*)m_pMgr->CreateFrame(m_strName.c_str(), _T("PlayerTrade"), _T("PlayerTradeFrame"), 0);
	if (P_VALID(pFrame))
	{
		pFrame->SetTargetPlayerID(dwTargetID);
		SetStrategy(EIST_ItemPlayerTrade);

		SpritePartnershipFrame *pFrame = (SpritePartnershipFrame*)TObjRef<GameFrameMgr>()->GetFrame( _T("SpritePartnership") );
		if( P_VALID(pFrame) )
		{
			pFrame->EscCancel();
		}
	}
}

VOID TradeFrame::ClosePlayerTrade()
{
	// 删除交易界面
	GameFrame* pFrame = m_pMgr->GetFrame(_T("PlayerTrade"));
	if (P_VALID(pFrame))
		m_pMgr->AddToDestroyList(pFrame);
	
	// 删除交易提示框
	if(P_VALID(GetObj("ExchangeReq")))
	{
		KillObj("ExchangeReq");
	}
	SetStrategy(EIST_ItemPocket, EIST_ItemPlayerTrade);
}

VOID TradeFrame::ShowExchangeErrorMsg( DWORD dwErrorCode, BOOL bNotMine )
{
	if (dwErrorCode > E_Exchange_Accept && dwErrorCode < E_Exchange_End )
	{
		TCHAR szTmp[X_SHORT_NAME] = {0};
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("ExchangeErrorMsg%d"), dwErrorCode);
		if(bNotMine) //如果不是自己则需要加上对方玩家，以示区别
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Exchange_TargetError")], g_StrTable[szTmp]);
		else
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szTmp]);
	}
}

VOID TradeFrame::UnlockPlayerTrade()
{
	PlayerTradeFrame* pFrame = (PlayerTradeFrame*)m_pMgr->GetFrame(_T("PlayerTrade"));
	if (P_VALID(pFrame))
	{
		pFrame->LockDstControl(false);
		pFrame->LockSrcControl(false);
	}
}

//DWORD TradeFrame::OnEventOpenShop( tagGameEvent* pGameEvent )
DWORD TradeFrame::OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam)
{
	if(pMsg->dwErrorCode!=E_Success)
		return 0; 

	const tagSkillData* pData = SkillMgr::Inst()->GetSkillData(pMsg->dwSkillID/100);
	if(P_VALID(pData))
	{
		if(ESSF_FreeOpenFace == pData->pProto->eSpecFunc && ESSOF_SHOP == pData->pProto->nSpecFuncVal1)
		{
			GameFrame* pShop = m_pMgr->GetFrame(_T("ShopShelf"));
			if (!P_VALID(pShop))
			{
				pShop = (ShopShelfFrame*)m_pMgr->CreateFrame(m_strName.c_str(), _T("ShopShelf"), _T("ShopShelfFrame"), 0);
			}

			if(P_VALID(pShop))
				SetStrategy(EIST_ItemShopVend);

			ShopShelfFrame* pShopFrame = static_cast<ShopShelfFrame*>(pShop);

			pShopFrame->ShowWndDirect(TRUE,pData->pProto->nSpecFuncVal2);

			//代开NPC商店，同时自动打开行囊
			m_pMgr->SendEvent( &tagGameEvent( _T("ForceOpen_Package_UI"), this ) );
		}
	}

	return 0;
}