//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: guild.cpp
// author: Sxg
// actor:
// data: 2009-4-7
// last:
// brief: 帮派
//-------------------------------------------------------------------------------
#include "StdAfx.h"

#include "guild.h"
#include "role.h"
#include "role_mgr.h"
#include "guild_mgr.h"
#include "guild_warehouse.h"
#include "guild_upgrade.h"
#include "city_mgr.h"
#include "script_mgr.h"
#include "att_res.h"
#include "GuildBattle.h"
#include "../WorldDefine/buff_define.h"

#include "../ServerDefine/msg_guild.h"
#include "../ServerDefine/msg_log.h"
#include "../ServerDefine/role_data.h"

#include "../WorldDefine/msg_guild.h"
#include "title_mgr.h"

#include "guild_war_declare_mgr.h"

const tagGuildPower		Guild::m_NullGuildPower;
const tagGuildKick		Guild::m_NullGuildKick;
const tagGuildAppoint	Guild::m_NullGuildAppoint;
//-------------------------------------------------------------------------------
// 获取操作权限表
//-------------------------------------------------------------------------------
const tagGuildPower& Guild::GetGuildPower(EGuildMemberPos eGuildPos) const
{
	if(!::IsGuildPosValid(eGuildPos))
	{
		ASSERT(0);
		IMSG(_T("\n\n\nCaution:\tThere is a critical error<invalid guildpos> in guild power table!\n\n"));
		return Guild::m_NullGuildPower;
	}

	return g_attRes.GetGuildPower(eGuildPos);
}

const tagGuildKick& Guild::GetGuildKick(EGuildMemberPos eGuildPos) const
{
	if(!::IsGuildPosValid(eGuildPos))
	{
		ASSERT(0);
		IMSG(_T("\n\n\nCaution:\tThere is a critical error<invalid guildpos> in guild kick table!\n\n"));
		return Guild::m_NullGuildKick;
	}

	return g_attRes.GetGuildKick(eGuildPos);
}

const tagGuildAppoint& Guild::GetGuildAppoint(EGuildMemberPos eGuildPos) const
{
	if(!::IsGuildPosValid(eGuildPos))
	{
		ASSERT(0);
		IMSG(_T("\n\n\nCaution:\tThere is a critical error<invalid guildpos> in guild appoint table!\n\n"));
		return Guild::m_NullGuildAppoint;
	}

	return g_attRes.GetGuildAppoint(eGuildPos);
}

//-------------------------------------------------------------------------------
// 构造&析构
//-------------------------------------------------------------------------------
Guild::Guild()
{
	m_bCheckGuildFund = FALSE;
	m_timeLastUpdate = 0;
}

Guild::Guild(const tagGuildLoad *pGuildLoad)
{
	m_timeLastUpdate = 0;
	m_bCheckGuildFund = FALSE;
	memset(&m_Att,0,sizeof(m_Att));
	m_GuildPos.Init(pGuildLoad->byLevel);
	m_MemberMgr.Init(pGuildLoad->dwID);

	m_Att.dwID				= pGuildLoad->dwID;
	m_Att.dwFounderNameID	= pGuildLoad->dwFounderNameID;
	m_Att.dwLeaderRoleID	= pGuildLoad->dwLeaderRoleID;

	// 帮派状态
	m_Att.dwSpecState		= pGuildLoad->dwSpecState;
	m_GuildState.SetState((EGuildSpecState)m_Att.dwSpecState);

	m_Att.byLevel			= pGuildLoad->byLevel;
	
	m_Att.nFund				= pGuildLoad->nFund;
	m_Att.nMaterial			= pGuildLoad->nMaterial;
	m_Att.nReputation		= pGuildLoad->nReputation;
	m_Att.n16Peace			= pGuildLoad->n16Peace;
	m_Att.n16MemberNum		= pGuildLoad->n16MemberNum;
	m_Att.n16Rank			= pGuildLoad->n16Rank;
	m_Att.byMinJoinLevel	= pGuildLoad->byMinJoinLevel;
	m_Att.nDailyCost		= pGuildLoad->nDailyCost;
	m_Att.nGroupPurchase	= pGuildLoad->nGroupPurchase;
	m_Att.byAffairRemainTimes = pGuildLoad->byAffairRemainTimes;
	m_Att.bCommendation		= pGuildLoad->bCommendation;
	m_Att.dwCreateTime		= pGuildLoad->dwCreateTime;

	// 帮派名,宗旨,徽标
	m_Att.strName			= pGuildLoad->szName;
	m_Att.strTenet			= pGuildLoad->szTenet;
	m_Att.strSymbol			= pGuildLoad->szSymbolURL;

	m_Att.dwGuildValue1		= pGuildLoad->dwGuildValue1;
	m_Att.dwGuildValue2		= pGuildLoad->dwGuildValue2;
	
	// 更新帮派消耗
	m_Att.nDailyCost = (INT32)MGuildDailyCost(m_Att.byLevel, m_Att.n16Peace);

	if ( m_Att.byLevel != 0 )
	{

		// 所属城市
		for(INT32 i=0; i<MAX_GUILD_HOLDCITY; ++i)
		{
			m_Att.byHoldCity[i]	= pGuildLoad->byHoldCity[i];
		}
	

	// 帮派设施初始化
	m_GuildUpgrade.Init(this, TRUE);

	// 帮务初始化(无需向数据库请求)
	m_GuildAffair.Init(this);

	// 帮派技能初始化
	m_GuildSkill.Init(this, TRUE);

	// 帮派跑商初始化
	m_GuildCommerce.Init(this, TRUE);

	// 帮派仓库初始化
	m_GuildWarehouse.Init(this, TRUE);
	}
}

Guild::~Guild()
{
}

//-------------------------------------------------------------------------------
// 根据数据库数据初始化
//-------------------------------------------------------------------------------
VOID Guild::InitDBGuildMember(const tagGuildMember& sGuildMember)
{
	if(LoadMember(sGuildMember) != E_Success)
	{
		// 执行到该处，多半是数据库数据库有错误
		ASSERT(0);
		return;
	}

	// 根据数据库中成员数据，重新设置
	tagGuildMember *pMember = GetMember(sGuildMember.dwRoleID);
	ASSERT(P_VALID(pMember) && sGuildMember.eGuildPos == pMember->eGuildPos);
	
	*pMember = sGuildMember;
}

//-------------------------------------------------------------------------------
// update
//-------------------------------------------------------------------------------
VOID Guild::Update()
{
	if( m_Att.byLevel != 0 )
	{
		// 处理邀请函
		if(!m_mapInvite.Empty())
		{
			DWORD dwInviteeID = GT_INVALID;
			tagInvite *pInviter = NULL;
			MapInvite::TMapIterator iter = m_mapInvite.Begin();
			while(m_mapInvite.PeekNext(iter, dwInviteeID, pInviter))
			{
				if(--pInviter->nInviteCountDown <= 0)
				{
					m_mapInvite.Erase(dwInviteeID);
					delete pInviter;
				}
			}
		}
	
		if (m_bCheckGuildFund == FALSE && m_GuildUpgrade.IsInitOK())
		{
			INT32 nMaxFund = m_GuildUpgrade.GetMaxFund();
			if (m_Att.nFund > nMaxFund)
			{
				m_Att.nFund = nMaxFund;
			}
			if (m_Att.nFund < 0)
			{
				m_Att.nFund = 0;
			}
			m_bCheckGuildFund = TRUE;
		}

	// 处理帮派仓库
	m_GuildWarehouse.Update();

		// 公会每日奖励凌晨职务领取次数清除
		tagDWORDTime now = g_world.GetWorldTime();
		if( m_timeLastUpdate > 0 )
		{
			if( m_timeLastUpdate.day != now.day )
			{
				memset(m_Att.wGuildPosTakeRewardTimes,0,sizeof(m_Att.wGuildPosTakeRewardTimes));
				SynGuildAtt2DB();
			}
		}
		m_timeLastUpdate = now;
	}
}

//-------------------------------------------------------------------------------
// check
//-------------------------------------------------------------------------------
BOOL Guild::IsGuildInitOK()
{
	BOOL bRet = TRUE;

	if(GetGuildMemberNum() <= 0)
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> members load failed! Please check db! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	if (!m_GuildAffair.IsInitOK())
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> affair init failed! Please check! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	if (!m_GuildUpgrade.IsInitOK())
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> facilities init failed! Please check! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	if (!m_GuildSkill.IsInitOK())
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> skill init failed! Please check! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	if (!m_GuildCommerce.IsInitOK())
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> commodity init failed! Please check! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	if (!m_GuildWarehouse.IsInitOK())
	{
		ASSERT(0);
		ILOG->Write(_T("Guild<id: %u> warehouse init failed! Please check! \r\n"), m_Att.dwID);
		bRet = FALSE;
	}

	return bRet;
}

//-------------------------------------------------------------------------------
// 初始化
//-------------------------------------------------------------------------------
VOID Guild::InitCreate(Role* pCreator, const tstring& strGuildName, 
						DWORD dwGuildID, const tagGuildCfg &sGuildCfg)
{
	m_Att.Init(strGuildName, dwGuildID, pCreator->GetID(), pCreator->GetNameID(), 
				sGuildCfg.byGuildLevelBegin, sGuildCfg.n16GuildPeaceBegin, 
				sGuildCfg.nGuildFundBegin, sGuildCfg.nGuildMaterialBegin, 
				sGuildCfg.nGuildRepBegin, sGuildCfg.nGuildGroupPurchaseBegin);

	m_GuildPos.Init(m_Att.byLevel);
	m_MemberMgr.Init(dwGuildID);

	// 设置创建者属性
	AddMember(pCreator->GetID(), EGMP_HuiZhang);
	pCreator->SetGuildID(dwGuildID);

	// 帮派设施
	m_GuildUpgrade.Init(this);
	m_GuildUpgrade.CreateFacilities();

	// 帮务初始化
	m_GuildAffair.Init(this);

	// 帮派技能初始化
	m_GuildSkill.Init(this);
	m_GuildSkill.CreateGuildSkill();

	// 帮派跑商初始化
	m_GuildCommerce.Init(this);

	// 帮派仓库(最好保证仓库在设施之后,否则可能需要reinit)
	m_GuildWarehouse.Init(this);
}

//-------------------------------------------------------------------------------
// 解散帮派
//-------------------------------------------------------------------------------
DWORD Guild::DismissGuild(Role* pRole)
{
	ASSERT(P_VALID(pRole));

	// 获取帮派成员信息
	tagGuildMember *pMember = GetMember(pRole->GetID());
	if(!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}

	// 是否有权限
	if(!GetGuildPower(pMember->eGuildPos).bDismissGuild)
	{
		return E_Guild_Power_NotEnough;
	}
	
	//判断帮派是否有统治城市
	for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
	{
		if (m_Att.byHoldCity[n] != 0)
		{
			return E_Guild_HoldCity;
		}
	}
	
	//判断本帮派是否处在城战报名状态
	if(g_cityMgr.IsAlreadySignup(m_Att.dwID,ECFT_Attack)
		||g_cityMgr.IsAlreadySignup(m_Att.dwID,ECFT_Defence)
		||g_cityMgr.IsAlreadySignup(m_Att.dwID,ECFT_Request))
	{
			return E_Guild_AlreadySigup;
	}
	
	// 是否还有其他成员
	if(m_MemberMgr.GetNumber() > 1)
	{
		return E_Guild_MemberHasMore;
	}

	// 当有公会要解散时触发,检查是否有宣战关系
	g_guildWarDeclareMgr.OnGuildDismiss(pRole->GetGuildID());

	// 清空成员 -- 该处需要调用，以修改数据库表
	RemoveMember(pRole->GetID(), pMember->eGuildPos);

	// 堂内属性在析构时自动清除

	// 设置角色属性
	pRole->SetGuildID(GT_INVALID);

	// 清空帮派仓库
	m_GuildWarehouse.RemoveAllItems();

	// 清除帮派设施信息
	m_GuildUpgrade.RemoveAllFacilitiesInfo();

	// 清除帮派技能信息
	m_GuildSkill.RemoveGuildSkillInfo();

	// 清除跑商信息(帮派解散时只有此人)
	m_GuildCommerce.AbandonCommerce(pRole->GetID(), TRUE);

	// Jason 2010-3-11 v1.4.0
	tagNS_GuildExploit send;
	send.dwRoleID			= pRole->GetID();
	send.nExploit		= 0;
	pRole->SendMessage(&send, send.dwSize);
	// Jason 2010-3-4 v1.4.0帮派功勋称号
	pRole->GetTitleMgr()->SigEvent( ETE_GUILD_EXPLOIT, GT_INVALID ,0 );

	return E_Success;
}

//-------------------------------------------------------------------------------
// 能否添加成员 -- bInsert：== TRUE，插入待加入列表；==FALSE，从列表中删除
//-------------------------------------------------------------------------------
DWORD Guild::CanInviteJoin(DWORD dwInviterID, DWORD dwInviteeID, 
						   Role **ppInvitee, BOOL bInsert/* = TRUE*/)
{
	// 找到邀请人
	tagGuildMember *pInviter = GetMember(dwInviterID);
	if(!P_VALID(pInviter))
	{
		return GT_INVALID;
	}

	// 判断权限
	if(!GetGuildPower(pInviter->eGuildPos).bInviteJoin)
	{
		return E_Guild_Power_NotEnough;
	}

	// 是否达到人数上限
	if(m_GuildPos.IsMemberFull())
	{
		return E_Guild_Member_Full;
	}

	// 找到被邀请者
	Role *pInvitee = g_roleMgr.GetRolePtrByID(dwInviteeID);
	if(!P_VALID(pInvitee))
	{
		return E_Role_Not_Online;
	}
	// 检查被邀请人设置
	if(pInvitee->GetSession()->GetPersonalSet().bAcceptFactionInvite == false)
	{
		return E_Guild_Join_SystemRefuse;
	}

	// 是否已加入帮派
	if(pInvitee->IsInGuild())
	{
		// 其他地方有逻辑问题
		ASSERT(P_VALID(g_guildMgr.GetGuild(pInvitee->GetGuildID())));

		return E_Guild_Join_AlreadyIn;
	}

	// 等级判断
	if(pInvitee->GetLevel() < g_guildMgr.GetGuildCfg().nJoinRoleMinLevel)
	{
		return E_Guild_Join_LevelLimit;
	}

	if(bInsert)	// 加入到待加入列表
	{
		// 是否已在待加入列表中
		if(m_mapInvite.IsExist(dwInviteeID))
		{
			return E_Guild_Join_BeInvited;
		}
		
		tagInvite *pInvite = new tagInvite(dwInviterID);
		ASSERT(P_VALID(pInvite));
		m_mapInvite.Add(dwInviteeID, pInvite);
	}
	else
	{
		// 是否已在待加入列表中
		if(!m_mapInvite.IsExist(dwInviteeID))
		{
			return E_Guild_Join_NotBeInvited;
		}

		tagInvite *pInviter = m_mapInvite.Peek(dwInviteeID);
		ASSERT(P_VALID(pInviter));
		delete pInviter;
		
		m_mapInvite.Erase(dwInviteeID);
	}

	if(ppInvitee != NULL)
	{
		*ppInvitee = pInvitee;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------
// 删除指定邀请帖
//-------------------------------------------------------------------------------
BOOL Guild::EraseInviteJoin(DWORD dwInviteeID)
{
	tagInvite* pInvite = m_mapInvite.Peek(dwInviteeID);
	if (P_VALID(pInvite))
	{
		SAFE_DEL(pInvite);
	}
	
	return m_mapInvite.Erase(dwInviteeID);
}

//-------------------------------------------------------------------------------
// 添加成员
//-------------------------------------------------------------------------------
DWORD Guild::InviteJoin(DWORD dwInviterID, DWORD dwInviteeID)
{
	Role *pInvitee = NULL;

	// 再次判断
	DWORD dwErrorCode = CanInviteJoin(dwInviterID, dwInviteeID, &pInvitee, FALSE);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 添加到成员列表中，并设置为帮众
	AddMember(dwInviteeID, EGMP_HuiYuan);

	// 设置角色帮派字段
	pInvitee->SetGuildID(m_Att.dwID);

	// Jason 2010-3-10 通知给被邀请者，帮派宗旨
	SendGuildTenet2Client(pInvitee->GetSession());

	return E_Success;
}

//-------------------------------------------------------------------------------
// 任职
//-------------------------------------------------------------------------------
DWORD Guild::Appoint(DWORD dwAppointorID, DWORD dwAppointeeID, EGuildMemberPos eGuildPos)
{
	// 获得成员属性
	tagGuildMember *pAppointor = GetMember(dwAppointorID);
	if(!P_VALID(pAppointor))
	{
		// 执行到此，说明逻辑上有错误
		ASSERT(0);
		return GT_INVALID;
	}

	tagGuildMember *pAppointee = GetMember(dwAppointeeID);
	if(!P_VALID(pAppointee))
	{
		return E_Guild_MemberNotIn;
	}

	// 任职前后职位是否相同
	if(pAppointee->eGuildPos == eGuildPos)
	{
		return E_Guild_Appoint_SamePos;
	}
	
	// 权限
	if(!GetGuildAppoint(pAppointor->eGuildPos).BitSetGuildAppoint[eGuildPos]
		|| !GetGuildAppoint(pAppointor->eGuildPos).BitSetGuildAppoint[pAppointee->eGuildPos])
	{
		return E_Guild_Power_NotEnough;
	}

	// 执行特殊要求
	DWORD dwErrorCode = m_GuildPos.SpecGuildPosLimit(dwAppointeeID, eGuildPos);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}
	
	// 检查职位上人员是否已满
	if(m_GuildPos.IsPosFull(eGuildPos))
	{
		return E_Guild_Pos_Full;
	}

	// 任命
	return ChangeGuildPos(pAppointee, eGuildPos);
}

//-------------------------------------------------------------------------------
// 剔出成员
//-------------------------------------------------------------------------------
DWORD Guild::KickMember(DWORD dwRoleID, DWORD dwKickID)
{
	// 获取帮派成员属性
	tagGuildMember *pMember = GetMember(dwRoleID);
	if(!P_VALID(pMember))
	{
		return GT_INVALID;
	}

	tagGuildMember *pBeKicked = GetMember(dwKickID);
	if(!P_VALID(pBeKicked))
	{
		return GT_INVALID;
	}

	// 权限
	if(!GetGuildKick(pMember->eGuildPos).BitSetGuildKick[pBeKicked->eGuildPos])
	{
		return E_Guild_Power_NotEnough;
	}

	// 设置
	RemoveMember(dwKickID, pBeKicked->eGuildPos);

	// 如果角色在线，处理角色信息中帮派信息
	Role *pRoleKicked = g_roleMgr.GetRolePtrByID(dwKickID);
	if(P_VALID(pRoleKicked))
	{
		pRoleKicked->SetGuildID(GT_INVALID);
	}

	// 如果不在线，则需玩家上线初始化帮派信息时重置

	return E_Success;
}

//-------------------------------------------------------------------------------
// 移交帮主
//-------------------------------------------------------------------------------
DWORD Guild::TurnoverLeader(DWORD dwOldLeaderID, DWORD dwNewLeaderID)
{
	// 获取帮派属性
	tagGuildMember *pOldLeader = GetMember(dwOldLeaderID);
	if(!P_VALID(pOldLeader))
	{
		return GT_INVALID;
	}

	tagGuildMember *pNewLeader = GetMember(dwNewLeaderID);
	if(!P_VALID(pNewLeader))
	{
		return GT_INVALID;
	}

	// 权限
	if(!GetGuildPower(pOldLeader->eGuildPos).bTurnoverLeader)
	{
		return E_Guild_Power_NotEnough;
	}
	
	/**********************************************************/
	/* 移交
	/**********************************************************/
	//// .1原帮主辞去职务
	//m_MemberMgr.SetGuildPos(pOldLeader, EGMP_BangZhong, TRUE);

	//// .2新帮主辞去原职务
	//if(::IsInTang(pNewLeader->eGuildPos))
	//{
	//	EGuildTangType eTang;
	//	ETangMemberPos eTangPos;
	//	::GetTangType(pNewLeader->eGuildPos, eTang, eTangPos);
	//	m_Tang[eTang].RemoveMember(dwNewLeaderID, eTangPos);
	//}

	//// .3新帮主任职
	//m_MemberMgr.SetGuildPos(pNewLeader, EGMP_BangZhu);

	// .1
	ChangeGuildPos(pOldLeader, EGMP_HuiYuan);
	ChangeGuildPos(pNewLeader, EGMP_HuiZhang);

	// .2设置现任帮主
	SetGuildBangzhu(dwNewLeaderID);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 辞职
//-------------------------------------------------------------------------------
DWORD Guild::DemissPostion(DWORD dwRoleID, OUT INT8 &n8OldGuildPos)
{
	// 获取帮派成员属性
	tagGuildMember *pMember = GetMember(dwRoleID);
	if(!P_VALID(pMember))
	{
		return GT_INVALID;
	}

	// 判断权限
	if(!GetGuildPower(pMember->eGuildPos).bDemissPostion)
	{
		return E_Guild_CanNot_Demiss;
	}

	// 获取原职位代码
	n8OldGuildPos = pMember->eGuildPos;
	
	// 辞去职务
	ChangeGuildPos(pMember, EGMP_HuiYuan);
	
	return E_Success;
}

//-------------------------------------------------------------------------------
// 脱离帮派
//-------------------------------------------------------------------------------
DWORD Guild::LeaveGuild(Role* pRole)
{
	ASSERT(P_VALID(pRole));

	// 获取帮派成员属性
	tagGuildMember *pMember = GetMember(pRole->GetID());
	if(!P_VALID(pMember))
	{
		return GT_INVALID;
	}

	// 判断权限
	if(!GetGuildPower(pMember->eGuildPos).bLeaveGuild)
	{
		return E_Guild_CanNot_Leave;
	}

	// 设置角色帮派属性
	pRole->SetGuildID(GT_INVALID);
	
	// 从管理器中删除
	RemoveMember(pRole->GetID(), pMember->eGuildPos);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 修改帮派宗旨
//-------------------------------------------------------------------------------
DWORD Guild::ModifyTenet(DWORD dwRoleID, LPCTSTR strNewTenet, INT32 nNumTCHAR)
{
	// 获取帮派成员属性
	tagGuildMember *pMember = GetMember(dwRoleID);
	if(!P_VALID(pMember))
	{
		return GT_INVALID;
	}

	// 权限
	if(!GetGuildPower(pMember->eGuildPos).bModifyTenet)
	{
		return E_Guild_Power_NotEnough;
	}

	/**********************************************************/
	/* 字符串检查
	/**********************************************************/
	tstring strTmp(strNewTenet, nNumTCHAR);

	// .1与原设置是否相同
	if(m_Att.strTenet == strTmp)
	{
		// 设置前后相同，不用处理，且不用反馈给客户端
		return E_Return_NotNeedFeed;
	}

	// .2长度检查
	if(g_attRes.GetVariableLen().nGuildTenet < nNumTCHAR)
	{
		return E_Filter_Text_TooLong;
	}

	// .3设置
	m_Att.strTenet = strTmp;

	// 向数据库服务器发送消息
	INT32 nMsgSz = sizeof(tagNDBC_ChangeGuildTenet) + nNumTCHAR * sizeof(TCHAR);
	
	MCREATE_MSG(pSend, nMsgSz, NDBC_ChangeGuildTenet);
	pSend->dwGuildID = m_Att.dwID;
	memcpy(pSend->szNewTenet, strNewTenet, nNumTCHAR * sizeof(TCHAR));
	pSend->szNewTenet[nNumTCHAR] = _T('\0');

	g_dbSession.Send(pSend, pSend->dwSize);

	MDEL_MSG(pSend);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 修改帮派徽标//?? -- 下一阶段工作
//-------------------------------------------------------------------------------
DWORD Guild::ModifySgin(DWORD dwRoleID, LPCTSTR strNewURL, INT32 nNumTCHAR)
{
	return GT_INVALID;
}

//-------------------------------------------------------------------------------
// 修改帮派团购指数
//-------------------------------------------------------------------------------
DWORD Guild::ModifyGroupPurchase(BOOL bInc)
{
	INT32 nGroupPurchase = m_Att.nGroupPurchase;

	if (bInc)
	{
		if(m_Att.nGroupPurchase < MAX_GUILD_GROUP_PURCHASE)
			m_Att.nGroupPurchase++;
		else
			m_Att.nGroupPurchase = MAX_GUILD_GROUP_PURCHASE;
	}
	else
	{
		if(m_Att.nGroupPurchase > 0)
			m_Att.nGroupPurchase--;
		else
			m_Att.nGroupPurchase = 0;
	}

	if (nGroupPurchase != m_Att.nGroupPurchase)
	{
		SaveGuildAtt2DB();
	}
	return E_Success;
}

//-------------------------------------------------------------------------------
// 获取指定成员扩展信息
//-------------------------------------------------------------------------------
DWORD Guild::SendSpecMemberEx2Client(Role *p2Role, DWORD dwSpecRoleID)
{
	ASSERT(P_VALID(p2Role));

	tagGuildMember *pSpecMember = GetMember(dwSpecRoleID);
	if(!P_VALID(pSpecMember))
	{
		// 正常情况下不会执行到该处 -- 修改客户端消息等
		return GT_INVALID;
	}

	tagNS_GuildGetMemberEx send;
	send.dwRoleID						= dwSpecRoleID;
	send.sGuildMemInfoEx.nExploit		= pSpecMember->nExploit;
	send.sGuildMemInfoEx.nTotalContrib	= pSpecMember->nTotalContribution;
	send.sGuildMemInfoEx.nKnowledge		= GT_INVALID;

	Role *pSpecRole = g_roleMgr.GetRolePtrByID(dwSpecRoleID);
	if(P_VALID(pSpecRole))
	{
		send.sGuildMemInfoEx.nKnowledge = pSpecRole->GetAttValue(ERA_Knowledge);
	}

	p2Role->SendMessage(&send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 更新指定成员的详细信息
//-------------------------------------------------------------------------------
DWORD Guild::SendSpecMember2Client(Role *p2Role, DWORD dwSpecRoleID)
{
	ASSERT(P_VALID(p2Role));

	tagGuildMember *pSpecMember = GetMember(dwSpecRoleID);
	if(!P_VALID(pSpecMember))
	{
		// 正常情况下不会执行到该处 -- 修改客户端消息等
		return GT_INVALID;
	}

	tagNS_GuildRefMember send;
	send.sGuildMemInfo.dwRoleID			= dwSpecRoleID;
	send.sGuildMemInfo.n8GuildPos		= pSpecMember->eGuildPos;
	send.sGuildMemInfo.nCurContrib		= pSpecMember->nContribution;

	send.sGuildMemInfoEx.nTotalContrib	= pSpecMember->nTotalContribution;
	send.sGuildMemInfoEx.nExploit		= pSpecMember->nExploit;

	Role *pSpecRole = g_roleMgr.GetRolePtrByID(dwSpecRoleID);
	if(P_VALID(pSpecRole))		// 在线
	{
		send.sGuildMemInfo.byLevel			= (BYTE)pSpecRole->GetLevel();
		send.sGuildMemInfo.bySex			= pSpecRole->GetSex();
		send.sGuildMemInfo.byClass			= pSpecRole->GetClass();
		send.sGuildMemInfo.dwTimeLastLogout	= GT_INVALID;
		send.sGuildMemInfo.bOnline			= true;

		send.sGuildMemInfoEx.nKnowledge		= pSpecRole->GetAttValue(ERA_Knowledge);
	}
	else	// 离线
	{
		tagRoleInfo *pSpecRoleInfo = g_roleMgr.GetRoleInfo(dwSpecRoleID);
		if(!P_VALID(pSpecRoleInfo))
		{
			// 帮派逻辑成员管理逻辑有问题
			ASSERT(0);
			return GT_INVALID;
		}

		send.sGuildMemInfo.byLevel			= pSpecRoleInfo->byLevel;
		send.sGuildMemInfo.bySex			= pSpecRoleInfo->bySex;
		send.sGuildMemInfo.byClass			= pSpecRoleInfo->byClass;		//?? 还没有职业
		send.sGuildMemInfo.dwTimeLastLogout	= pSpecRoleInfo->dwTimeLastLogout;
		send.sGuildMemInfo.bOnline			= false;

		send.sGuildMemInfoEx.nKnowledge		= GT_INVALID;
	}

	p2Role->SendMessage(&send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获取帮派名称
//-------------------------------------------------------------------------------
DWORD Guild::SendGuildName2Client(PlayerSession *pSession)
{
	ASSERT(P_VALID(pSession));
	
	INT32 nNumTCHAR = GetGuildAtt().strName.size();
	ASSERT(nNumTCHAR > 0);

	INT32 nMsgSz = sizeof(tagNS_GetGuildName) + nNumTCHAR * sizeof(TCHAR);

	MCREATE_MSG(pSend, nMsgSz, NS_GetGuildName);
	pSend->dwGuildID	= GetGuildAtt().dwID;
	_tcscpy(pSend->szGuildName, GetGuildAtt().strName.c_str());
	pSession->SendMessage(pSend, pSend->dwSize);
	MDEL_MSG(pSend);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获取帮派宗旨 -- 没有，则不给客户端反馈
//-------------------------------------------------------------------------------
DWORD Guild::SendGuildTenet2Client(PlayerSession *pSession)
{
	ASSERT(P_VALID(pSession));
	
	INT32 nNumTCHAR = GetGuildAtt().strTenet.size();
	if(nNumTCHAR <=0)
	{
		return E_Success;
	}

	INT32 nMsgSz = sizeof(tagNS_GetGuildTenet) + nNumTCHAR * sizeof(TCHAR);

	MCREATE_MSG(pSend, nMsgSz, NS_GetGuildTenet);
	_tcscpy(pSend->szGuildTenet, GetGuildAtt().strTenet.c_str());
	pSession->SendMessage(pSend, pSend->dwSize);
	MDEL_MSG(pSend);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 获取帮派设施升级信息
//-------------------------------------------------------------------------------
DWORD Guild::SendFacilitiesInfo2Client(Role* p2Role)
{
	ASSERT(P_VALID(p2Role));

	tagGuildMember* pMember = m_MemberMgr.GetMember(p2Role->GetID());
	if (!P_VALID(pMember))
	{
		return E_Guild_MemberNotIn;
	}

	// 判断玩家权限
	//if (!GetGuildPower(pMember->eGuildPos).bUpgrade)
	//{
	//	return E_Guild_Power_NotEnough;
	//}

	// 发送设施信息
	tagNS_GetGuildUpgradeInfo send;

	DWORD dwErrorCode = m_GuildUpgrade.GetGuildFacilitiesInfo(send.sFacilitiesInfo);

	if (E_Success == dwErrorCode)
	{
		// 重新设置消息大小
		p2Role->SendMessage(&send, send.dwSize);
	}

	return dwErrorCode;
}

//-------------------------------------------------------------------------------
// 删除角色时清理相关数据
//-------------------------------------------------------------------------------
DWORD Guild::ClearRoleRemove(DWORD dwRoleID)
{
	// 获取帮派成员属性
	tagGuildMember *pMember = GetMember(dwRoleID);
	if(!P_VALID(pMember))
	{
		ASSERT(0);
		return GT_INVALID;
	}

	// 判断权限
	if(!GetGuildPower(pMember->eGuildPos).bLeaveGuild)
	{
		return E_Guild_CanNot_Leave;
	}

	// 从管理器中删除
	RemoveMember(dwRoleID, pMember->eGuildPos);

	return E_Success;
}

VOID  Guild::ChangeName(LPCTSTR lpTGuildNewName)
{
	  tstring tOldName = m_Att.strName ;
      m_Att.strName = lpTGuildNewName ;
	  UpdateGuildNameTo2DB();
       

	  //若原来改过名字，就得先把空帮派从数据删除 否则就不用删除
	  DWORD dwOldGuildId = g_world.LowerCrc32(tOldName.c_str());
	  Guild* pOldGuild = g_guildMgr.GetGuild(dwOldGuildId);
	  if ( P_VALID(pOldGuild) )
	  {
		  if ( pOldGuild->m_Att.dwLeaderRoleID == 0 )
		  {
			  //把已有的帮派空壳子删了,还要把帮派管理器里的也删了
			  g_guildMgr.SendDismissGuild2DB(dwOldGuildId);
			  SAFE_DEL(pOldGuild);
			  g_guildMgr.GetMapGuild().Erase(dwOldGuildId);
		  }
	  }

       
	  //创建一个空帮派壳子保存旧名称和新的guildId
	  DWORD dwNewGuildId = g_world.LowerCrc32(lpTGuildNewName);
      Guild* pGuild = new Guild;
	  if ( P_VALID( pGuild ) )
	  {
		 
          pGuild->m_Att.dwID = dwNewGuildId ;
		  pGuild->m_Att.dwLeaderRoleID = 0;
		  pGuild->m_Att.dwFounderNameID = m_Att.dwLeaderRoleID; //是这样的
		  pGuild->m_Att.strName = tOldName.c_str();
          pGuild->m_Att.nReputation   = 0;
		  pGuild->m_Att.nDailyCost    = 0;
		  pGuild->m_Att.bCommendation = FALSE;
          pGuild->m_Att.byAffairRemainTimes = 0 ;
		  pGuild->m_Att.byMinJoinLevel = 1 ;
		  pGuild->m_Att.n16Rank       = 0 ;
		  pGuild->m_Att.n16MemberNum  = 1 ;
		  pGuild->m_Att.n16Peace      = 0 ;
		  pGuild->m_Att.nGroupPurchase = 0;
          pGuild->m_Att.nReputation = 0 ;
		  pGuild->m_Att.nMaterial = 0;
		  pGuild->m_Att.nFund = 0 ;
          pGuild->m_Att.byLevel = 0 ;
          pGuild->m_Att.dwSpecState = EGDSS_Distress ;
		  pGuild->m_Att.dwGuildValue1 = 0;
		  pGuild->m_Att.dwGuildValue2 = 0;
		  g_guildMgr.GetMapGuild().Add(dwNewGuildId,pGuild);
		  g_guildMgr.SendCreateGuild2DB(pGuild->m_Att);
	  }


}

 VOID   Guild::SetGuildAtt(const tagGuildLoad *pGuildLoad)
 {
	 memset(&m_Att,0,sizeof(m_Att));
	 m_Att.dwID				= pGuildLoad->dwID;
	 m_Att.dwFounderNameID	= pGuildLoad->dwFounderNameID;
	 m_Att.dwLeaderRoleID	= pGuildLoad->dwLeaderRoleID;

	 // 帮派状态
	 m_Att.dwSpecState		= pGuildLoad->dwSpecState;
	 m_GuildState.SetState((EGuildSpecState)m_Att.dwSpecState);

	 m_Att.byLevel			= pGuildLoad->byLevel;

	 m_Att.nFund				= pGuildLoad->nFund;
	 m_Att.nMaterial			= pGuildLoad->nMaterial;
	 m_Att.nReputation		= pGuildLoad->nReputation;
	 m_Att.n16Peace			= pGuildLoad->n16Peace;
	 m_Att.n16MemberNum		= pGuildLoad->n16MemberNum;
	 m_Att.n16Rank			= pGuildLoad->n16Rank;
	 m_Att.byMinJoinLevel	= pGuildLoad->byMinJoinLevel;
	 m_Att.nDailyCost		= pGuildLoad->nDailyCost;
	 m_Att.nGroupPurchase	= pGuildLoad->nGroupPurchase;
	 m_Att.byAffairRemainTimes = pGuildLoad->byAffairRemainTimes;
	 m_Att.bCommendation		= pGuildLoad->bCommendation;
	 m_Att.dwCreateTime		= pGuildLoad->dwCreateTime;

	 // 帮派名,宗旨,徽标
	 m_Att.strName			= pGuildLoad->szName;
	 m_Att.strTenet			= pGuildLoad->szTenet;
	 m_Att.strSymbol		= pGuildLoad->szSymbolURL;

	 m_Att.dwGuildValue1	= pGuildLoad->dwGuildValue1;
	 m_Att.dwGuildValue2	= pGuildLoad->dwGuildValue2;

	 // 更新帮派消耗
	 m_Att.nDailyCost = (INT32)MGuildDailyCost(m_Att.byLevel, m_Att.n16Peace);
 }

 //-------------------------------------------------------------------------------
 //帮派改名通知所有帮派成员及其每个帮派周围9空格
 //-------------------------------------------------------------------------------
 VOID  Guild::SendChangeGuildNameMsg(LPVOID pMsg, DWORD dwSize)
 {
        m_MemberMgr.SendChangeGuildNameMsg(pMsg, dwSize);
 }

//-------------------------------------------------------------------------------
// 保存帮派属性到数据库
//-------------------------------------------------------------------------------
VOID Guild::SaveGuildAtt2DB()
{
	tagNDBC_SaveGuildAtt send;
	send.pGuildBase = (tagGuildBase)m_Att;

	g_dbSession.Send(&send, send.dwSize);
}

VOID   Guild::UpdateGuildNameTo2DB()
{
	  tagNDBC_ChangeGuildName send;
	  send.dwGuildID = m_Att.dwID;
	  _tcscpy_s(send.szGuildName,m_Att.strName.c_str());
	  g_dbSession.Send(&send,send.dwSize);
}


//-------------------------------------------------------------------------------
// 设置帮派仓库操作权限
//-------------------------------------------------------------------------------
DWORD Guild::SetGuildWarePrivilege( Role* pRole, DWORD dwRoleID, BOOL bCanUse )
{
	//上层保证玩家和帮派的合法性
	ASSERT(P_VALID(pRole));

	// 玩家非法
	if (!GT_VALID(dwRoleID))
	{
		return GT_INVALID;
	}

	// 判断操作者的权限
	tagGuildMember* pMember = GetMember(pRole->GetID());
	if (!P_VALID(pMember))
	{
		// 异常,所在帮派无此人
		ASSERT(pMember);
		return E_Guild_MemberNotIn;
	}

	if (!GetGuildPower(pMember->eGuildPos).bSetWareRights)
	{
		return E_Guild_Power_NotEnough;
	}

	// 检查被操作者
	pMember = GetMember(dwRoleID);
	if (!P_VALID(pMember))
	{
		// 被操作这不在该帮派中
		return E_Guild_MemberNotIn;
	}
	else if (GetGuildPower(pMember->eGuildPos).bSetWareRights)
	{
		return E_Guild_Power_NotEnough;
	}

	m_MemberMgr.SetGuildWarePrivilege(dwRoleID, bCanUse);

	return E_Success;
}

//-------------------------------------------------------------------------------
// 帮派属性变更
//-------------------------------------------------------------------------------
BOOL Guild::IncGuildFund( DWORD dwRoleID, INT32 nFund, DWORD dwCmdID )
{
	if (nFund <= 0)
	{
		return FALSE;
	}

	m_Att.nFund += nFund;
	INT32 nMaxFund = m_GuildUpgrade.GetMaxFund();
	if (m_Att.nFund > nMaxFund || m_Att.nFund < 0)
	{
		nFund = m_Att.nFund - nMaxFund;
		m_Att.nFund = nMaxFund;
	}

	LogGuildFund(dwRoleID, nFund, m_Att.nFund, dwCmdID);

	if (IsInGuildState(EGDSS_Distress))
	{
		UnsetGuildState(EGDSS_Distress);
		if (m_Att.nFund < m_Att.nDailyCost*3)
		{
			SetGuildState(EGDSS_Shortage);
		}
	}
	else if (IsInGuildState(EGDSS_Shortage))
	{
		if (m_Att.nFund >= m_Att.nDailyCost*3)
		{
			UnsetGuildState(EGDSS_Shortage);
		}
	}

	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

BOOL Guild::IncGuildMaterial( DWORD dwRoleID, INT32 nMaterial, DWORD dwCmdID )
{
	if (nMaterial <= 0)
	{
		return FALSE;
	}

	m_Att.nMaterial += nMaterial;

	INT32 nMaxMaterial = m_GuildUpgrade.GetMaxMaterial();
	if (m_Att.nMaterial > nMaxMaterial)
	{
		nMaterial = m_Att.nMaterial - nMaxMaterial;
		m_Att.nMaterial = nMaxMaterial;
	}
	LogGuildMaterial(dwRoleID, nMaterial, m_Att.nMaterial, dwCmdID);

	// 保存到数据库
	SaveGuildAtt2DB();

	// 资材现在不涉及帮派状态状态

	return TRUE;
}

BOOL Guild::IncGuildReputation( DWORD dwRoleID, INT32 nReputation, DWORD dwCmdID )
{
	if (nReputation <= 0)
	{
		return FALSE;
	}

	m_Att.nReputation += nReputation;

	if (m_Att.nReputation > MAX_GUILD_REP)
	{
		nReputation = m_Att.nReputation - MAX_GUILD_REP;
		m_Att.nReputation = MAX_GUILD_REP;
	}
	LogGuildReputation(dwRoleID, nReputation, m_Att.nReputation, dwCmdID);

	// 这里涉及了帮派的名次，调用g_GuildMgr接口


	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

BOOL Guild::IncGuildPeace( DWORD dwRoleID, INT16 nPeace, DWORD dwCmdID )
{
	if (nPeace <= 0)
	{
		return FALSE;
	}

	m_Att.n16Peace += nPeace;

	if (m_Att.n16Peace > MAX_GUILD_PEACE)
	{
		nPeace = m_Att.n16Peace - MAX_GUILD_PEACE;
		m_Att.n16Peace = MAX_GUILD_PEACE;
	}

	// 更新帮派消耗
	m_Att.nDailyCost = (INT32)MGuildDailyCost(m_Att.byLevel, m_Att.n16Peace);

	//??安定度Log

	if (IsInGuildState(EGDSS_Chaos))
	{
		if (m_Att.n16Peace >= 500)
		{
			UnsetGuildState(EGDSS_Chaos);
		}
	}

	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

BOOL Guild::DecGuildFund( DWORD dwRoleID, INT32 nFund, DWORD dwCmdID )
{
	if (nFund <= 0)
	{
		return FALSE;
	}

	m_Att.nFund -= nFund;
	if (m_Att.nFund < 0)
	{
		nFund += m_Att.nFund;
		m_Att.nFund = 0;
	}
	LogGuildFund(dwRoleID, -nFund, m_Att.nFund, dwCmdID);

	if (!IsInGuildStateAny(EGDSS_Distress | EGDSS_Shortage))
	{
		if (m_Att.nFund == 0)
		{
			SetGuildState(EGDSS_Distress);
		}
		else if (m_Att.nFund < m_Att.nDailyCost*3)
		{
			SetGuildState(EGDSS_Shortage);
		}
	}
	else if (IsInGuildState(EGDSS_Shortage))
	{
		if (m_Att.nFund == 0)
		{
			UnsetGuildState(EGDSS_Shortage);
			SetGuildState(EGDSS_Distress);
		}
	}

	// 保存到数据库
	SaveGuildAtt2DB();
	
	return TRUE;
}

BOOL Guild::DecGuildMaterial( DWORD dwRoleID, INT32 nMaterial, DWORD dwCmdID )
{
	if (nMaterial <= 0)
	{
		return FALSE;
	}

	m_Att.nMaterial -= nMaterial;
	if (m_Att.nMaterial < 0)
	{
		nMaterial += m_Att.nMaterial;
		m_Att.nMaterial = 0;
	}
	LogGuildMaterial(dwRoleID, -nMaterial, m_Att.nMaterial, dwCmdID);

	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

BOOL Guild::DecGuildReputation( DWORD dwRoleID, INT32 nReputation, DWORD dwCmdID )
{
	if (nReputation <= 0)
	{
		return FALSE;
	}

	m_Att.nReputation -= nReputation;
	if (m_Att.nReputation < 0)
	{
		nReputation += m_Att.nReputation;
		m_Att.nReputation = 0;
	}
	LogGuildReputation(dwRoleID, -nReputation, m_Att.nReputation, dwCmdID);

	// 这里进行帮派排名


	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

BOOL Guild::DecGuildPeace( DWORD dwRoleID, INT16 nPeace, DWORD dwCmdID )
{
	if (nPeace <= 0)
	{
		return FALSE;
	}

	m_Att.n16Peace -= nPeace;
	if (m_Att.n16Peace < 0)
	{
		nPeace += m_Att.n16Peace;
		m_Att.n16Peace = 0;
	}

	// 更新帮派消耗
	m_Att.nDailyCost = (INT32)MGuildDailyCost(m_Att.byLevel, m_Att.n16Peace);

	//??安定度Log

	if (!IsInGuildState(EGDSS_Chaos))
	{
		if (m_Att.n16Peace < 500)
		{
			SetGuildState(EGDSS_Chaos);
		}
	}

	// 保存到数据库
	SaveGuildAtt2DB();

	return TRUE;
}

//-----------------------------------------------------------------------------
// 记录LOG
//-----------------------------------------------------------------------------
VOID Guild::LogGuildFund( DWORD dwRoleID, INT32 nFund, INT32 nTotalFund, DWORD dwCmdID )
{
	tagNDBC_LogFund send;
	
	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if (!P_VALID(pRoleInfo))
	{
		return;
	}

	send.sLogFund.dwAccountID	= pRoleInfo->dwAccountID;
	send.sLogFund.dwCmdID		= dwCmdID;
	send.sLogFund.dwGuildID		= m_Att.dwID;
	send.sLogFund.dwRoleID		= dwRoleID;
	send.sLogFund.nFund			= nFund;
	send.sLogFund.nTotalFund	= nTotalFund;

	g_dbSession.Send(&send, send.dwSize);
}

VOID Guild::LogGuildMaterial( DWORD dwRoleID, INT32 nMaterial, INT32 nTotalMaterial, DWORD dwCmdID )
{
	tagNDBC_LogMaterial send;

	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if (!P_VALID(pRoleInfo))
	{
		return;
	}

	send.sLogMaterial.dwAccountID		= pRoleInfo->dwAccountID;
	send.sLogMaterial.dwCmdID			= dwCmdID;
	send.sLogMaterial.dwGuildID			= m_Att.dwID;
	send.sLogMaterial.dwRoleID			= dwRoleID;
	send.sLogMaterial.nMaterial			= nMaterial;
	send.sLogMaterial.nTotalMaterial	= nTotalMaterial;

	g_dbSession.Send(&send, send.dwSize);
}

VOID Guild::LogGuildReputation( DWORD dwRoleID, INT32 nReputation, INT32 nTotalReputation, DWORD dwCmdID )
{
	tagNDBC_LogReputation send;

	tagRoleInfo* pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if (!P_VALID(pRoleInfo))
	{
		return;
	}

	send.sLogReputation.dwAccountID			= pRoleInfo->dwAccountID;
	send.sLogReputation.dwCmdID				= dwCmdID;
	send.sLogReputation.dwGuildID			= m_Att.dwID;
	send.sLogReputation.dwRoleID			= dwRoleID;
	send.sLogReputation.nReputation			= nReputation;
	send.sLogReputation.nTotalReputation	= nTotalReputation;

	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------
// 每日重置帮派事务状态并扣除消耗
//-------------------------------------------------------------------------------
VOID Guild::DailyGuildReset()
{
	// 扣除帮派每日资金和安定消耗
	DecGuildFund(GT_INVALID, m_Att.nDailyCost, ELCLD_Guild_DailyCost);
	DecGuildPeace(GT_INVALID, MGuildPeaceCost(m_Att.byLevel), ELCLD_Guild_DailyCost);

	// 奖励跑商贡献比较大的玩家
	m_GuildCommerce.ProvideBonus();

	// 重置帮派状态发布次数
	m_GuildAffair.ResetAffairCounter();

	//??保存一下帮派相关信息
	SaveGuildAtt2DB();
}

// 返回此帮派占神迹的个数
INT32 Guild::GetCaptureGodMiracleNum()
{	
	INT32 godMiracleNum(0);
	//g_ScriptMgr.CallScriptFunction("GetGuildCaptureGodMiracleNum", "u>i", GetGuildAtt().dwID, &godMiracleNum);
	godMiracleNum = CGodMiracleMgr::GetInst()->GetGuildDefendGodMiracle(GetGuildAtt().dwID);
	return godMiracleNum;
}

#if 0// 《圣魔之血》战场不需要这个buff了
DWORD GetBuffIDByGodNum(INT16 GodNum)
{
	if (1 == GodNum)
		return 2024101;
	if (2 == GodNum)
		return 2024102;
	if ( 3 == GodNum)
		return 2024103;
	if ( 4 == GodNum)
		return 2024104;
	if ( 5 == GodNum)
		return 2024105;
	return 0;
}
#endif

//-------------------------------------------------------------------------------
// 添加成员
//-------------------------------------------------------------------------------
DWORD Guild::AddMember(DWORD dwRoleID, EGuildMemberPos eGuildPos, BOOL bSave2DB/* = TRUE*/)
{
	if(m_MemberMgr.IsExist(dwRoleID))
	{
		return E_Success;
	}

	DWORD dwErrorCode = m_GuildPos.AddMember(dwRoleID, eGuildPos);
	if(E_Success == dwErrorCode)
	{
		m_MemberMgr.AddMember(dwRoleID, eGuildPos, bSave2DB);
		m_Att.n16MemberNum = GetGuildMemberNum();
		// Jason 2010-3-2 v1.3.2扩充离线玩家信息
		tagRoleInfo * pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
		if( P_VALID(pRoleInfo) )
		{
			pRoleInfo->dwGuildID = GetGuildAtt().dwID;
		}
	}
	ASSERT(E_Success == dwErrorCode);

	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if ( NULL == pRole)
		return dwErrorCode;

	// 同帮派在线人数发生变化，通知客户端
	SyncOnlineNum2AllGuildMumber();

	// Jason 2010-3-11 v1.4.0
	tagNS_GuildExploit send;
	send.dwRoleID			= dwRoleID;
	send.nExploit		= 0;
	pRole->SendMessage(&send, send.dwSize);
	// Jason 2010-3-4 v1.4.0帮派功勋称号
	pRole->GetTitleMgr()->SigEvent( ETE_GUILD_EXPLOIT, GT_INVALID ,0 );

#if 0// 《圣魔之血》战场不需要这个buff了
	// 查看本帮派是否占有神迹
 	INT16 n16GodNum(0);
 	n16GodNum = GetCaptureGodMiracleNum();
 	if ( 0 == n16GodNum)
		return dwErrorCode;

	DWORD dwBuffID(0);
 	dwBuffID = GetBuffIDByGodNum(n16GodNum);
	if ( 0 == dwBuffID)
		return dwErrorCode;

	const tagBuffProto* pBuffProto = g_attRes.GetBuffProto(dwBuffID);
 	if ( NULL == pBuffProto)
	 	return dwErrorCode;

 	pRole->TryAddBuff(static_cast<Unit*>(pRole), pBuffProto, NULL, NULL, NULL);
#endif
	return dwErrorCode;
}

// Jason 2010-3-3 v1.3.2 增加批准申请人接口
DWORD	Guild::ApproveToJoinGuild(DWORD dwMasterID, DWORD dwApplier)
{
	DWORD dwErrorCode = InviteJoin(dwMasterID, dwApplier);
	if( E_Role_Not_Online != dwErrorCode && E_Guild_Join_BeInvited != dwErrorCode && E_Success != dwErrorCode && E_Guild_Join_NotBeInvited != dwErrorCode )
		return GT_INVALID;

	if( E_Role_Not_Online == dwErrorCode || E_Guild_Join_NotBeInvited == dwErrorCode )
	{
		Role * pApplier = g_roleMgr.GetRolePtrByID(dwApplier);
		AddMember(dwApplier,EGMP_HuiYuan);
		if( P_VALID(pApplier) )
		{
			pApplier->SetGuildID(m_Att.dwID);
			// Jason 2010-3-10 通知给被邀请者，帮派宗旨
			SendGuildTenet2Client(pApplier->GetSession());
		}
		return E_Success;
	}
	return dwErrorCode;
}
//-------------------------------------------------------------------------------
// 删除成员
//-------------------------------------------------------------------------------
VOID Guild::RemoveMember(DWORD dwRoleID, EGuildMemberPos eGuildPos)
{
	m_GuildPos.RemoveMember(dwRoleID, eGuildPos);
	// 职位变更导致其他属性变化
	m_GuildCommerce.AbandonCommerce(dwRoleID, TRUE);

	m_MemberMgr.RemoveMember(dwRoleID);
	m_Att.n16MemberNum = GetGuildMemberNum();

	// Jason 2010-3-2 v1.3.2扩充离线玩家信息
	tagRoleInfo * pRoleInfo = g_roleMgr.GetRoleInfo(dwRoleID);
	if( P_VALID(pRoleInfo) )
	{
		pRoleInfo->dwGuildID = GT_INVALID;
	}

	Role* pRole = g_roleMgr.GetRolePtrByID(dwRoleID);
	if ( NULL == pRole)
	{//不在线玩家直接添加buff到db
		g_roleMgr.OfflineRoleAddBuff(dwRoleID, LEAVE_GUILD_BUFF_ID);
		return ;
	}

	// 给玩家添加“帮派决裂”的buff,(bufferID：LEAVE_GUILD_BUFF_ID)
	const tagBuffProto* pGuildRemoveBuffProto = g_attRes.GetBuffProto(LEAVE_GUILD_BUFF_ID);
	if (NULL != pGuildRemoveBuffProto)
	{
		pRole->TryAddBuff(static_cast<Unit*>(pRole), pGuildRemoveBuffProto, NULL, NULL, NULL);
	}

	// Jason 2010-3-11 v1.4.0
	tagNS_GuildExploit send;
	send.dwRoleID			= dwRoleID;
	send.nExploit		= 0;
	pRole->SendMessage(&send, send.dwSize);
	// Jason 2010-3-4 v1.4.0帮派功勋称号
	pRole->GetTitleMgr()->SigEvent( ETE_GUILD_EXPLOIT, GT_INVALID ,0 );

#if 0// 《圣魔之血》战场不需要这个buff了
	// 查看本帮派是否占有神迹
 	INT16 n16GodNum(0);
	n16GodNum = GetCaptureGodMiracleNum();
	if ( 0 == n16GodNum)
		return ;
 
 	DWORD dwBuffID(0);
 	dwBuffID = GetBuffIDByGodNum(n16GodNum);
	if ( 0 == dwBuffID)
		return ;

 	const tagBuffProto* pBuffProto = g_attRes.GetBuffProto(dwBuffID);
 	if ( NULL == pBuffProto)
		return ;

	pRole->RemoveBuff(Buff::GetIDFromTypeID(dwBuffID), TRUE);
#endif
}

//-------------------------------------------------------------------------------
// 帮派升级
//-------------------------------------------------------------------------------
VOID Guild::ReInitbyGuildUpgrade( BYTE byNewLevel /*= 0*/ )
{
	// 判断参数的合法性
	if (byNewLevel > MAX_GUILD_LEVEL)
	{
		return;
	}

	if (m_Att.byLevel == byNewLevel)
	{
		return;
	}

	// 记录原等级
	BYTE byOldLevel = m_Att.byLevel;

	// 设置帮派新等级
	if (byNewLevel == 0)
	{
		m_Att.byLevel++;
	}
	else
	{
		m_Att.byLevel = byNewLevel;
	}

	// 重新计算与帮派等级相关属性
	m_GuildPos.Init(m_Att.byLevel);
	m_GuildWarehouse.ReInit();

	m_Att.nDailyCost = (INT32)MGuildDailyCost(m_Att.byLevel, m_Att.n16Peace);
	ReCalAffairRemainTimes(MGuildAffairTimes(byOldLevel) - m_Att.byAffairRemainTimes);

	// 保存到数据库
	SaveGuildAtt2DB();
}

//-------------------------------------------------------------------------------
// 重新计算帮务剩余发布次数
//-------------------------------------------------------------------------------
VOID Guild::ReCalAffairRemainTimes( BYTE bySpreadTimes )
{
	BYTE byTimes = m_Att.byAffairRemainTimes;
	m_Att.byAffairRemainTimes	= MGuildAffairTimes(m_Att.byLevel) - bySpreadTimes;

	if (byTimes != m_Att.byAffairRemainTimes)
	{
		SaveGuildAtt2DB();
	}
}
//-------------------------------------------------------------------------------
// 占领城市 这里的cityindex就是cityid
//-------------------------------------------------------------------------------
BOOL Guild::HoldCity( BYTE byCityIndex )
{
	if (byCityIndex == 0)
	{
		return FALSE;
	}

	for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
	{
		if (m_Att.byHoldCity[n] == 0)
		{
			m_Att.byHoldCity[n] = byCityIndex;
			SaveGuildAtt2DB();
			return TRUE;
		}
	}
	return FALSE;

}
//-------------------------------------------------------------------------------
// 释放这个城市的掌控
//-------------------------------------------------------------------------------
BOOL Guild::ReleaseCity( BYTE byCityIndex )
{
	if (byCityIndex == 0)
	{
		return FALSE;
	}

	for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
	{
		if (m_Att.byHoldCity[n] == byCityIndex)
		{
			m_Att.byHoldCity[n] = 0;
			SaveGuildAtt2DB();
			return TRUE;
		}
	}
	return FALSE;
}
//-------------------------------------------------------------------------------
// 检查帮派是否占领了指定城市 这里的cityindex就是cityid
//-------------------------------------------------------------------------------
BOOL Guild::IsHoldCity( BYTE byCityIndex )
{
	if (byCityIndex == 0)
	{
		return TRUE;
	}

	for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
	{
		if (m_Att.byHoldCity[n] == byCityIndex)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//-------------------------------------------------------------------------------
// 检查帮派是否占领城市数量达到上限
//-------------------------------------------------------------------------------
BOOL Guild::IsHoldCityMax()
{
	INT nCityNum = 0;
	for (int n=0; n<MAX_GUILD_HOLDCITY; n++)
	{
		if (m_Att.byHoldCity[n] != 0)
		{
			nCityNum++;
		}
	}

	return nCityNum >= MGuildMaxHoldCityNum(m_Att.byLevel);
}

//-------------------------------------------------------------------------------
// 给所有帮派成员发送在线人数变化消息
//-------------------------------------------------------------------------------
VOID Guild::SyncOnlineNum2AllGuildMumber(BOOL bLogout)
{
	m_MemberMgr.SyncOnlineNum2AllGuildMumber(bLogout);
}

VOID Guild::SetGuildValue1(DWORD dwGuildValue)
{
	m_Att.dwGuildValue1 = dwGuildValue;
}

VOID Guild::SetGuildValue2(DWORD dwGuildValue)
{
	m_Att.dwGuildValue2 = dwGuildValue;
}

DWORD Guild::GetGuildValue1()
{
	return m_Att.dwGuildValue1;
}

DWORD Guild::GetGuildValue2()
{
	return m_Att.dwGuildValue2;
}