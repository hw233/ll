#include "StdAfx.h"
#include "ItemProduceFrame.h"
#include "DragBtn.h"
#include "ProduceData.h"
#include "Item.h"
#include "PocketFrame.h"
#include "Container.h"
#include "ItemMgr.h"
#include "ItemProtoData.h"
#include "ToolTipCreator.h"
#include "ToolTipFrame.h"
#include "CurrencyMgr.h"
#include "..\WorldDefine\ItemDefine.h"
#include "SkillMgr.h"
#include "RoleMgr.h"
#include "AudioSys.h"
#include "SkillEvent.h"
#include "ItemFrame.h"
#include "ComposeMgr.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\currency_define.h"
#include "GUIEffectMgr.h"

#define MIsRWeapon(pProtoType)		(pProtoType->eType == EIT_Weapon)
#define MIsClothArmor(pProtoType)	(pProtoType->eType == EIT_ClothArmor)
#define MIsDecoration(pProtoType)	(pProtoType->eType == EIT_Decoration)
#define MIsArmor(pProtoType)		(pProtoType->eType == EIT_Armor)

ItemProduceFrame::ItemProduceFrame(void):m_Trunk(this)
{
	m_pWndMain		= NULL;
	m_pWndCaption	= NULL;
	m_pWndName		= NULL;
	m_pBtnExit		= NULL;
	m_pBtnOk		= NULL;
	m_pBtnCancel	= NULL;
	m_pStcNotice	= NULL;
	m_pTreeList		= NULL;
	m_pStcListTitle	= NULL;
	m_pStcSuccTitle	= NULL;
	m_pStcSuccCtrl	= NULL;
	m_pStcSucc		= NULL;
	m_pStcSuccOdds	= NULL;
	m_pStcModi		= NULL;
	m_pStcModiOdds	= NULL;
	m_pStcWhiteIdetifyPct  =  NULL;
	m_pStcYellowIdetifyPct =  NULL;
	m_pStcGreenIdetifyPct  =  NULL;
	m_pStcBlueIdetifyPct   =  NULL;
	m_pStcOrangeIdetifyPct =  NULL;
	m_pStcVioletIdetifyPct =  NULL;

	m_pStcNeedShow	= NULL;
	m_pStcGolden	= NULL;
	m_pStcSiller	= NULL;
	m_pProTime		= NULL;
	m_pEditComNum	= NULL;
	m_pStcPotValHint= NULL;
	m_pStcSuccMaster= NULL;
	m_pStcPotVal		= NULL;
	memset(m_pStcCtrl, 0x0, sizeof(m_pStcCtrl));
	memset(m_pPBtnFormular, 0x0, sizeof(m_pPBtnFormular));
	memset(m_pStcStuffHint, 0x0, sizeof(m_pStcStuffHint));
	memset(m_pStcStuffQuan, 0x0, sizeof(m_pStcStuffQuan));
	m_dwStartTime	= 0;
	m_eComType		= ECOMT_NULL;
	m_eProType		= EPCT_NULL;
	m_nProduceNum	= 1;
	m_eCurState		= EEItemProduce_NULL;
	m_eComposeType	= EStartCompose_NULL;
	m_escapeTime	= GT_INVALID;
	m_bUnChlik		= false;
}

ItemProduceFrame::~ItemProduceFrame(void)
{
	
}

BOOL ItemProduceFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	//--注册游戏事件
	m_pMgr->RegisterEventHandle(_T("tagUpdateLPSkillEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemProduceFrame::OnUpdateLPSkillEvent));
	m_pMgr->RegisterEventHandle(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemProduceFrame::OnEventProduceAddItem));

	//--注册网络消息
	m_pCmdMgr->Register("NS_ItemRemove", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvItemUpdate), 
		_T("NS_ItemRemove"));
	m_pCmdMgr->Register("NS_Produce", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvNS_Produce), 
		_T("NS_Produce"));
	m_pCmdMgr->Register("NS_Decomposition", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvNS_Decomposition), 
		_T("NS_Decomposition"));

	return TRUE;
}

BOOL ItemProduceFrame::Destroy()
{
	CloseAllUiEffect();
	HandleItem(true);
	GameFrame::Destroy();
	if( P_VALID(m_pWndMain) )
		m_pGUI->AddToDestroyList(m_pWndMain);

	//--注销游戏事件
	m_pMgr->UnRegisterEventHandler(_T("tagUpdateLPSkillEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemProduceFrame::OnUpdateLPSkillEvent));
	m_pMgr->UnRegisterEventHandler(_T("ItemEvent_Move2Compose"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ItemProduceFrame::OnEventProduceAddItem));

	//--游销网络消息
	m_pCmdMgr->UnRegister("NS_ItemRemove", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvItemUpdate));
	m_pCmdMgr->UnRegister("NS_Produce", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvNS_Produce));
	m_pCmdMgr->UnRegister("NS_Decomposition", (NETMSGPROC)m_Trunk.sfp2(&ItemProduceFrame::NetRecvNS_Decomposition));


	return TRUE;
}

BOOL ItemProduceFrame::EscCancel()
{
	if( m_eCurState==EEItemProduce_SendMsg )
	{
		ResetStartState();
		return TRUE;
	}
	else
	{
		DeleteFormula();
		ClearStuff();
		m_pFrameMgr->AddToDestroyList(this);
		SetStrategy(EIST_ItemPocket);
		return TRUE;
	}
	return FALSE;
}

BOOL ItemProduceFrame::ReloadUI()
{
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\compose.xml");
	m_pGUI->LoadXml(&element, "VFS_System", strPath.c_str());
	for(int i=0; i<CTRL_QUANTITY; i++)
	{
		char temp[256] = {0};
		sprintf(temp, "composewin\\cb_pic\\composepic\\item%d", i);
		m_pGUI->ChangeXml(&element, temp, "ClassName", "ComposeStatic");
	}
	m_pGUI->ChangeXml(&element, "composewin\\cb_pic\\composepic\\im", "ClassName", "ComposeStatic");
	m_pGUI->ChangeXml(&element, "composewin\\cb_pic\\directionpic\\direction", "ClassName", "AppliedTree");
	m_pWndMain = m_pGUI->CreateWnd(_T("\\desktop"), &element);

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWndMain->GetFullName().c_str(), m_Trunk.sfp1(&ItemProduceFrame::EventHandler));

	m_pWndCaption	= m_pWndMain->GetChild( _T("cb_pic") );
	m_pWndName		= (GUIStatic*)m_pWndCaption->GetChild( _T("win_name") );
	m_pBtnExit		= (GUIButton*)m_pWndCaption->GetChild( _T("closebutt") );

	m_pStcComBack	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic") );
	m_pBtnOk		= (GUIButton*)m_pWndCaption->GetChild( _T("composepic\\butt1") );
	m_pBtnOk->SetText(g_StrTable[_T("ProduceStart")]);
	m_pBtnCancel	= (GUIButton*)m_pWndCaption->GetChild( _T("composepic\\butt2") );
	m_pBtnCancel->SetEnable(false);
	m_pStcNotice	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\notic") );

	m_pStcTreeBack	= (GUIPatch*)m_pWndCaption->GetChild( _T("directionpic") );
	m_pTreeList		= (AppliedTree*)m_pWndCaption->GetChild( _T("directionpic\\direction") );
	m_pStcListTitle = (GUIStatic*)m_pWndCaption->GetChild( _T("directionpic\\title") );

	m_pStcSuccTitle	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\title") );
	m_pStcSuccCtrl		= (ComposeStatic*)m_pWndCaption->GetChild( _T("composepic\\im") );
	m_pStcSucc		= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word1") );
	m_pStcSuccOdds	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\odds1") );
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcModi		= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word2") );
	m_pStcModiOdds	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\odds2") );
	m_pStcModiOdds->SetText(_T(""));
	m_pStcWhiteIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num00") );
	m_pStcWhiteIdetifyPct->SetText(_T(""));
	m_pStcYellowIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num10") );
	m_pStcYellowIdetifyPct->SetText(_T(""));
    m_pStcGreenIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num20") );
	m_pStcGreenIdetifyPct->SetText(_T(""));
	m_pStcBlueIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num30") );
	m_pStcBlueIdetifyPct->SetText(_T(""));
	m_pStcOrangeIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num40") );
	m_pStcOrangeIdetifyPct->SetText(_T(""));
	m_pStcVioletIdetifyPct	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num50") );
	m_pStcVioletIdetifyPct->SetText(_T(""));
	
   
	m_pStcNeedShow	= (GUIStaticEx*)m_pWndCaption->GetChild( _T("composepic\\win\\word") );

	m_pEditComNum	= (GUIEditBox*)m_pWndCaption->GetChild( _T("composepic\\putinumpic\\putinum") );
	m_pStcConNum	= (GUIEditBox*)m_pWndCaption->GetChild( _T("composepic\\word12") );
	m_pStcSuccMaster= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num1") );
	m_pStcSuccMaster->SetText(_T(""));
	m_pStcMasterHint=(GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word4") );
	m_pStcPotValHint= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\word5") );
	//m_pStcPotValHint->SetText(g_StrTable[_T("ProduceVitality")]);
	m_pStcPotVal	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\num2") );
	m_pStcPotVal->SetText(_T(""));

	m_pStcGolden	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\money1") );
	m_pStcGolden->SetText(_T(""));
	m_pStcSiller	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\money2") );
	m_pStcSiller->SetText(_T(""));
	
	m_pProTime		= (GUIProgress*)m_pWndCaption->GetChild( _T("composepic\\timepic\\time") );
	m_pStcMainStuffHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\hide0") );
	m_pStcMainStuffHint->SetInvisible(TRUE);

	m_pStcSuccHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic4") );
	m_pStcSuccHint->SetInvisible(TRUE);
	m_pStcLoseHint	= (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic3") );
	m_pStcLoseHint->SetInvisible(TRUE);
	m_pStcPerfectHint = (GUIStatic*)m_pWndCaption->GetChild( _T("composepic\\pic5") );
	m_pStcPerfectHint->SetInvisible(TRUE);

	for(int i=0; i<CTRL_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("composepic\\item") << i;
		m_pStcCtrl[i] = (ComposeStatic*)m_pWndCaption->GetChild(szName.str().c_str());	
	}
	
	for(int i=0; i<FORMULA_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("com_fra\\optbutt") << i;
		m_pPBtnFormular[i] = (GUIPushButton*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pPBtnFormular[i]->SetInvisible(FALSE);
	}

	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		tstringstream szName;
		szName << _T("composepic\\itemnum") << i+1;
		m_pStcStuffQuan[i] = (GUIStatic*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		szName.str(_T(""));
		szName << _T("composepic\\hide") << i+1;
		m_pStcStuffHint[i] = (GUIStatic*)m_pWndCaption->GetChild(szName.str().c_str());
		m_pStcStuffHint[i]->SetInvisible(TRUE);
	}

	m_pWndMain->SetInvisible(TRUE);
	m_nProduceNum = 1;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();
	m_bUnChlik = false;

	return TRUE;
}

void ItemProduceFrame::SetStcCtrlState(EProduceType eType)
{
	if(eType == EPCT_PointUp)
	{
		for(int i=1; i<CTRL_QUANTITY; i++)
		{
			m_pStcCtrl[i]->SetInvisible(TRUE);
		}
		m_pStcCtrl[0]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
		m_pStcCtrl[0]->SetInvisible(FALSE);
		m_pStcConNum->SetInvisible(TRUE);
		m_pEditComNum->SetInvisible(TRUE);
		m_pStcSucc->SetInvisible(TRUE);
		m_pStcModiOdds->SetText(_T(""));
		m_pStcWhiteIdetifyPct->SetText(_T(""));
		m_pStcYellowIdetifyPct->SetText(_T(""));
		m_pStcGreenIdetifyPct->SetText(_T(""));	
		m_pStcBlueIdetifyPct->SetText(_T(""));
		m_pStcOrangeIdetifyPct->SetText(_T(""));	
		m_pStcVioletIdetifyPct->SetText(_T(""));

		m_pStcModi->SetInvisible(TRUE);
		//m_pStcModiOdds->SetInvisible(FALSE);
        //m_pStcWhiteIdetifyPct->SetInvisible(FALSE);
		//m_pStcYellowIdetifyPct->SetInvisible(FALSE);
		//m_pStcGreenIdetifyPct->SetInvisible(FALSE);
		//m_pStcBlueIdetifyPct->SetInvisible(FALSE);
		//m_pStcOrangeIdetifyPct->SetInvisible(FALSE);
		//m_pStcVioletIdetifyPct->SetInvisible(FALSE);

		m_pStcMasterHint->SetInvisible(FALSE);
		m_pStcSuccMaster->SetInvisible(FALSE);
		m_pStcPotValHint->SetInvisible(FALSE);
		m_pStcPotVal->SetInvisible(FALSE);
		switch( m_eComType )
		{
		case ECOMT_EquipdDecompose:
			{
				m_pStcMainStuffHint->SetText(g_StrTable[_T("Compose_EquipdDecomposeHint")]);
				m_pStcMainStuffHint->SetInvisible(FALSE);
			}
			break;
		case ECOMT_PointUp:
			{
				m_pStcMainStuffHint->SetText(g_StrTable[_T("Compose_PointUpHint")]);
				m_pStcMainStuffHint->SetInvisible(FALSE);
			}
			break;
		}
	}
	else 
	{
		for(int i=1; i<CTRL_QUANTITY; i++)
		{
			m_pStcCtrl[i]->SetInvisible(TRUE);
		}
		m_pStcCtrl[0]->SetInvisible(TRUE);
		m_pStcCtrl[0]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
		m_pStcConNum->SetInvisible(FALSE);
		m_pEditComNum->SetInvisible(FALSE);
		m_pStcSucc->SetInvisible(FALSE);
		m_pStcModiOdds->SetText(_T(""));
		m_pStcWhiteIdetifyPct->SetText(_T(""));
        m_pStcYellowIdetifyPct->SetText(_T(""));
		m_pStcGreenIdetifyPct->SetText(_T(""));
		m_pStcBlueIdetifyPct->SetText(_T(""));
		m_pStcOrangeIdetifyPct->SetText(_T(""));
		m_pStcVioletIdetifyPct->SetText(_T(""));

		
		m_pStcMainStuffHint->SetText(g_StrTable[_T("Compose_MainHint")]);
		m_pStcMainStuffHint->SetInvisible(FALSE);
		if( eType == EPCT_GodArtisan )
		{
			m_pStcModi->SetInvisible(TRUE);
			m_pStcModiOdds->SetInvisible(TRUE);
			m_pStcWhiteIdetifyPct->SetInvisible(TRUE);
			m_pStcYellowIdetifyPct->SetInvisible(TRUE);
			m_pStcGreenIdetifyPct->SetInvisible(TRUE);
			m_pStcBlueIdetifyPct->SetInvisible(TRUE);
			m_pStcOrangeIdetifyPct->SetInvisible(TRUE);
			m_pStcVioletIdetifyPct->SetInvisible(TRUE);
			
			m_pStcMasterHint->SetInvisible(TRUE);
			m_pStcSuccMaster->SetInvisible(TRUE);
			m_pStcPotValHint->SetInvisible(TRUE);
			m_pStcPotVal->SetInvisible(TRUE);
		}
		else if( eType == EPCT_EquipMaster || eType == EPCT_Gem )
		{
			m_pStcModi->SetInvisible(TRUE);
			m_pStcMasterHint->SetInvisible(TRUE);
			m_pStcSuccMaster->SetInvisible(TRUE);
			m_pStcPotValHint->SetInvisible(TRUE);
			m_pStcPotVal->SetInvisible(TRUE);
		}
		else
		{
			m_pStcModi->SetInvisible(TRUE);
			//m_pStcModiOdds->SetInvisible(FALSE);
			//m_pStcWhiteIdetifyPct->SetInvisible(FALSE);
			//m_pStcYellowIdetifyPct->SetInvisible(FALSE);
			//m_pStcGreenIdetifyPct->SetInvisible(FALSE);
			//m_pStcBlueIdetifyPct->SetInvisible(FALSE);
			//m_pStcOrangeIdetifyPct->SetInvisible(FALSE);
			//m_pStcVioletIdetifyPct->SetInvisible(FALSE);
			
			m_pStcMasterHint->SetInvisible(FALSE);
			m_pStcSuccMaster->SetInvisible(FALSE);
			m_pStcPotValHint->SetInvisible(FALSE);
			m_pStcPotVal->SetInvisible(FALSE);
		}
	}
}

VOID ItemProduceFrame::Update()
{
	GameFrame::Update();

	if( m_eComposeType == EStartCompose_NPC )
	{
		m_escapeTime += Kernel::Inst()->GetDeltaTimeDW();
		if( m_escapeTime > 1000 )//每隔1秒检测距离目标NPC的距离
		{
			m_escapeTime = 0;

			if (RoleMgr::Inst()->IsOutValidDist( m_uComposeID.npcID ) )
			{
				m_pWndMain->SetInvisible(TRUE);
				HandleItem(true);
				ClearStuff();
				DeleteFormula();
				SetStrategy(EIST_ItemPocket);
				m_pMgr->AddToDestroyList(this);
				return;
			}
		}
	}


	switch(m_eCurState)
	{
	case EEItemProduce_SendMsg:
		{
			float deltaTime =  Cool3D::Kernel::Inst()->GetDeltaTime();
			float fcur = m_pProTime->GetCurrentValue();
			float fMax = m_pProTime->GetMaxValue();
			if(fcur < fMax)
			{
				fcur += deltaTime;
				m_pProTime->SetValue(fcur, true);
				m_pProTime->SetRefresh();
			}
			//发送合成消息
			if(fcur >= fMax)
			{
				if( m_eProType == EPCT_PointUp )
					SendDeComposeMsg();
				else
					SendProduceMsg();
			}
		}
		break;
	case EEItemProduce_RecvMsg:
		{
			if(Kernel::Inst()->GetAccumTimeDW()- m_dwRecvMsgTime >= 1000)
			{
				m_pStcSuccHint->SetInvisible(TRUE);
				m_pStcLoseHint->SetInvisible(TRUE);
				m_pStcPerfectHint->SetInvisible(TRUE);
				if(m_nProduceNum>0)
				{
					m_pProTime->SetValue(0, true);
					m_pProTime->SetRefresh();
					if( m_eProType == EPCT_PointUp )
					{
	
					}
					else
					{
						if(ReCountStuff())
						{
							JudgeStuffIsContentForm(m_eProType, m_dwFormulaID);
						}
						else
						{
							//辅料不满足配方需求
							CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
							m_eCurState = EEItemProduce_NULL;
							m_pBtnOk->SetEnable(true);
							m_pBtnCancel->SetEnable(false);
							PushButtonState(true);
							m_pWndMain->SetRefresh(TRUE);
							m_pTreeList->SetCurAppliedTreeState();
						}
					}
				}
				else
				{
					if( m_eProType != EPCT_PointUp )
						ReCountStuff();
					else if( m_eProType == EPCT_PointUp )
					{
						DeleteFormula();
						ClearStuff();
					}
					ResetStartState();
				}// end if
			}// end if
		}
		break;
	default:
		break;
	}
}

VOID ItemProduceFrame::OnEvent(tagGameEvent* pEvent)
{

}

void ItemProduceFrame::SetOpenUIMode(EStartCompose eType, ComposeID uID)
{
	m_eComposeType = eType;
	m_uComposeID = uID;
	ClearStuff();
}

void ItemProduceFrame::ResetStartState()
{
	HandleItem(true);
	m_eCurState = EEItemProduce_NULL;
	m_pBtnOk->SetEnable(true);
	m_pBtnCancel->SetEnable(false);
	m_pProTime->SetValue(0, true);
	m_nProduceNum = 1;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();
	m_pWndMain->SetRefresh(TRUE);
	PushButtonState(true);
	m_pTreeList->SetCurAppliedTreeState();
}

BOOL ItemProduceFrame::EventHandler(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Click:
		{
			if( pWnd == m_pBtnExit )				//退出
			{
				HandleItem(true);
				DeleteFormula();
				ClearStuff();
				m_pFrameMgr->AddToDestroyList(this);
				SetStrategy(EIST_ItemPocket);
			}
			if( pWnd == m_pBtnOk )					//开始
			{
				JudgeStuffIsContentForm(m_eProType, m_dwFormulaID);
			}
			if( pWnd==m_pBtnCancel && m_eCurState==EEItemProduce_SendMsg )				//取消
			{
				ResetStartState();
			}
			
		}
		break;
	case EGUIE_PushButtonDown:
		{
			//如果是模拟发的Down事件,则不处理
			if(pEvent->dwParam1!=1)
				return false;

			if(m_eCurState!=EEItemProduce_NULL)
				return FALSE;

			if (m_bUnChlik)
			{
				m_bUnChlik = false;
				return false;
			}

			for(int i=0; i<FORMULA_QUANTITY; i++)
			{
				if( pWnd == m_pPBtnFormular[i] )
				{
					m_eComType = (EComposeType)(m_eProType*10+i);
					AddFormula2Tree(m_eProType, m_eComType);
					this->ClearStuff();
				}
			}
		}
		break;
	case EGUIE_ItemClick:
		{
			if( pWnd == m_pTreeList )
			{
				if(m_eCurState!=EEItemProduce_NULL)
					return FALSE;

				TreeFormularMap::iterator iter = m_tree2formula.find(pEvent->dwParam1);
				if(iter != m_tree2formula.end())
				{
					m_dwFormulaID = iter->second;
					SetStuffOnFormula(m_eProType, m_dwFormulaID);
				}
			}
		}
		break;
	case EGUIE_Drag:
		{
			if (_tcsnccmp(pWnd->GetName().c_str(), _T("item"), 4) == 0)
			{
				for(int i=0; i<CTRL_QUANTITY; i++)
				{
					if( pWnd == m_pStcCtrl[i] )
					{
						DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
						if( P_VALID(pDrag) )
						{
							pDrag->OnEvent(pEvent);
							break;
						}
					}
				}
			}
			else if(_tcsnccmp(pWnd->GetName().c_str(), _T("im"), 2) == 0)
			{
				if( pWnd == m_pStcSuccCtrl )
				{
					DragBtn* pDrag = (DragBtn*)m_pGUI->GetWnd(_T("\\desktop\\DragButton"));
					if( P_VALID(pDrag) )
					{
						pDrag->OnEvent(pEvent);
						break;
					}
				}
			}
		}
		break;
	case EOnEventSendDropGUI_Item:
		{
			if(m_eCurState!=EEItemProduce_NULL)
				return FALSE;

			return HandleDragStuff(pEvent);
		}
		break;
	case EGUIE_MoveIntoCtrl:
		{
			if( pWnd == m_pStcCtrl[0] )
			{
				switch(m_eProType)
				{
				case EPCT_Artisan:
				case EPCT_DanTraining:
				case EPCT_Smith:
				case EPCT_Casting:
				case EPCT_Dressmaker:
				case EPCT_Aechnics:
				case EPCT_Smilt:
				case EPCT_GodArtisan:
				case EPCT_Gem:
				case EPCT_Pet:
				case EPCT_EquipMaster:
				case EPCT_TreasureCom:
				case EPCT_FoodGod1:
				case EPCT_FoodGod2:
				case EPCT_FoodGod3:
				case EPCT_Perfume:
				case EPCT_EquipRefine:
				case EPCT_StuffRefine:
				case EPCT_TimeLimitEquip:
				case EPCT_Crystal:
				case EPCT_Stole_52:
				case EPCT_Stole_67:
				case EPCT_Stole_82:
				case EPCT_Stole_Blade:
				case EPCT_Stole_Bow:
				case EPCT_Stole_Valor:
				case EPCT_Stole_Swing:
				case EPCT_SoulLinker:
				case EPCT_MoonCake:
				case EPCT_HallowmasGift:
				case EPCT_HallowmasName:
				case EPCT_MoShenEquip:
				case EPCT_FairySoulStren://add by xp
				case EPCT_FairySkillbookStren://add end
				case EPCT_BattleFieldCompose://add end
				case EPCT_ReplaceFabao://add end
					{
						tagMainStuff mainStuff;
						m_Container.GetMainStuff(mainStuff);
						if( mainStuff.dwTypeID!=0 && mainStuff.dwTypeID!=GT_INVALID )
						{
							ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(mainStuff.dwTypeID, GT_INVALID).c_str());
						}
					}
					break;
				case EPCT_PointUp:
					{
						tagMainStuff mainStuff;
						m_Container.GetMainStuff(mainStuff);
						Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
						if( P_VALID(pEquip) )
						{
							ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pEquip).c_str());
						}
					}
					break;
				}
				

			}
			else if( pWnd == m_pStcSuccCtrl )
			{
				tagIMStuff  im;
				m_Container.GetIMStuff(im);
				Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(im.n64ID);
				if( P_VALID(pItem) )
				{
					ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
				}
			}
			else
			{
				for(int i=0; i<STUFF_QUANTITY; i++)
				{
					int nPos = i+1;
					if( pWnd == m_pStcCtrl[nPos] )
					{
						tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
						if(!P_VALID(pAssStuff))
							break;
						Item* pItem = ItemMgr::Inst()->GetPocketItemByID(pAssStuff->n64ID);
						if( P_VALID(pItem) )
						{
							ShowTip(pWnd, ToolTipCreator::Inst()->GetItemTips(pItem).c_str());
						}
						break;
					}
				}
			}
		}
		break;
	case EGUIE_MoveOutOfCtrl:
		{
			ShowTip(pWnd, NULL);
		}
		break;
	default:
		break;
	}

	return FALSE;
}

BOOL ItemProduceFrame::JudgeSpecItem(ESpecFuncComposeAdvance eSpec)
{
	if(eSpec==ESFCA_AllProduce || eSpec==EISFC_All)
		return TRUE;

	if((INT)m_eProType == (INT)eSpec)
		return TRUE;

	return FALSE;
}

BOOL ItemProduceFrame::HandleDragStuff(tagGUIEvent* pEvent)
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	// 放入特殊物品以提高生成成功率
	if( pWnd == m_pStcSuccCtrl )
	{	
		DWORD dwTypeID = pEvent->dwParam3;
		if(MIsEquipment(dwTypeID))
		{
			IMSG(_T("is not item, return!\n"));
			return FALSE;
		}

		INT16 nPos = INT16(pEvent->dwParam2);
		Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pItem))
			return FALSE;

		const tagItemProto* pItemProto = pItem->GetProto();
		if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
		{
			if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
				return FALSE;
		}
		else if( pItemProto->eSpecFunc!=EISF_ProtectSign )
		{
			return FALSE;
		}
		

		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcSuccCtrl->SetPic(strPic);

		tagIMStuff imStuff;
		imStuff.n64ID = pItem->GetItemId();

		m_Container.Add(imStuff);		

		m_pWndMain->SetRefresh(TRUE);
	}
	//只有点化才能放入主材料
	else if( pWnd==m_pStcCtrl[0] && m_eProType==EPCT_PointUp )
	{
		//判断是否为装备
		DWORD dwTypeID = pEvent->dwParam3;
		if(!MIsEquipment(dwTypeID))
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEquip")]);
			return FALSE;
		}

		INT16 nPos = INT16(pEvent->dwParam2);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pEquip))
			return FALSE;


		if( pEquip->IsNotIdetified() )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_NoIdetified")]);
			return FALSE;
		}

		ClearStuffSelectFormula();
		DeleteFormula();
		m_dwFormulaID = GT_INVALID;

		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcCtrl[0]->SetPic(strPic);

		tagMainStuff tagMainStuff;
		tagMainStuff.n64ID = pEquip->GetItemId();
		tagMainStuff.euqipPos = pEquip->GetEquipPos();
		m_Container.Add(tagMainStuff);

		//设置配方
		SetFormulaForPointUp();
	}
	else 
	{
		int i=0;
		for(; i<STUFF_QUANTITY; ++i)
		{
			int nNum = i+1;
			if( pWnd == m_pStcCtrl[nNum] )
			{
				DWORD dwTypeID = pEvent->dwParam3;
				if(MIsEquipment(dwTypeID))
				{
					IMSG(_T("is not item, return!\n"));
					return FALSE;
				}
				
				//在背包中的位置
				INT16 nPos = INT16( pEvent->dwParam2);
				Item* pItem = ItemMgr::Inst()->GetPocketItem(nPos);
				if(!P_VALID(pItem))
					return FALSE;

				//同堆物品不能放到不同的位置
				vector<tagAssistStuff> assStuff;
				m_Container.GetAssistStuffMap(assStuff);
				for(int j=0; j<(int)assStuff.size(); j++)
				{
					if(i!=j && assStuff[j].n64ID==pItem->GetItemId())
						return FALSE;
				}


				//背包中的数量
				INT16 nItemCount = INT16( (pEvent->dwParam2 & 0xFFFF000) >> 16 );

				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
				

				//判断配方需求辅料的类型与拖动释放的物品类型是否相同
				tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
				if(pAssStuff->stuffType!=EST_Null && pAssStuff->stuffType!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->stuffType == pItem->GetItemStuffType())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->dwTypeID = pItem->GetItemTypeID();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
					}
					else 
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				}
				else  if(pAssStuff->dwTypeID!=0 && pAssStuff->dwTypeID!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->dwTypeID == pItem->GetItemTypeID())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->stuffType = pItem->GetItemStuffType();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
					}
					else 
						CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
				}
				UtilCalculateSuccessRatio();
				m_pWndMain->SetRefresh(TRUE);
			}
		}
	}
	return TRUE;
}

VOID ItemProduceFrame::SetProduceMode(EProduceType eType)
{ 
	if( !P_VALID(m_pWndName) || !P_VALID(m_pStcMainStuffHint))
		return;
	m_eProType = eType;
	TCHAR szBuff[64];
	_stprintf(szBuff, _T("%s%d"), _T("ProduceType_"), m_eProType);
	m_pWndName->SetText(g_StrTable[szBuff]);
	m_nProduceNum	= 1;
	//设置配方
	SetFormula(m_eProType);
}

void ItemProduceFrame::SetFormula(EProduceType eType)
{
	for(int i=0; i<FORMULA_QUANTITY; i++)
	{
		m_pPBtnFormular[i]->SetInvisible(TRUE);
	}
	TCHAR szName[64];
	switch(eType)
	{
	case EPCT_Artisan:
		{
			for(int i=0; i<ArtisanSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_DanTraining:
		{
			for(int i=0; i<DanTrainingSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Smith:
		{
			for(int i=0; i<SmithSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Casting:
		{
			for(int i=0; i<CastingSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Dressmaker:
		{
			for(int i=0; i<DressmakerSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Aechnics:
		{
			for(int i=0; i<AechnicsSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Smilt:
		{
			for(int i=0; i<SmiltSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_PointUp:
		{
			for(int i=0; i<PointUpSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_GodArtisan:
		{
			for(int i=0; i<GodArtisanSubTypeQuan; i++)
			{
				if(i >= FORMULA_QUANTITY) // 防止数组越界
					break;

				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Gem:
		{
			for(int i=0; i<GodArtisanSubTypeQuan; i++)
			{
				if(i >= FORMULA_QUANTITY) // 防止数组越界
					break;

				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), 9*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Pet:
		{
			for(int i=0; i<PetSubTypeQuan; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_EquipMaster:
		{
			for( int i = 0; i < EquipSubTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_EquipRefine:
		{
			for( int i = 0; i < EquipRefineTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_StuffRefine:
		{
			for( int i = 0; i < StuffRefineTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_TimeLimitEquip:
		{
			for( int i = 0; i < TimeLimitEquipTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_FoodGod1:
		{
			for( int i = 0; i < FoodProduceTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_FoodGod2:
		{
			for( int i = 0; i < SpriteFoodProduceTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_FoodGod3:
		{
			for( int i = 0; i < ChristmasWeaponTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Perfume:
		{
			for( int i = 0; i < SpriteSoulTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_TreasureCom:
		{
			m_pPBtnFormular[0]->SetInvisible(FALSE);
			_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType);
			m_pPBtnFormular[0]->SetText(g_StrTable[szName]);
		}
		break;
	case EPCT_Crystal:
		{
			for( int i = 0; i < CryStalTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_52:
		{
			for( int i = 0; i < Stole_52TypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_67:
		{
			for( int i = 0; i < Stole_67TypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_82:
		{
			for( int i = 0; i < Stole_82TypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_Blade:
		{
			for( int i = 0; i < Stole_BladeTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_Bow:
		{
			for( int i = 0; i < Stole_BowTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_Valor:
		{
			for( int i = 0; i < Stole_ValorTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_Stole_Swing:
		{
			for( int i = 0; i < Stole_SwingTypeQuan; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_SoulLinker:
		{
			for( int i = 0; i < Stole_SoulLinker; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_MoonCake:
		{
			for( int i = 0; i < Stole_MoonCake; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_HallowmasGift:
		{
			for( int i = 0; i < HallowmasGift; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_HallowmasName:
		{
			for( int i = 0; i < HallowmasName; i++ )
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_MoShenEquip:
		{
			for(int i = 0; i < TaiWamMoshenEquip; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}
		}
		break;
	case EPCT_FairySoulStren://add by xp
		{
			for(int i = 0; i < FairySoulStren; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}

		}
		break;
	case EPCT_FairySkillbookStren:
		{
			for(int i = 0; i < FairySkillbookStren; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}

		}
		break;//add end
	case EPCT_BattleFieldCompose:
		{
			for(int i = 0; i < BattleFieldCompose; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}

		}
		break;//add end
	case EPCT_ReplaceFabao:
		{
			for(int i = 0; i < ReplaceFabao; i++)
			{
				m_pPBtnFormular[i]->SetInvisible(FALSE);
				_stprintf(szName, _T("%s%d"), _T("ComposeType_"), eType*10+i);
				m_pPBtnFormular[i]->SetText(g_StrTable[szName]);
			}

		}
		break;//add end
	default:
		IMSG(_T("ItemSmithComposeFrame SetFormula type error!\n"));
		break;
	}
	//发送点击第一个pushbutton按钮消息
	m_eComType = (EComposeType)(eType*10);
	m_pPBtnFormular[0]->SetState(EGUIBS_PushDown);
}

void ItemProduceFrame::SetFormulaForPointUp()
{
	//首先清空树控件保存的配方ID
	DeleteFormula();

	DWORD treeID = GT_INVALID;

	if( m_tree2formula.empty() )	
	{
		m_pTreeList->InsterItem(0, _T("Root"), _T("RootNode"));
	}

	tagMainStuff tagMainStuff;
	m_Container.GetMainStuff(tagMainStuff);
	
	DWORD dwSkillLvl = MTransSkillLevel(m_uComposeID.skillID);

	//
	Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(tagMainStuff.n64ID);
	if(!P_VALID(pEquip))
		return ;

	if( MIsRWeapon(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		ProduceData::Inst()->GetDeComposeMap(decomposeMap);
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10 )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.eTypeEx==pEquip->GetItemTypeEx()
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10 )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsClothArmor(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		ProduceData::Inst()->GetDeComposeMap(decomposeMap);
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_ClothArmor )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsDecoration(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		ProduceData::Inst()->GetDeComposeMap(decomposeMap);
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Decoration )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
	else if( MIsArmor(pEquip->GetProto()) )
	{
		map<DWORD, tagDeComposeProto> decomposeMap;
		ProduceData::Inst()->GetDeComposeMap(decomposeMap);
		for(map<DWORD, tagDeComposeProto>::iterator it=decomposeMap.begin();
			it!=decomposeMap.end(); it++)
		{
			tagDeComposeProto decomposeProto = it->second;
			if(decomposeProto.eComType == m_eComType)
			{
				switch(m_eComposeType)
				{
				case EStartCompose_NPC:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_NPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Skill:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Skill
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorNPC
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor
							&& decomposeProto.dwFormLvl <= dwSkillLvl )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				case EStartCompose_Item:
					{
						if( (decomposeProto.eFormFrom==EFormulaFrom_Item
							|| decomposeProto.eFormFrom==EFormulaFrom_SkillorItem
							|| decomposeProto.eFormFrom==EFormulaFrom_NPCorItem)
							&& decomposeProto.byQuality==pEquip->GetItemQuality()
							&& decomposeProto.ePos==pEquip->GetEquipPos()
							&& decomposeProto.byLevel == pEquip->GetItemLevel()/10
							&& decomposeProto.eType == EIT_Armor )
						{
							TCHAR szText[64];
							_stprintf(szText, _T("DeCompose_%d"), decomposeProto.dwID);
							treeID = m_pTreeList->InsterItem(_T("\\Root"), szText, decomposeProto.szName);
							m_tree2formula[treeID] = decomposeProto.dwID;
						}
					}
					break;
				}
			}
		}
	}
}

void ItemProduceFrame::AddFormula2Tree( EProduceType eProType, EComposeType eComType )
{
	//首先清空树控件保存的配方ID
	DeleteFormula();

	DWORD treeID = GT_INVALID;

	if( m_tree2formula.empty() )	
	{
		m_pTreeList->InsterItem(0, _T("Root"), _T("RootNode"));
	}
	TCHAR szBuff[64];
	TCHAR szPath[64];
	TCHAR szText[64];
	switch(eProType)
	{
	case EPCT_Artisan:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Artisan);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Artisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Artisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Artisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_DanTraining:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_DanTraining);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("DanTraining_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("DanTraining_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("DanTraining_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Smith:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Smith);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smith_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smith_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smith_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Casting:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Casting);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Casting_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Casting_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Casting_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Dressmaker:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Dressmaker);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Dressmaker_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Dressmaker_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Dressmaker_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Aechnics:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Aechnics);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Aechnics_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Aechnics_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Aechnics_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Smilt:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Smilt);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smilt_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smilt_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Smilt_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_GodArtisan:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_GodArtisan);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Gem:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Gem);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("GodArtisan_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Pet:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Pet);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Pet_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Pet_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Pet_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_EquipMaster:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_EquipMaster);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipMaster_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipMaster_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipMaster_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_EquipRefine:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_EquipRefine);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("EquipRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_StuffRefine:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_StuffRefine);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_TimeLimitEquip:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_TimeLimitEquip);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_FoodGod1:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_FoodGod1);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_FoodGod2:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_FoodGod2);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_FoodGod3:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_FoodGod3);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Perfume:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Perfume);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_TreasureCom:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(eProType);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==(eComType/10))
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Sprite_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Sprite_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("Sprite_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Crystal:
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Crystal);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	case EPCT_Stole_52:
	case EPCT_Stole_67:
	case EPCT_Stole_82:
	case EPCT_Stole_Blade:
	case EPCT_Stole_Bow:
	case EPCT_Stole_Valor:
	case EPCT_Stole_Swing:
	case EPCT_SoulLinker:
	case EPCT_MoonCake:
	case EPCT_HallowmasGift:
	case EPCT_HallowmasName:
	case EPCT_MoShenEquip:
	case EPCT_FairySoulStren://add by xp
	case EPCT_FairySkillbookStren://add by xp
	case EPCT_BattleFieldCompose://add by xp
	case EPCT_ReplaceFabao://add by xp
		{
			const map<DWORD, tagProduceProto>* proMap;
			proMap = ProduceData::Inst()->GetProduceMap(EPCT_Stole_52);
			for(map<DWORD, tagProduceProto>::const_iterator iter= proMap->begin();
				iter!=proMap->end(); ++iter)
			{
				const tagProduceProto* proProto = &iter->second;
				if(proProto->eComType==eComType)
				{
					switch(m_eComposeType)
					{
					case EStartCompose_NPC:
						{
							if( proProto->eFormFrom==EFormulaFrom_NPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Skill:
						{
							if( (proProto->eFormFrom==EFormulaFrom_Skill
								|| proProto->eFormFrom==EFormulaFrom_SkillorNPC
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem)
								&& proProto->dwFormLvl <= MTransSkillLevel(m_uComposeID.skillID) )
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					case EStartCompose_Item:
						{
							if( proProto->eFormFrom==EFormulaFrom_Item
								|| proProto->eFormFrom==EFormulaFrom_SkillorItem
								|| proProto->eFormFrom==EFormulaFrom_NPCorItem)
							{
								_stprintf(szBuff, _T("%s%d"), _T("FormulaType_"), proProto->eFormType);
								m_pTreeList->InsterItem(_T("\\Root"), szBuff, g_StrTable[szBuff], FALSE, 0xFFFFF2A4);
								_stprintf(szPath, _T("%s%s"), _T("\\Root\\"), szBuff);
								_stprintf(szText, _T("StuffRefine_%d"), proProto->dwID);
								treeID = m_pTreeList->InsterItem(szPath, szText, proProto->szName);
								m_tree2formula[treeID] = proProto->dwID;
							}
						}
						break;
					}
				}
			}
		}
		break;
	default:
		IMSG(_T("ItemSmithComposeFrame AddFormula2Tree type error!\n"));
		break;
	}
}

void ItemProduceFrame::DeleteFormula()
{
	TreeFormularMap::iterator iter;
	for( iter=m_tree2formula.begin(); iter != m_tree2formula.end();  )
	{
		m_pTreeList->RemoveItem( iter->first );
		iter = m_tree2formula.erase(iter);
	}
	m_pTreeList->SetRefresh();
}

void ItemProduceFrame::SetStuffOnFormula( EProduceType eType, DWORD dwID )
{
	
	switch(eType)
	{
	case EPCT_GodArtisan:
	case EPCT_Gem:
		{
			tstringstream notice;
			notice << g_StrTable[_T("Consolidate_notice5")] << _T("\n");
			m_pStcNotice->SetText(notice.str().c_str());
			notice.clear();
		}
	case EPCT_Artisan:
	case EPCT_DanTraining:
	case EPCT_Smith:
	case EPCT_Casting:
	case EPCT_Dressmaker:
	case EPCT_Aechnics:
	case EPCT_Smilt:
	case EPCT_Pet:
	case EPCT_EquipMaster:
	case EPCT_TreasureCom:
	case EPCT_FoodGod1:
	case EPCT_FoodGod2:
	case EPCT_FoodGod3:
	case EPCT_Perfume:
	case EPCT_EquipRefine:
	case EPCT_StuffRefine:
	case EPCT_TimeLimitEquip:
	case EPCT_Crystal:
	case EPCT_Stole_52:
	case EPCT_Stole_67:
	case EPCT_Stole_82:
	case EPCT_Stole_Blade:
	case EPCT_Stole_Bow:
	case EPCT_Stole_Valor:
	case EPCT_Stole_Swing:
	case EPCT_SoulLinker:
	case EPCT_MoonCake:
	case EPCT_HallowmasGift:
	case EPCT_HallowmasName:
	case EPCT_MoShenEquip:
	case EPCT_FairySoulStren://add by xp
	case EPCT_FairySkillbookStren://add by xp
	case EPCT_BattleFieldCompose://add by xp
	case EPCT_ReplaceFabao://add by xp
		{
			ClearStuffSelectFormula();
			const tagProduceProto* pProProto = NULL;
			pProProto = ProduceData::Inst()->FindProduceProto(eType, dwID);
			if( !P_VALID(pProProto) )
				return;
			//设置生产出的物品
			if( MIsEquipment(pProProto->dwProItemTypeID) )
			{
				tagMainStuff stuff;
				stuff.dwTypeID = pProProto->dwProItemTypeID;
				tstring strPic = ItemMgr::Inst()->GetItemIconPath(stuff.dwTypeID);
				m_pStcCtrl[0]->SetPic(strPic);
				m_pStcCtrl[0]->SetText(_T(""));
				m_pStcCtrl[0]->SetInvisible(FALSE);
				m_Container.Add(stuff);
				if( eType!=EPCT_GodArtisan && eType!=EPCT_Gem)
				{
					tstringstream notice;
					notice << g_StrTable[_T("Consolidate_notice3")]; // << g_StrTable[_T("Consolidate_notice4")];
					m_pStcNotice->SetText(notice.str().c_str());
					notice.clear();
				}
			}
			else 
			{
				tagMainStuff stuff;
				stuff.dwTypeID = pProProto->dwProItemTypeID;
				tstring strPic = ItemMgr::Inst()->GetItemIconPath(stuff.dwTypeID);
				m_pStcCtrl[0]->SetPic(strPic);
				TCHAR szBuff[X_SHORT_NAME] = {0};
				_sntprintf( szBuff, X_SHORT_NAME, _T("%d"), pProProto->dwProQuantity );
				m_pStcCtrl[0]->SetText(szBuff);
				m_pStcCtrl[0]->SetInvisible(FALSE);
				m_Container.Add(stuff);
				if( eType!=EPCT_GodArtisan && eType!=EPCT_Gem)
				{
					tstringstream notice;
					notice << g_StrTable[_T("Consolidate_notice2")] << _T("\n");
					m_pStcNotice->SetText(notice.str().c_str());
					notice.clear();
				}
			}

			//设置材料
			for(int i=0; i<MAX_PRODUCE_STUFF_QUANTITY; i++)
			{
				//有材料类型
				EStuffType eStuffType = pProProto->ProduceStuff[i].eStuffType;
				if(eStuffType!=EST_Null && eStuffType!=GT_INVALID)
				{
					TCHAR szText[64];
					//--材料提示
					_stprintf(szText, _T("%s%d"), _T("StuffType_"), eStuffType);
					m_pStcStuffHint[i]->SetTextColor(0xFFFCF157);
					m_pStcStuffHint[i]->SetText(g_StrTable[szText]);
					m_pStcStuffHint[i]->SetInvisible(FALSE);
					//--材料数量
					_stprintf(szText, _T("%d/%d"), 0, pProProto->ProduceStuff[i].dwStuffNum);
					m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
					m_pStcStuffQuan[i]->SetText(szText);
					m_pStcStuffQuan[i]->SetInvisible(FALSE);
					//--材料控件
					m_pStcCtrl[i+1]->SetInvisible(FALSE);

					tagAssistStuff tagConso;
					tagConso.stuffType = eStuffType;
					tagConso.nQuantity = pProProto->ProduceStuff[i].dwStuffNum;
					//缓存强化辅料数据
					m_Container.Add(tagConso);
				}
				//有材料的TypeID
				else if(pProProto->ProduceStuff[i].dwStuffID!=0 
					&& pProProto->ProduceStuff[i].dwStuffID!=GT_INVALID)
				{
					TCHAR szText[64];
					//--材料提示(从背包中获取该TypeID材料的材料类型)
					tstring strName =  ItemProtoData::Inst()->FindItemDisplay(pProProto->ProduceStuff[i].dwStuffID)->szName;
					BYTE byQuality = ItemProtoData::Inst()->FindItemProto(pProProto->ProduceStuff[i].dwStuffID)->byQuality;
					DWORD dwColor = ComposeMgr::Inst()->GetColorByQuality(byQuality);
					m_pStcStuffHint[i]->SetText(strName.c_str());
					m_pStcStuffHint[i]->SetInvisible(FALSE);
					//--材料数量
					_stprintf(szText, _T("%d/%d"), 0, pProProto->ProduceStuff[i].dwStuffNum);
					m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
					m_pStcStuffQuan[i]->SetText(szText);
					m_pStcStuffQuan[i]->SetInvisible(FALSE);
					//--材料控件
					m_pStcCtrl[i+1]->SetInvisible(FALSE);

					tagAssistStuff tagProStuff;
					tagProStuff.dwTypeID = pProProto->ProduceStuff[i].dwStuffID;
					tagProStuff.nQuantity = pProProto->ProduceStuff[i].dwStuffNum;

					//缓存强化辅料数据
					m_Container.Add(tagProStuff);
				}
			}// end for

			//--成功率
			TCHAR szText[64];
			_stprintf( szText, _T("%d%%"),  pProProto->nSuccessRate/100);
			m_pStcSuccOdds->SetText(szText);

			//--金钱消耗
			DWORD dwMoney = pProProto->dwMoneyConsume;
			DWORD dwGolden = MSilver2DBGold(dwMoney);
			DWORD dwSiller = MSilver2DBSilver(dwMoney);
			_stprintf( szText, _T("%u"), dwGolden );
			m_pStcGolden->SetText(szText);
			_stprintf( szText, _T("%u"), dwSiller );
			m_pStcSiller->SetText(szText);

			//--活力值消耗
			_stprintf( szText, _T("%u"),  pProProto->dwVitalityConsume);
			m_pStcPotVal->SetText(szText);

			//--熟练度增加
			_stprintf( szText, _T("%u"), pProProto->dwMasterIncre );
			m_pStcSuccMaster->SetText(szText);

			//--生产物品介绍
			const tagItemDisplayInfo* pInfo = ItemProtoData::Inst()->FindItemDisplay(pProProto->dwProItemTypeID);
			if( P_VALID(pInfo) )
			{
				tstringstream strHint;
				strHint << pInfo->szName <<  _T(" X ") << pProProto->dwProQuantity <<_T("\\n");
				strHint <<g_StrTable[_T("Compose_FormulaLvl")] << pProProto->dwFormLvl <<_T("\\n");
				strHint << pInfo->szDesc << _T("\\n");
				m_pStcNeedShow->SetText(strHint.str().c_str());

				tagPoint size = m_pStcNeedShow->GetSize();
				if( m_pStcNeedShow->GetTextTotalHeight() > size.y )
				{
					size.y = m_pStcNeedShow->GetTextTotalHeight();
					m_pStcNeedShow->SetResizable(TRUE);
					m_pStcNeedShow->Resize( size );
					m_pStcNeedShow->SetResizable(FALSE);
				}

				m_pStcNeedShow->SetRefresh(TRUE);
			}
		}
		break;
	case EPCT_PointUp:
		{
			const tagDeComposeProto* pDeComposeProto = NULL;
			pDeComposeProto = ProduceData::Inst()->FinDeComposeProto(dwID);
			if( !P_VALID(pDeComposeProto) )
				return;

			TCHAR szText[64];
			//--金钱消耗
			DWORD dwMoney = pDeComposeProto->dwMoneyConsume;
			DWORD dwGolden = dwMoney/10000;
			DWORD dwSiller = dwMoney - dwGolden;
			_stprintf( szText, _T("%u"), dwGolden );
			m_pStcGolden->SetText(szText);
			_stprintf( szText, _T("%u"), dwSiller );
			m_pStcSiller->SetText(szText);

			//--活力值消耗
			_stprintf( szText, _T("%u"),  pDeComposeProto->dwVitalityConsume);
			m_pStcPotVal->SetText(szText);

			//--熟练度增加
			_stprintf( szText, _T("%u"), pDeComposeProto->dwMasterIncre );
			m_pStcSuccMaster->SetText(szText);

			//--提示
			TCHAR szBuff[64];
			tstringstream strHint;
			_stprintf( szBuff, _T("DeComposeHint_%d"), m_eComType );
			strHint << g_StrTable[szBuff] << _T("\n");
			for(int i=0; i<MAX_PRODUCE_STUFF_QUANTITY; i++)
			{
				DWORD dwTypeID = pDeComposeProto->OutputStuff[i].dwStuffTypeID;
				if(dwTypeID!=GT_INVALID)
				{
					const tagItemDisplayInfo* pDisplayInfo = ItemProtoData::Inst()->FindItemDisplay(dwTypeID);
					if( P_VALID(pDisplayInfo) )
					{
						strHint << pDisplayInfo->szName << _T("\n");
					}
				}
			}
			m_pStcNeedShow->SetText(strHint.str().c_str());

			tagPoint size = m_pStcNeedShow->GetSize();
			if( m_pStcNeedShow->GetTextTotalHeight() > size.y )
			{
				size.y = m_pStcNeedShow->GetTextTotalHeight();
				m_pStcNeedShow->SetResizable(TRUE);
				m_pStcNeedShow->Resize( size );
				m_pStcNeedShow->SetResizable(FALSE);
			}

			m_pStcNeedShow->SetRefresh(TRUE);
		}
		break;
	default:
		IMSG(_T("ItemProduceFrame SetStuffOnFormula type error!\n"));
		break;
	}
}

void ItemProduceFrame::ClearStuffSelectFormula()
{
	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		m_pStcStuffHint[i]->SetText(_T(""));
		m_pStcStuffHint[i]->SetInvisible(TRUE);
		m_pStcStuffQuan[i]->SetText(_T(""));		
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));	
	}
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcNeedShow->SetText(_T(""));
	m_pStcPotVal->SetText(_T(""));
	m_pStcSuccMaster->SetText(_T(""));
	m_pProTime->SetValue(0, true);
	m_nProduceNum = 1;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();
	m_eCurState	= EEItemProduce_NULL;
	m_Container.ClearAssistStuff();
	m_pStcSuccCtrl->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
	//m_dwFormulaID = GT_INVALID;
	this->SetStcCtrlState(m_eProType);
}

void ItemProduceFrame::ClearStuff()
{
	ResetStartState();
	for(int i=0; i<STUFF_QUANTITY; i++)
	{
		m_pStcStuffHint[i]->SetText(_T(""));
		m_pStcStuffHint[i]->SetInvisible(TRUE);
		m_pStcStuffQuan[i]->SetText(_T(""));
		m_pStcStuffQuan[i]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetInvisible(TRUE);
		m_pStcCtrl[i+1]->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));	
	}
	m_pStcSuccOdds->SetText(_T(""));
	m_pStcNeedShow->SetText(_T(""));
	m_pStcPotVal->SetText(_T(""));
	m_pStcSuccMaster->SetText(_T(""));
	m_pProTime->SetValue(0, true);
	m_nProduceNum = 1;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();
	m_eCurState	= EEItemProduce_NULL;
	m_Container.ClearAllStuff();
	m_dwFormulaID = GT_INVALID;
	m_pStcSuccCtrl->SetPic(_T("data\\ui\\main\\l_icon_back.bmp"));
	this->SetStcCtrlState(m_eProType);
}

void ItemProduceFrame::JudgeStuffIsContentForm(EProduceType eType, DWORD dwID)
{
	switch(eType)
	{
	case EPCT_Artisan:
	case EPCT_DanTraining:
	case EPCT_Smith:
	case EPCT_Casting:
	case EPCT_Dressmaker:
	case EPCT_Aechnics:
	case EPCT_Smilt:
	case EPCT_GodArtisan:
	case EPCT_Gem:
	case EPCT_Pet:
	case EPCT_EquipMaster:
	case EPCT_TreasureCom:
	case EPCT_FoodGod1:
	case EPCT_FoodGod2:
	case EPCT_FoodGod3:
	case EPCT_Perfume:
	case EPCT_EquipRefine:
	case EPCT_StuffRefine:
	case EPCT_TimeLimitEquip:
	case EPCT_Crystal:
	case EPCT_Stole_52:
	case EPCT_Stole_67:
	case EPCT_Stole_82:
	case EPCT_Stole_Blade:
	case EPCT_Stole_Bow:
	case EPCT_Stole_Valor:
	case EPCT_Stole_Swing:
	case EPCT_SoulLinker:
	case EPCT_MoonCake:
	case EPCT_HallowmasGift:
	case EPCT_HallowmasName:
	case EPCT_MoShenEquip:
	case EPCT_FairySoulStren://add by xp
	case EPCT_FairySkillbookStren://add by xp
	case EPCT_BattleFieldCompose://add by xp
	case EPCT_ReplaceFabao://add by xp
		{
			TCHAR szBuff[32];
			_tcscpy(szBuff, m_pEditComNum->GetText());
			m_nProduceNum = _ttoi(szBuff);
			if( m_nProduceNum <= 0 )
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateComNumError")]);
				return;
			}
			const tagProduceProto* pProProto = NULL;
			pProProto = ProduceData::Inst()->FindProduceProto(eType, dwID);	
			//没有配方不能生产
			if( !P_VALID(pProProto) )
				return;
			//判断钱
			INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
			if( n64Silver <  pProProto->dwMoneyConsume)
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
				return;
			}

			//判断技能的活力值

			vector<tagAssistStuff> assStuff;
			m_Container.GetAssistStuffMap(assStuff);
			bool bSendMsg = true;
			if( (int)assStuff.size()==0 )
				bSendMsg = false;
			for(int i=0; i<(int)assStuff.size(); i++)
			{
				if(assStuff[i].nQuantity > assStuff[i].nBagQuan )
				{
					bSendMsg = false;
					break;
				}
			}
			if(bSendMsg && 	m_nProduceNum>0)
			{
				HandleItem(false);
				m_pProTime->SetInvisible(FALSE);
				m_pProTime->SetSpeed(100);
				m_pProTime->SetValue(0, true);
				m_pProTime->SetMaxValue((FLOAT)pProProto->dwProduceTime/1000);
				m_pProTime->SetRefresh();
				m_eCurState = EEItemProduce_SendMsg;
				m_pBtnOk->SetEnable(false);
				m_pBtnCancel->SetEnable(true);
				PushButtonState(false);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_NoOperate);
				m_pWndMain->SetRefresh(TRUE);

				// 开始读条音效
				TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);
			}
			else
			{
				//辅料不满足配方需求
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
			}
		}
		break;
	case EPCT_PointUp:
		{
			const tagDeComposeProto* pDeComposeProto = NULL;
			pDeComposeProto = ProduceData::Inst()->FinDeComposeProto(dwID);	
			if( !P_VALID(pDeComposeProto) )
				return;
			//判断钱
			INT64 n64Silver = CurrencyMgr::Inst()->GetBagSilver();
			if( n64Silver <  pDeComposeProto->dwMoneyConsume)
			{
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateMoneyError")]);
				return;
			}

			//判断技能的活力值
			bool bSendMsg = true;

			if(bSendMsg && 	m_nProduceNum>0)
			{
				HandleItem(false);
				TCHAR szBuff[32];
				_tcscpy(szBuff, m_pEditComNum->GetText());
				m_nProduceNum = _ttoi(szBuff);
				m_pProTime->SetInvisible(FALSE);
				m_pProTime->SetSpeed(100);
				m_pProTime->SetValue(0, true);
				m_pProTime->SetMaxValue((FLOAT)pDeComposeProto->dwProduceTime/1000);
				m_pProTime->SetRefresh();
				m_eCurState = EEItemProduce_SendMsg;
				m_pBtnOk->SetEnable(false);
				m_pBtnCancel->SetEnable(true);
				PushButtonState(false);
				m_pTreeList->SetCurAppliedTreeState(AppliedTree::EAppliedTree_NoOperate);
				m_pWndMain->SetRefresh(TRUE);

				// 读条音效
				TObjRef<AudioSys>()->Play2DSound(_T("make_item"), SOUND_NORMAL);

			}
			else
			{
				//辅料不满足配方需求
				CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
			}
		}
		break;
	}
}

void ItemProduceFrame::SendProduceMsg()
{
	m_pProTime->SetValue(0, true);
	TCHAR szMsg[1024] = {0};
	tagNC_Produce* pMsg = (tagNC_Produce*)szMsg;
	pMsg->dwID		= m_pUtil->Crc32("NC_Produce");
	pMsg->dwFormulaID = m_dwFormulaID;
	switch(m_eComposeType)
	{
	case EStartCompose_NPC:
		pMsg->dwNPCID = m_uComposeID.npcID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	case EStartCompose_Skill:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = m_uComposeID.skillID;
		break;
	case EStartCompose_Item:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID =m_uComposeID.itemID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	}

	tagIMStuff imStuff;
	m_Container.GetIMStuff(imStuff);
	pMsg->n64IMID = imStuff.n64ID;

	vector<tagAssistStuff> assMap;
	m_Container.GetAssistStuffMap(assMap);
	int nStuffQuan = 0;
	for(vector<tagAssistStuff>::iterator it=assMap.begin(); 
		it!=assMap.end(); ++it)
	{
		if( (it->stuffType!=EST_Null || it->dwTypeID!=GT_INVALID)
			&& it->nBagQuan>=it->nQuantity)
		{
			pMsg->n64StuffID[nStuffQuan]  = it->n64ID;
			nStuffQuan++;
		}
	}

	pMsg->dwSize = sizeof(tagNC_Produce) + sizeof(INT64)*(nStuffQuan-1);

	m_pSession->Send(pMsg);
	m_eCurState = EEItemProduce_Waiting;
	m_nProduceNum--;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();
}

void ItemProduceFrame::SendDeComposeMsg()
{
	m_pProTime->SetValue(0, true);
	TCHAR szMsg[1024] = {0};
	tagNC_Decomposition *pMsg = (tagNC_Decomposition*)szMsg;
	pMsg->dwID		= m_pUtil->Crc32("NC_Decomposition");
	pMsg->dwFormulaID = m_dwFormulaID;

	switch(m_eComposeType)
	{
	case EStartCompose_NPC:
		pMsg->dwNPCID = m_uComposeID.npcID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	case EStartCompose_Skill:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID = GT_INVALID;
		pMsg->dwSkillID = m_uComposeID.skillID;
		break;
	case EStartCompose_Item:
		pMsg->dwNPCID = GT_INVALID;
		pMsg->n64ItemID =m_uComposeID.itemID;
		pMsg->dwSkillID = GT_INVALID;
		break;
	}
	
	tagIMStuff imStuff;
	m_Container.GetIMStuff(imStuff);
	pMsg->n64IMID = imStuff.n64ID;

	tagMainStuff mainStuff;
	m_Container.GetMainStuff(mainStuff);
	pMsg->n64Item = mainStuff.n64ID;

	pMsg->dwSize = sizeof(tagNC_Decomposition);

	m_pSession->Send(pMsg);
	m_eCurState = EEItemProduce_Waiting;
	m_nProduceNum--;
	TCHAR szBuff[32];
	m_pEditComNum->SetText(_itot(m_nProduceNum, szBuff, 10));
	m_pEditComNum->SetRefresh();

}

bool ItemProduceFrame::ReCountStuff()
{
	vector<tagAssistStuff> assStuff;
	m_Container.GetAssistStuffMap(assStuff);
	TCHAR szText[64];
	bool bContinue = true;
	for(int i=0; i<(int)assStuff.size(); i++)
	{
		if(assStuff[i].nQuantity > assStuff[i].nBagQuan)
		{
			//--材料数量
			_stprintf(szText, _T("%d/%d"), assStuff[i].nBagQuan, assStuff[i].nQuantity);
			m_pStcStuffQuan[i]->SetText(szText);
			m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
			bContinue = false;
		}
		else
		{
			//--材料数量
			_stprintf(szText, _T("%d/%d"), assStuff[i].nBagQuan, assStuff[i].nQuantity);
			m_pStcStuffQuan[i]->SetText(szText);
		}
	}
	return bContinue;
}

DWORD ItemProduceFrame::NetRecvItemUpdate(tagNS_ItemRemove* pMsg, DWORD pPrama)
{
	if(pMsg->eConType != EICT_Bag && pMsg->eConType != EICT_WarBag)
		return 0;

	tagAssistStuff* assStuff = NULL;
	assStuff = m_Container.GetAssistStuff(pMsg->n64Serial);
	if(!P_VALID(assStuff))
		return 0;

	assStuff->nBagQuan = pMsg->n16Num;
	return 0;
}

DWORD ItemProduceFrame::NetRecvNS_Produce(tagNS_Produce* pMsg, DWORD pPrama)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			//m_pStcSuccHint->SetInvisible(FALSE);
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
			GUIWnd* pDesktop = TObjRef<GUISystem>()->GetDesktop();
			GUIEffectMgr::Inst()->PlayScreenEffect(_T("txui015"),pDesktop->GetView(),pDesktop->GetSize(),GUIEffectMgr::EET_ComposSucess);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_pStcPerfectHint->SetInvisible(FALSE);
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			//m_pStcLoseHint->SetInvisible(FALSE);
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
			
			GUIWnd* pDesktop = TObjRef<GUISystem>()->GetDesktop();
			GUIEffectMgr::Inst()->PlayScreenEffect(_T("txui016"),pDesktop->GetView(),pDesktop->GetSize(),GUIEffectMgr::EET_ComposFail);
		}
		break;
	case E_Compose_NPC_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPC_Not_Exis")]);
		break;
	case E_Compose_Formula_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Formula_Not_Exist")]);
		break;
	case E_Compose_Type_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_UnIdentify")]);
		break;
	case E_Compose_Stuff_Not_Enough:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Stuff_Not_Enough")]);
		break;
	case E_Compose_FormulaNotMatch:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_FormulaNotMatch")]);
		break;
	case E_Compose_Skill_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Skill_Not_Exist")]);
		break;
	case E_Compose_NPCCanNotCompose:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPCCanNotCompose")]);
		break;
	case E_Compose_NotEnough_Money:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEnough_Money")]);
		break;
	case E_Compose_Vitality_Inadequate:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Vitality_Inadequate")]);
		break;
	case E_Compose_Bag_Full:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PickGroundItemErr1")]);
		break;
	case E_Compose_EquipUnidentify:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_UnIdentify")]);
		break;
	default:
		break;
	}

	m_pStcWhiteIdetifyPct->SetText(_T(""));
	m_pStcYellowIdetifyPct->SetText(_T(""));
	m_pStcGreenIdetifyPct->SetText(_T(""));
	m_pStcBlueIdetifyPct->SetText(_T(""));
	m_pStcOrangeIdetifyPct->SetText(_T(""));
	m_pStcVioletIdetifyPct->SetText(_T(""));
	HandleItem(true);
	m_pStcSuccHint->SetTopMost(TRUE);
	m_pStcSuccHint->FlipToTop();
	m_pWndMain->SetRefresh(TRUE);
	// Jason 2010-4-29
	UtilCalculateSuccessRatio();
	return 0;
}

DWORD ItemProduceFrame::NetRecvNS_Decomposition(tagNS_Decomposition* pMsg, DWORD pPrama)
{
	switch(pMsg->dwErrorCode)
	{
	case E_Compose_Consolidate_Success:
		{	
			m_pStcSuccHint->SetInvisible(FALSE);
			m_pStcSuccHint->SetTopMost(TRUE);
			m_pStcSuccHint->FlipToTop();
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_success"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Perfect:
		{
			m_pStcPerfectHint->SetInvisible(FALSE);
			m_pStcPerfectHint->SetTopMost(TRUE);
			m_pStcPerfectHint->FlipToTop();
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_perfect"), SOUND_NORMAL);
		}
		break;
	case E_Compose_Consolidate_Lose:
		{
			m_pStcLoseHint->SetInvisible(FALSE);
			m_pStcLoseHint->SetTopMost(TRUE);
			m_pStcLoseHint->FlipToTop();
			m_eCurState = EEItemProduce_RecvMsg;
			m_dwRecvMsgTime = Kernel::Inst()->GetAccumTimeDW();

			TObjRef<AudioSys>()->Play2DSound(_T("make_failure"), SOUND_NORMAL);
		}
		break;
	case E_Compose_NPC_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPC_Not_Exist")]);
		break;
	case E_Compose_Formula_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Formula_Not_Exist")]);
		break;
	case E_Compose_Type_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Type_Not_Exist")]);
		break;
	case E_Compose_Stuff_Not_Enough:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Stuff_Not_Enough")]);
		break;
	case E_Compose_FormulaNotMatch:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_FormulaNotMatch")]);
		break;
	case E_Compose_Skill_Not_Exist:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Skill_Not_Exist")]);
		break;
	case E_Compose_NPCCanNotCompose:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NPCCanNotCompose")]);
		break;
	case E_Compose_NotEnough_Money:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEnough_Money")]);
		break;
	case E_Compose_Vitality_Inadequate:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Vitality_Inadequate")]);
		break;
	case E_Compose_Quality_Not_Match:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Quality_Not_Match")]);
		break;
	case E_Compose_Not_Fashion:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Not_Fashion")]);
		break;
	case E_Compose_Equip_Lock:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Lock")]);
		break;
	case E_Compose_Equip_Time_Limit:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Time_Limit")]);
		break;
	case E_Compose_Equip_Not_identify:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Not_identify")]);
		break;
	case E_Compose_Equip_Level_Inadequate:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Level_Inadequate")]);
		break;
	case E_Compose_Equip_Quality_Inadequate:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Quality_Inadequate")]);
		break;
	case E_Compose_Equip_Type_Inadequate:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_Type_Inadequate")]);
		break;
	case E_Compose_Bag_Full:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("PickGroundItemErr1")]);
	case E_OnlyCanStrengthenByScript:
		ResetStartState();
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("E_OnlyCanStrengthenByScript")]);
		break;
	default:
		break;
	}	

	m_pStcWhiteIdetifyPct->SetText(_T(""));
	m_pStcYellowIdetifyPct->SetText(_T(""));
	m_pStcGreenIdetifyPct->SetText(_T(""));
	m_pStcBlueIdetifyPct->SetText(_T(""));
	m_pStcOrangeIdetifyPct->SetText(_T(""));
	m_pStcVioletIdetifyPct->SetText(_T(""));
	m_pWndMain->SetRefresh(TRUE);
	return 0;
}

DWORD ItemProduceFrame::OnUpdateLPSkillEvent( tagUpdateLPSkillEvent* pEvent )
{
	if(pEvent->eType == ESUT_Passive)
		return 0;

	if( m_eComposeType==EStartCompose_Skill
		&& m_eProType!= EPCT_NULL 
		&& m_eProType!=EPCT_PointUp)
	{
		DWORD dwOrcID = MTransSkillID(m_uComposeID.skillID);

		if( dwOrcID == pEvent->dwID )
		{
			const tagSkillData* pSkill = SkillMgr::Inst()->GetSkillData(dwOrcID);
			if( P_VALID(pSkill) )
			{
				INT nLevel = pSkill->nLevel;
				DWORD dwNewSkillID = MTransSkillTypeID(pEvent->dwID, nLevel);
				if( dwNewSkillID != m_uComposeID.skillID)
				{
					//设置配方
					m_uComposeID.skillID = dwNewSkillID;
					DeleteFormula();
					AddFormula2Tree(m_eProType, m_eComType);
				}
			}
		}
	}
	return 0;
}

DWORD ItemProduceFrame::OnEventProduceAddItem( tagItemPosChangeExEvent* pGameEvent )
{
	if( pGameEvent->eTypeSrc != EICT_Bag && pGameEvent->eTypeSrc != EICT_WarBag)
		return 0;

	if( m_eCurState != EEItemProduce_NULL )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Processing")]);
		return 0;
	}

	DWORD dwTypeID = pGameEvent->dwTypeIDSrc;
	if( m_eProType==EPCT_PointUp )
	{
		//判断是否为装备

		if(!MIsEquipment(dwTypeID))
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_NotEquip")]);
			return 0;
		}


		INT16 nPos = INT16(pGameEvent->n16IndexSrc);
		Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItem(nPos);
		if(!P_VALID(pEquip))
			return 0;


		if( pEquip->IsNotIdetified() )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_Equip_NoIdetified")]);
			return 0;
		}

		ClearStuffSelectFormula();
		DeleteFormula();
		m_dwFormulaID = GT_INVALID;

		tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
		m_pStcCtrl[0]->SetPic(strPic);

		tagMainStuff tagMainStuff;
		tagMainStuff.n64ID = pEquip->GetItemId();
		tagMainStuff.euqipPos = pEquip->GetEquipPos();
		m_Container.Add(tagMainStuff);

		//设置配方
		SetFormulaForPointUp();
	}
	else 
	{
		if( m_dwFormulaID==GT_INVALID )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_First")]);
			return 0;
		}

		/*
		if(MIsEquipment(dwTypeID))
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_IsEquip")]);
					return 0;
				}*/
		

		//在背包中的位置
		INT16 nPos = INT16( pGameEvent->n16IndexSrc);
		Item* pItem = ItemMgr::Inst()->GetConItem(pGameEvent->eTypeSrc, nPos);
		if(!P_VALID(pItem))
			return 0;

		const tagItemProto* pItemProto = pItem->GetProto();
		//特殊物品
		if( pItemProto->eSpecFunc==EISF_ComposeAdvance)
		{
			if(!JudgeSpecItem((ESpecFuncComposeAdvance)pItemProto->nSpecFuncVal1))
				return 0;

			tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
			m_pStcSuccCtrl->SetPic(strPic);

			tagIMStuff imStuff;
			imStuff.n64ID = pItem->GetItemId();

			m_Container.Add(imStuff);		

			m_pWndMain->SetRefresh(TRUE);
		}
		// 保底符
		else if(pItemProto->eSpecFunc==EISF_ProtectSign)
		{
			tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);
			m_pStcSuccCtrl->SetPic(strPic);

			tagIMStuff imStuff;
			imStuff.n64ID = pItem->GetItemId();

			m_Container.Add(imStuff);	

			m_pWndMain->SetRefresh(TRUE);
		}
		//普通材料
		else
		{
			int i=0;
			for(; i<m_Container.GetAssStuffSize(); i++)
			{
				int nNum = i+1;
				//获得数量
				INT16 nItemCount = pItem->GetItemQuantity();
				tstring strPic = ItemMgr::Inst()->GetItemIconPath(dwTypeID);


				//判断配方需求辅料的类型与拖动释放的物品类型是否相同
				tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
				if( !P_VALID(pAssStuff) )
				{
					CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Compose_First")]);
					return 0;
				}
				if(pAssStuff->stuffType!=EST_Null && pAssStuff->stuffType!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->stuffType == pItem->GetItemStuffType())
					{
						pAssStuff->nBagQuan = nItemCount;
						pAssStuff->dwTypeID = pItem->GetItemTypeID();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
						UtilCalculateSuccessRatio();
						m_pWndMain->SetRefresh(TRUE);					
						return 0;
					}
				}
				else  if(pAssStuff->dwTypeID!=0 && pAssStuff->dwTypeID!=GT_INVALID)
				{
					//材料类型相同
					if(pAssStuff->dwTypeID == pItem->GetItemTypeID())
					{
						if (AlreadyInAssStuf(pItem->GetItemId()))
							return 0;

						if (0 != pAssStuff->nBagQuan && HaveMorePlaceToPutItem(pAssStuff->dwTypeID) )
							continue;

						pAssStuff->nBagQuan = nItemCount;
						//pAssStuff->stuffType = pItem->GetItemStuffType();
						pAssStuff->n64ID = pItem->GetItemId();
						//设置控件数量
						TCHAR szText[64];
						_stprintf(szText, _T("%d/%d"), pAssStuff->nBagQuan, pAssStuff->nQuantity);
						m_pStcStuffQuan[i]->SetText(szText);

						if(pAssStuff->nBagQuan < pAssStuff->nQuantity)
							m_pStcStuffQuan[i]->SetTextColor(0xFFFF0000);
						else
							m_pStcStuffQuan[i]->SetTextColor(0xFF00FF00);

						m_pStcCtrl[nNum]->SetPic(strPic);
						UtilCalculateSuccessRatio();
						m_pWndMain->SetRefresh(TRUE);
						return 0;
					}
				}	
			}	
			CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("ConsolidateStuffError")]);
			m_pWndMain->SetRefresh(TRUE);
		}
	}
	return 0;
}

void ItemProduceFrame::PushButtonState(bool bEnable)
{
	for(int i=0; i<FORMULA_QUANTITY; i++)
	{
		m_pPBtnFormular[i]->SetEnable(bEnable);
	}
}
/*
 * Jason 2010-4-2 材料类型可以为空，这时材料必须是装备
 */
void ItemProduceFrame::UtilCalculateSuccessRatio()
{
	switch(m_eProType)
	{
	case EPCT_Artisan:
	case EPCT_DanTraining:
	case EPCT_Smith:
	case EPCT_Casting:
	case EPCT_Dressmaker:
	case EPCT_Aechnics:
	case EPCT_Smilt:
	case EPCT_GodArtisan:
	case EPCT_Pet:
	case EPCT_EquipMaster:
	case EPCT_TreasureCom:
	case EPCT_FoodGod1:
	case EPCT_FoodGod2:
	case EPCT_FoodGod3:
	case EPCT_Perfume:
	case EPCT_EquipRefine:
	case EPCT_StuffRefine:
	case EPCT_TimeLimitEquip:
	case EPCT_Gem:
	case EPCT_Crystal:
	case EPCT_Stole_52:
	case EPCT_Stole_67:
	case EPCT_Stole_82:
	case EPCT_Stole_Blade:
	case EPCT_Stole_Bow:
	case EPCT_Stole_Valor:
	case EPCT_Stole_Swing:
	case EPCT_SoulLinker:
	case EPCT_MoonCake:
	case EPCT_HallowmasGift:
	case EPCT_HallowmasName:
	case EPCT_MoShenEquip:
	case EPCT_FairySoulStren://add by xp
	case EPCT_FairySkillbookStren://add by xp
	case EPCT_BattleFieldCompose://add by xp
	case EPCT_ReplaceFabao://add by xp
		{
			const tagProduceProto* pProProto = NULL;
			pProProto = ProduceData::Inst()->FindProduceProto(m_eProType, m_dwFormulaID);
			if( !P_VALID(pProProto) )
				return;

			//非装备生产不计算修正率
			if( !MIsEquipment(pProProto->dwProItemTypeID) )
				return;
			
			int nStuffNum = 0;
			vector<tagAssistStuff> stuff;
			m_Container.GetAssistStuffMap(stuff);
			int size = (int)stuff.size();
			int nStuffCount = 0;
			for( int i=0; i<size; ++i )
			{
				if( (stuff[i].nBagQuan!=0 && stuff[i].nBagQuan!=GT_INVALID)
					&& stuff[i].nBagQuan >= stuff[i].nQuantity  )
				{
					++nStuffNum;
					if( EST_Null != stuff[i].stuffType || MIsEquipment(stuff[i].dwTypeID) )
						nStuffCount += stuff[i].nQuantity;
				}
			}

			//所有材料都放了
			if( nStuffNum == size )
			{
				INT nWhiteItem = 0;
				INT nYellowItem = 0;
				INT nGreenitem = 0;
				INT nBlueItem = 0;
				INT nOrangeItem = 0;
				INT nPurpleItem = 0;
				for( int i=0; i<size; ++i )
				{
					//if( EST_Null != stuff[i].stuffType )
					{
						BYTE byQuality = 0;
						if( EST_Null == stuff[i].stuffType && MIsEquipment(stuff[i].dwTypeID) )
						{
							Equipment* pEquip = (Equipment*)ItemMgr::Inst()->GetPocketItemByID(stuff[i].n64ID);
							if( P_VALID(pEquip) )
							{
								byQuality = pEquip->GetItemQuality();
							}
						}
						else if( EST_Null != stuff[i].stuffType && /*!MIsEquipment*/P_VALID(stuff[i].dwTypeID) )
							byQuality = ItemProtoData::Inst()->FindItemProto(stuff[i].dwTypeID)->byQuality;
						else
							continue;

						switch( byQuality )
						{
						case 0:
							nWhiteItem += stuff[i].nQuantity;
							break;
						case 1:
							nYellowItem += stuff[i].nQuantity;
							break;
						case 2:
							nGreenitem += stuff[i].nQuantity;;
							break;
						case 3:
							nBlueItem += stuff[i].nQuantity;
							break;
						case 4:
							nOrangeItem += stuff[i].nQuantity;
							break;
						case 5:
							nPurpleItem += stuff[i].nQuantity;
							break;
						default:
							nWhiteItem += stuff[i].nQuantity;
							break;
						}
					}
				}

				/* 
				1级修正率=-50%×白品材料数量/材料总数量+10%×黄品材料数量/材料总数量+25%×绿品材料数量
				/材料总数量+50%×蓝品材料数量/材料总数量+100%×橙品材料数量/材料总数量					
				*/
                FLOAT fModiOddsRate = 0;
				if( nStuffCount )
					fModiOddsRate = -50.0f*nWhiteItem/nStuffCount + 10.0f*nYellowItem/nStuffCount + 15.0f*nGreenitem/nStuffCount + 
						50*nBlueItem/nStuffCount + 100.0f*nOrangeItem/nStuffCount + 100.0f*nPurpleItem/nStuffCount;
				if (fModiOddsRate > 100.0f)
				{
					fModiOddsRate = 100.0f;
				}
				INT nModiOddsRate = (INT)fModiOddsRate;


				/*
				2级修正率=5%×绿品材料数量/材料总数量+20%×蓝品材料数量/材料总数量+30%×橙品材料数量/材料总数量
				*/
                FLOAT fModiOddsRate2 = 0;
				if( nStuffCount )
					fModiOddsRate2 = 5.0f*nGreenitem/nStuffCount + 20.0f*nBlueItem/nStuffCount + 30.0f*nOrangeItem/nStuffCount;
				if ( fModiOddsRate2 > 100.0f)
				{
					fModiOddsRate2 =100.0f;
				}
                INT   nModiOddsRate2 = static_cast<INT>(fModiOddsRate2);
				/*
				生成紫色品级装备的几率，公式为：
				装备紫色品级几率=5%×蓝品材料数量/材料总数量+22%×橙品材料数量/材料总数量+100%×紫品材料数量/材料总数量
				*/
                FLOAT  fVioletRate = 0;
				if( nStuffCount )
					fVioletRate = 5.0f*nBlueItem/nStuffCount + 22.0f*nOrangeItem/nStuffCount + 100.0f*nPurpleItem/nStuffCount;
				if( fVioletRate > 100.0f)
				{
					fVioletRate = 100.0f ;
				}
				INT    nVioletRate = static_cast<INT>(fVioletRate);

                
				//以下根据以上的一级修正率，二级，紫色几率 算装备的生成几率
				const tagEquipQltyPct* pAtt = ItemProtoData::Inst()->FindEquipQltyPctByItemID(pProProto->dwProItemTypeID);
                assert( P_VALID(pAtt) );
				if ( !P_VALID(pAtt) )
				{
					return ;
				}
				
				//各种品级的装备原始鉴定几率
				INT nWhiteTemp = pAtt->nEquipQltyPct[0],nYellowTemp =pAtt->nEquipQltyPct[1] ,nGreenTemp =pAtt->nEquipQltyPct[2],
					nBlueTemp  = pAtt->nEquipQltyPct[3],nOrangeTemp =pAtt->nEquipQltyPct[4],nVioletTemp =pAtt->nEquipQltyPct[5];
				FLOAT fWhite =0.0f,fYellow =0.0f ,fGreen =0.0f,fBlue =0.0f,fOrange =0.0f,fViolet =0.0f;
                FLOAT fModiRate1 = fModiOddsRate, fModiRate2 = fModiOddsRate2;
				if ( fModiRate1 > 100.0f)
				{
					fModiRate1 = 100.0f;
				}
                
				if ( fModiRate2 > 100.0f)
				{
					fModiRate2 = 100.0f;
				}

				fWhite  = ((FLOAT)nWhiteTemp) / 10000.0f;
				fYellow = ((FLOAT)nYellowTemp)/ 10000.0f;
				fGreen  = ((FLOAT)nGreenTemp) / 10000.0f; 
				fBlue   = ((FLOAT)nBlueTemp) /  10000.0f;
				fOrange = ((FLOAT)nOrangeTemp)/ 10000.0f;
				fViolet = ((FLOAT)fVioletRate)/ 100.0f ;
				fModiRate1 = ((FLOAT)fModiRate1)/ 100.0f;
				fModiRate2 = ((FLOAT)fModiRate2)/ 100.0f;

				if ( nModiOddsRate >= 0)
				{
					  fOrange = (1.0f - fViolet)*(fOrange+(fBlue)*(fModiRate1)+fGreen*(1.0f-fModiRate1)*(fModiRate2));
 				      fBlue   = (1.0f - fViolet)*(fBlue*(1.0f-fModiRate1)*(1.0f-fModiRate2)+fGreen*(fModiRate1)+fYellow*(1.0f-fModiRate1)*(fModiRate2));
                      fGreen  = (1.0f - fViolet)*(fGreen*(1.0f-fModiRate1)*(1.0f-fModiRate2)+fYellow*(fModiRate1)+fWhite*(1.0f-fModiRate1)*(fModiRate2));
                      fYellow = (1.0f - fViolet)*(fYellow*(1.0f-fModiRate1)*(1.0f-fModiRate2)+fWhite*(fModiRate1));
					  fWhite  = (1.0f - fViolet)*(fWhite*(1.0f-fModiRate1)*(1.0f-fModiRate2));

 			          
				}
				else
				{
					fWhite  = fWhite - fYellow*(fModiRate1);
					fYellow = fYellow*(1.0f+fModiRate1)- fGreen*(fModiRate1);
					fGreen  = fGreen*(1.0f+fModiRate1) - fBlue*(fModiRate1);
					fBlue   = fBlue*(1.0f+fModiRate1)  - fOrange*(fModiRate1);
					fOrange = fOrange*(1.0f+fModiRate1);
					fViolet = 0.0f ;
					
					
					
					
			        
					  
					   
			    }

                INT nW = (INT)(fWhite*100.0f);
				INT nY = (INT)(fYellow*100.0f);
				INT nG = (INT)(fGreen*100.0f);
				INT nB = (INT)(fBlue*100.0f);
				INT nO = (INT)(fOrange*100.0f);
				INT nV = (INT)((fViolet)*100.0f);

				if (nW < 0)
				{
					nW = 0;
				}

				if (nY < 0)
				{
					nY = 0 ;
				}

				if (nG < 0)
				{
					nG = 0;
				}

				if ( nB < 0 )
				{
					nB = 0;
				}

				if ( nO < 0)
				{
					nO = 0;
				}

				if ( nV < 0)
				{
					nV = 0 ;
				}

				
				TCHAR szText[64];
				_stprintf( szText, _T("%d%%"),  nModiOddsRate);
				m_pStcModiOdds->SetText(szText);

				_stprintf( szText, _T("%d%%"),  nW);
				m_pStcWhiteIdetifyPct->SetText(szText);

				_stprintf( szText, _T("%d%%"),  nY);
				m_pStcYellowIdetifyPct->SetText(szText);

                _stprintf( szText, _T("%d%%"), nG);
				m_pStcGreenIdetifyPct->SetText(szText);

				_stprintf( szText, _T("%d%%"),  nB);
				m_pStcBlueIdetifyPct->SetText(szText);

				_stprintf( szText, _T("%d%%"),  nO);
				m_pStcOrangeIdetifyPct->SetText(szText);

				_stprintf( szText, _T("%d%%"),  nV);
				m_pStcVioletIdetifyPct->SetText(szText);
				m_pWndMain->SetRefresh(TRUE);
			}
		}
		break;
	case EPCT_PointUp:
		break;
	}
}

void ItemProduceFrame::HandleItem( bool bOperable )
{
	vector<tagAssistStuff> assStuff;
	m_Container.GetAssistStuffMap(assStuff);
	for( size_t i = 0; i < assStuff.size(); i++ )
	{
		Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(assStuff[i].n64ID);
		if( P_VALID(pItem) )
			pItem->SetOperable(bOperable);
	}

	tagIMStuff im;
	m_Container.GetIMStuff(im);
	Item* pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(im.n64ID);
	if( P_VALID(pItem) )
		pItem->SetOperable(bOperable);

	tagMainStuff mainStuff;
	m_Container.GetMainStuff(mainStuff);
	pItem = (Item*)ItemMgr::Inst()->GetPocketItemByID(mainStuff.n64ID);
	if( P_VALID(pItem) )
		pItem->SetOperable(bOperable);
}

void ItemProduceFrame::ReSetUI( EProduceType eType )
{
	if ( EPCT_GodArtisan == eType )
	{
		AddFormula2Tree(EPCT_GodArtisan, ECOMT_Rebuild);
		this->ClearStuff();
		for(int i=0; i<FORMULA_QUANTITY; i++)
		{
			if ( 0 == i)
			{
				m_pPBtnFormular[i]->SetText(g_StrTable[_T("Compose_ShengyinHecheng")]);
				continue;
			}
			m_pPBtnFormular[i]->SetInvisible(TRUE);
		}
		m_bUnChlik = true;
		m_pWndName->SetText(g_StrTable[_T("Compose_ShengyinGongyi")]);
	} 
	else if ( EPCT_Gem == eType )
	{
		AddFormula2Tree(EPCT_GodArtisan, ECOMT_Rebuild);
		this->ClearStuff();
		for(int i=0; i<FORMULA_QUANTITY; i++)
		{
			if ( 0 == i)
			{
				AddFormula2Tree(EPCT_Gem, ECOMT_GemCompose);
				this->ClearStuff();
				m_pPBtnFormular[i]->SetText(g_StrTable[_T("Compose_BaoshiHecheng")]);
				continue;
			}
			m_pPBtnFormular[i]->SetInvisible(TRUE);
		}
		m_bUnChlik = true;
		m_pWndName->SetText(g_StrTable[_T("Compose_BaoshiGongyi")]);
	}
}

VOID ItemProduceFrame::CloseAllUiEffect()
{
	GUIEffectMgr::Inst()->RemoveOneEffectByType(GUIEffectMgr::EET_ComposSucess);
	GUIEffectMgr::Inst()->RemoveOneEffectByType(GUIEffectMgr::EET_ComposFail);
}

bool ItemProduceFrame::HaveMorePlaceToPutItem(DWORD dwTypeID)
{
	for(int i =0; i<m_Container.GetAssStuffSize(); i++)
	{
		tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
		if( P_VALID(pAssStuff) )
		{
			if(pAssStuff->dwTypeID == dwTypeID  && 0 == pAssStuff->nBagQuan)
				return true;
		}
	}
	return false;
}

bool ItemProduceFrame::AlreadyInAssStuf(INT64 n64id)
{
	for(int i =0; i<m_Container.GetAssStuffSize(); i++)
	{
		tagAssistStuff* pAssStuff = m_Container.GetAssistStuff(i);
		if( P_VALID(pAssStuff) )
		{
			if(pAssStuff->n64ID == n64id  && 0 != pAssStuff->nBagQuan)
				return true;
		}
	}
	return false;
}