//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loot_mgr.cpp
// author: 
// actor:
// data: 2008-10-23
// last: 2008-10-27
// brief: 掉落管理器
//------------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "../WorldDefine/loot.h"
#include "../WorldDefine/msg_loot.h"
#include "../WorldDefine/chat.h"
#include "../WorldDefine/chat_define.h"
#include "../WorldDefine/ItemDefine.h"
#include "../ServerDefine/log_cmdid_define.h"

#include "loot_mgr.h"
#include "unit.h"
#include "role.h"
#include "item_creator.h"
#include "creature.h"
#include "MsgInfoMgr.h"
#include "shielding_mgr.h"
#include "global_limited_item.h"

//-------------------------------------------------------------------------------------------------------
// 构造函数
//-------------------------------------------------------------------------------------------------------
LootMgr::LootMgr()
:m_Trunk(this)
{
	m_dwReloadFlag = 0;
}

//-------------------------------------------------------------------------------------------------------
// 析构函数
//-------------------------------------------------------------------------------------------------------
LootMgr::~LootMgr()
{
	Destroy();
}

//-------------------------------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::Init()
{
	m_pUtil = "Util";

	// 加载怪物掉落
	if( !LoadCreatureLoot() )
	{
		return FALSE;
	}

	// 加载掉落集合
	if( !LoadItemSetLoot() )
	{
		return FALSE;
	}

	// 加载任务物品掉落
	if( !LoadQuestItemLoot() )
	{
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 加载怪物资源
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::LoadCreatureLoot()
{
	// 生成掉落容器
	CreateObj("CreatureLoot", "VarContainer");
	m_pVar = "CreatureLoot";

	// 从配置文件 world.ini 中得到掉落文件路径
	TObjRef<VarContainer> pVar = "VarContainer";

	// 得到配置文件中怪物掉落表的路径
	tstring strCreatureLoot	= pVar->GetString(_T("creature_loot"),	_T("loot"));

	IMSG(_T("Loading %s\r\n"), strCreatureLoot.c_str());
	// 加载怪物掉落文件
	std::list<tstring> listField;
	m_pVar->Load(NULL, strCreatureLoot.c_str(), "id", &listField);

	// 一个一个的加载怪物掉落文件
	for(std::list<tstring>::iterator it = listField.begin(); it != listField.end(); ++it)
	{
		tagCreatureLoot* pCreatureLoot = new tagCreatureLoot;
		ZeroMemory(pCreatureLoot, sizeof(tagCreatureLoot));

		pCreatureLoot->dwLootID		=	_tcstol(it->c_str(), NULL, 10);
		pCreatureLoot->eOpType		=	(ELootOpType)m_pVar->GetDword(_T("optype"), it->c_str(), 0);
		pCreatureLoot->eLootMode	=	(ELootMode)m_pVar->GetDword(_T("mode"), it->c_str(), 0);
		pCreatureLoot->nMinMoney	=	m_pVar->GetDword(_T("min_money"), it->c_str(), 0);
		pCreatureLoot->nMaxMoney	=	m_pVar->GetDword(_T("max_money"), it->c_str(), 0);
		pCreatureLoot->dwKillCount	=	GT_INVALID;
		pCreatureLoot->dw_num	=	m_pVar->GetDword(_T("num"), it->c_str(), 1);//控制游戏币掉落的堆数  2010-07-21 Alex
		if(pCreatureLoot->dw_num<=0){pCreatureLoot->dw_num=1;} 

		

		// 掉落表物品
		for(INT i = 0; i < MAX_CREATURE_LOOT_NUM; ++i)
		{
			tstringstream ss;
			ss << _T("item_") << i+1 << _T("_id");
			pCreatureLoot->Loot[i].dwItemID = m_pVar->GetDword(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_type");
			pCreatureLoot->Loot[i].eLootType = (ELootType)m_pVar->GetInt(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_max");
			pCreatureLoot->Loot[i].nMax = m_pVar->GetInt(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_min");
			pCreatureLoot->Loot[i].nMin = m_pVar->GetInt(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_chance");
			pCreatureLoot->Loot[i].fChance = m_pVar->GetFloat(ss.str().c_str(), it->c_str(), 0.0f);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_num");
			pCreatureLoot->Loot[i].nKill = m_pVar->GetInt(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_team_chance");
			pCreatureLoot->Loot[i].fTeamChance = m_pVar->GetFloat(ss.str().c_str(), it->c_str(), 0.f);
			ss.str(_T(""));
			ss << _T("get_broadcast_") << i+1;
			pCreatureLoot->Loot[i].bBroadcast = m_pVar->GetInt(ss.str().c_str(), it->c_str(), 0)!=0;
			if(pCreatureLoot->Loot[i].nKill>0) pCreatureLoot->dwKillCount = 0;
		}

		m_mapCreatureLoot.Add(pCreatureLoot->dwLootID, pCreatureLoot);
	}

	m_pVar->Clear();
	KillObj("CreatureLoot");

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 加载掉落集合
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::LoadItemSetLoot()
{
	// 生成掉落容器
	CreateObj("ItemSetLoot", "VarContainer");
	m_pVar = "ItemSetLoot";

	// 从配置文件 world.ini 中得到掉落文件路径
	TObjRef<VarContainer> pVar = "VarContainer";
	tstring strLootItemSet = pVar->GetString(_T("loot_item_set"),	_T("loot"));

	IMSG(_T("Loading %s\r\n"), strLootItemSet.c_str());
	// 加载怪物掉落文件
	std::list<tstring> listField;
	// 加载掉落物品集合文件
	m_pVar->Load(NULL, strLootItemSet.c_str(), "id", &listField);

	for(std::list<tstring>::iterator it = listField.begin(); it != listField.end(); ++it)
	{
		tagLootItemSet* pLootItemSet = new tagLootItemSet;
		ZeroMemory(pLootItemSet, sizeof(tagLootItemSet));

		pLootItemSet->dwSetID = _tcstol(it->c_str(), NULL, 10);

		// 加载物品
		for(INT i = 0; i < MAX_ITEM_SET_NUM; ++i)
		{
			tstringstream ss;
			ss << _T("item_") << i+1 << _T("_id");
			pLootItemSet->ItemSet[i].dwItemID = m_pVar->GetDword(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_num");
			pLootItemSet->ItemSet[i].nItemNum = m_pVar->GetDword(ss.str().c_str(), it->c_str(), 0);
		}

		// 加入map
		m_mapLootItemSet.Add(pLootItemSet->dwSetID, pLootItemSet);
	}

	m_pVar->Clear();
	KillObj("ItemSetLoot");

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 加载任务掉落
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::LoadQuestItemLoot()
{
	// 生成掉落容器
	CreateObj("QuestItemLoot", "VarContainer");
	m_pVar = "QuestItemLoot";

	// 从配置文件 world.ini 中得到掉落文件路径
	TObjRef<VarContainer> pVar = "VarContainer";
	tstring strLootQuestItem = pVar->GetString(_T("loot_quest_item"),	_T("loot"));

	IMSG(_T("Loading %s\r\n"), strLootQuestItem.c_str());
	// 加载怪物掉落文件
	std::list<tstring> listField;
	// 加载任务掉落物品文件
	m_pVar->Load(NULL, strLootQuestItem.c_str(), "id", &listField);

	for(std::list<tstring>::iterator it = listField.begin(); it != listField.end(); ++it)
	{
		tagLootQuestItem* pLootQuestItem = new tagLootQuestItem;
		ZeroMemory(pLootQuestItem, sizeof(tagLootQuestItem));

		pLootQuestItem->dwCreatureID = _tcstol(it->c_str(), NULL, 10);

		for(INT i = 0; i < MAX_LOOT_QUEST_ITEM_NUM; ++i)
		{
			tstringstream ss;
			ss << _T("item_") << i+1 << _T("_id");
			pLootQuestItem->QuestItem[i].dwQuestItemID  = m_pVar->GetDword(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_chance");
			pLootQuestItem->QuestItem[i].fChance = m_pVar->GetFloat(ss.str().c_str(), it->c_str(), 0);
			ss.str(_T(""));
			ss << _T("item_") << i+1 << _T("_team_chance");
			pLootQuestItem->QuestItem[i].fTeamChance = m_pVar->GetFloat(ss.str().c_str(), it->c_str(), 0);
		}

		m_mapLootQuestItem.Add(pLootQuestItem->dwCreatureID, pLootQuestItem);
	}

	m_pVar->Clear();
	KillObj("QuestItemLoot");

	return TRUE;
}


//-------------------------------------------------------------------------------------------------------
// 释放资源
//-------------------------------------------------------------------------------------------------------
VOID LootMgr::Destroy()
{
	// 释放 怪物掉落
	m_mapCreatureLoot.ResetIterator();
	tagCreatureLoot* pCreatureLoot = NULL;
	while (m_mapCreatureLoot.PeekNext(pCreatureLoot))
	{
		SAFE_DEL(pCreatureLoot);
	}
	m_mapCreatureLoot.Clear();

	// 释放 掉落物品集合
	m_mapLootItemSet.ResetIterator();
	tagLootItemSet* pLootItemSet = NULL;
	while (m_mapLootItemSet.PeekNext(pLootItemSet))
	{
		SAFE_DEL(pLootItemSet);
	}
	m_mapLootItemSet.Clear();

	// 释放 任务掉落
	m_mapLootQuestItem.ResetIterator();
	tagLootQuestItem* pLootQuestItem = NULL;
	while (m_mapLootQuestItem.PeekNext(pLootQuestItem))
	{
		SAFE_DEL(pLootQuestItem);
	}
	m_mapLootQuestItem.Clear();
}

//---------------------------------------------------------------------------------------------
// 怪物掉落
//---------------------------------------------------------------------------------------------
VOID LootMgr::MonsterLoot(Creature* pCreature, Role* pPicker,TLootItemEvent func)
{
	if( !P_VALID(pCreature) || !P_VALID(pPicker) )
		return;

	// 进行小队任务掉落
	QuestLoot(pCreature, pPicker, TRUE);

	// 掉落在地面，非单一掉落， 小队掉落
	NormalLoot(pCreature, pPicker, ELT_Ground, FALSE, TRUE,func);
}

//----------------------------------------------------------------------------------------------
// 资源掉落
//----------------------------------------------------------------------------------------------
VOID LootMgr::ResourceLoot(Creature* pCreature, Role* pPicker)
{
	if( !P_VALID(pCreature) || !P_VALID(pPicker) )
		return;

	// 掉落在包里，包满则掉落在地面，单一掉落，非小队掉落
	NormalLoot(pCreature, pPicker, ELT_BagFirst, TRUE, FALSE);
}

//----------------------------------------------------------------------------------------------
// 调查地物掉落
//----------------------------------------------------------------------------------------------
VOID LootMgr::InvestigateLoot(Creature* pCreature, Role* pPicker)
{
	if( !P_VALID(pCreature) || !P_VALID(pPicker) )
		return;

	// 非小队任务掉落
	QuestLoot(pCreature, pPicker, FALSE);

	// 强制落在背包里，否则删除，非单一掉落，非小队掉落
	NormalLoot(pCreature, pPicker, ELT_Ground, FALSE, TRUE);
	//NormalLoot(pCreature, pPicker, ELT_ForceBag, FALSE, FALSE);
}



//-------------------------------------------------------------------------------------------------------
// 普通掉落
//-------------------------------------------------------------------------------------------------------
VOID LootMgr::NormalLoot(Creature* pCreature, Role* pRole, ELootTo eLootTo, BOOL bSingelLoot, BOOL bTeam,TLootItemEvent func)
{
	if( !P_VALID(pCreature) ) return;
	if( !P_VALID(pRole) ) return;

	// 掉落位置索引
	INT	nPosIndex =	0;

	// 得到怪物掉落
	tagCreatureLoot* pLootProto = (tagCreatureLoot*)GetCreatureLoot(pCreature->GetLootID());
	if(P_VALID(pLootProto) )
	{ 
       NormalLoot_ForLootID123(pCreature,pLootProto,nPosIndex,pRole,eLootTo,bSingelLoot,bTeam,func);
	}

	pLootProto = (tagCreatureLoot*)GetCreatureLoot(pCreature->GetLootID2());
	if(P_VALID(pLootProto) )
	{ 
		NormalLoot_ForLootID123(pCreature,pLootProto,nPosIndex,pRole,eLootTo,bSingelLoot,bTeam,func);
	}

	pLootProto = (tagCreatureLoot*)GetCreatureLoot(pCreature->GetLootID3());
	if(P_VALID(pLootProto) )
	{ 
		NormalLoot_ForLootID123(pCreature,pLootProto,nPosIndex,pRole,eLootTo,bSingelLoot,bTeam,func);
	}


}

//-------------------------------------------------------------------------------------------------------
// 普通掉落增加了LootID2 和 LootID3 ，拆分了原来的NormalLoot的代码  2010-07-22 ALEX
//-------------------------------------------------------------------------------------------------------

VOID LootMgr::NormalLoot_ForLootID123(Creature* pCreature,tagCreatureLoot* pLootProto ,INT &nPosIndex, Role* pRole, ELootTo eLootTo, BOOL bSingelLoot, BOOL bTeam,TLootItemEvent func)
{
	if( !P_VALID(pCreature) ) return;
	if( !P_VALID(pRole) ) return;


	// 得到物品掉落的加成率
	FLOAT fLootChanceAdd = GetCreatureLootChanceAdd(pCreature, pRole,ERA_Money_Add_Rate);

	// 金钱掉落
	DropMoneyTo(pLootProto, pCreature, eLootTo, pRole, fLootChanceAdd, nPosIndex);

	fLootChanceAdd = GetCreatureLootChanceAdd(pCreature, pRole,ERA_Loot_Add_Rate);

	if(pLootProto->dwKillCount != GT_INVALID)
	{
		// 需要计数的 累加掉落次数
		INT nCurrent = pLootProto->dwKillCount;
		while(nCurrent != InterlockedCompareExchange((LPLONG)&pLootProto->dwKillCount, nCurrent+1, nCurrent))
			nCurrent = pLootProto->dwKillCount;
	}

	// 物品掉落
	for(INT nLootIndex = 0; nLootIndex < MAX_CREATURE_LOOT_NUM; ++nLootIndex)
	{
		const tagLoot* pLoot = &(pLootProto->Loot[nLootIndex]);
		if( !P_VALID(pLoot->dwItemID) ) break;

		if(pLoot->nKill> 0)
		{
			// 计数掉落的 这里判断是否到了掉落的次数
			if(pLootProto->dwKillCount % pLoot->nKill != 0) 
				continue; 
		}
		else
		{
			if(bTeam && P_VALID(pRole) && pRole->GetTeamID() != GT_INVALID)
			{
				// 判断掉率，看是否满足
				FLOAT fRealChance = pLoot->fTeamChance * fLootChanceAdd;
				if( IUTIL->Rand() % 100000 > INT(fRealChance * 100000.0f) )
					continue;
			}
			else
			{
				// 判断掉率，看是否满足
				FLOAT fRealChance = pLoot->fChance * fLootChanceAdd;
				if( IUTIL->Rand() % 100000 > INT(fRealChance * 100000.0f) )
					continue;
			}
		}

		DWORD bRtv = FALSE;		

		// 如果是物品掉落
		if( ELT_Item == pLoot->eLootType )
		{
			bRtv = DropItemToEx(pLoot, pCreature, eLootTo, pRole, bTeam, fLootChanceAdd, nPosIndex,pLootProto->eLootMode,func);
		}

		// 如果是集合掉落
		else if( ELT_Set == pLoot->eLootType )
		{
			bRtv = DropSetToEx(pLoot, pCreature, eLootTo, pRole, bTeam, fLootChanceAdd, nPosIndex,pLootProto->eLootMode,func);
		}
		else
		{
			ASSERT(0);
			bRtv = FALSE;
		}

		if(bRtv && pLoot->bBroadcast && pLoot->eLootType == ELT_Item)
		{
			DWORD	dwMsgInfoID		=	(DWORD)g_MsgInfoMgr.BeginMsgEvent();
			DWORD dwTmpNum = 300004;
			g_MsgInfoMgr.AddMsgEvent(dwMsgInfoID, EMUT_BigBroadcast, &dwTmpNum);
			dwTmpNum = pRole->GetID();
			g_MsgInfoMgr.AddMsgEvent(dwMsgInfoID, EMUT_RoleID, &dwTmpNum);
			dwTmpNum = pCreature->GetTypeID();
			g_MsgInfoMgr.AddMsgEvent(dwMsgInfoID, EMUT_NPCTypeID, &dwTmpNum);
			dwTmpNum = pLoot->dwItemID;
			g_MsgInfoMgr.AddMsgEvent(dwMsgInfoID, EMUT_ItemTypeID, &dwTmpNum);
			g_MsgInfoMgr.DispatchWorldMsgEvent(dwMsgInfoID);
		}

		// 如果是只掉落一次，则直接返回
		if( bRtv && bSingelLoot )
			break;
	}
}





//-------------------------------------------------------------------------------------------------------
// 任务掉落
//-------------------------------------------------------------------------------------------------------
VOID LootMgr::QuestLoot(Creature* pCreature, Role* pRole, BOOL bTeam)
{
	if( !P_VALID(pCreature) ) return;
	if( !P_VALID(pRole) ) return;

	const Team* pTeam = g_groupMgr.GetTeamPtr(pRole->GetTeamID());

	// 得到任务掉落
	const tagLootQuestItem* pQuestLoot	= GetLootQuestItem(pCreature->GetTypeID());
	if( !P_VALID(pQuestLoot) ) return;

	for(INT i = 0; i < MAX_LOOT_QUEST_ITEM_NUM; ++i)
	{
		DWORD dwItemID = pQuestLoot->QuestItem[i].dwQuestItemID;

		// 如果有小队，取小队几率，否则取玩家个人几率
		FLOAT fChance = 0.0f;
		if( P_VALID(pTeam) )
		{
			fChance = pQuestLoot->QuestItem[i].fTeamChance;
		}
		else
		{
			fChance = pQuestLoot->QuestItem[i].fChance;
		}

		// 检查物品ID是否为零
		if( !P_VALID(dwItemID) ) break;

		// 计算几率
		if( IUTIL->Rand() % 100000 > INT(fChance * 100000.0f) )
			continue;

		// 找到物品
		tagItemProto* pProto = NULL;
		if( MIsEquipment(dwItemID) )
		{
			pProto = g_attRes.GetEquipProto(dwItemID);
		}
		else
		{
			pProto = g_attRes.GetItemProto(dwItemID);
		}

		if( !P_VALID(pProto) ) continue;

		// 队伍任务拾取
		if(bTeam && P_VALID(pTeam))
		{
			INT		nTeamNum	=	pTeam->GetMemNum();
			Role*	pPicker		=	NULL;

			for(INT j = 0; j < nTeamNum; ++j)
			{
				pPicker = pTeam->GetMemPtr(j);

				if( !P_VALID(pPicker) || !pCreature->IsLootShareDistance(pPicker) )
					continue;

				DropQuestItem(pPicker, pProto, pCreature->GetID());
			}
		}
		// 单人任务拾取
		else
		{
			DropQuestItem(pRole, pProto, pCreature->GetID());
		}				
	}
}

//-------------------------------------------------------------------------------------------------------
// 取得物品掉落位置
//-------------------------------------------------------------------------------------------------------
INT LootMgr::GetPutDownPos(Creature* pCreature, Vector3 &vPos, INT &nIndex)
{
	ASSERT(P_VALID(pCreature));

	MTRANS_ELSE_RET(pMap, pCreature->GetMap(), Map, 0);

	for( ; nIndex < MAX_LOOT_POS; ++nIndex)
		if (pMap->CanPutDown(pCreature, nIndex, vPos))
			return TRUE;

	return FALSE;
}

//-------------------------------------------------------------------------------------------------------
// 得到生物掉率加成
//-------------------------------------------------------------------------------------------------------
FLOAT LootMgr::GetCreatureLootChanceAdd(Creature* pCreature, Role* pReward,INT addType)
{
	if( !P_VALID(pCreature) || !P_VALID(pReward) ) return 1.0f;

	if( ECTT_All == pCreature->GetTaggedType() ) return 1.0f;

	// 如果是资源
	if( pCreature->IsRes() )
	{
		return GetResLootChanceAdd(pCreature, pReward);
	}
	else if( pCreature->IsInves() )
	{
		return GetInvesLootChanceAdd(pReward);
	}
	else
	{
		return GetMosterLootChanceAddEx(pCreature,pReward,addType);//GetMosterLootChanceAdd(pReward);
	}
}

//-------------------------------------------------------------------------------------------------------
// 怪物掉率加成
//-------------------------------------------------------------------------------------------------------
FLOAT LootMgr::GetMosterLootChanceAdd(Role* pReward)
{
	if( !P_VALID(pReward) ) return 1.0f;

	FLOAT	fGMLootRate		=	pReward->GetMap()->GetLootRate();				// 全服双倍，0-5 倍数
	INT		nFatigueRate	=	pReward->GetEarnRate();							// 防沉迷， 0-10000映射为0-1的分数 倍数
	INT		nAddOn			=	pReward->GetAttValue(ERA_Loot_Add_Rate);		// 自身掉率加成
	FLOAT	fVNBLootAdd		=	pReward->GetVNBLootRate();
	FLOAT	fSpiRate		=	pReward->GetSpiRate();							// 精力值加成

	return fGMLootRate * (FLOAT(nFatigueRate) / 10000.0f) * (1.0f + FLOAT(nAddOn) / 10000.0f + fVNBLootAdd) * fSpiRate;
}
FLOAT LootMgr::GetMosterLootChanceAddEx(Creature* pCreature,Role* pReward,INT addType)
{
	if( !P_VALID(pReward) || !P_VALID(pCreature) ) return 1.0f;

	FLOAT decreaseFactor = 1.0;

	const tagCreatureProto *pProto = pCreature->GetProto();
	if( !P_VALID(pProto) ) return 1.0f;

	if( ECILAT_None != pProto->eItemLootAttenuType && ECTT_All != pCreature->GetTaggedType() && !pReward->IsHaveBuff(61096))	//副本体验buff的也不衰减
	{
		INT nRoleLevel = pReward->GetLevel  ();
		DWORD dwTeamID = pReward->GetTeamID  ();
		if( GT_VALID( dwTeamID ) )
		{
			const Team * pTeam = g_groupMgr.GetTeamPtr  (dwTeamID);
			if( P_VALID(pTeam) )
			{
				int count = pTeam->GetMemNum();
				Role * p = NULL;
				for(int i = 0; i < count ; ++i)
				{
					p = pTeam->GetMemPtr  (i);
					if( P_VALID(p) && p != pReward )
						nRoleLevel += p->GetLevel  ();
				}
				if( count > 0 )
					nRoleLevel /= count;
			}
		}
		INT nCreatureLevel = pCreature->GetLevel  ();
		INT delta = nRoleLevel - nCreatureLevel;

		if( delta <= 9 )
			decreaseFactor = 1.0;
		else if( delta <= 14 )
			decreaseFactor = 0.8;
		else if( delta <= 19 )
			decreaseFactor = 0.5;
		else if( delta <= 24 )
			decreaseFactor = 0.2;
		else if( delta <= 29 )
			decreaseFactor = 0.1;
		else
			decreaseFactor = 0;
	}

	FLOAT	fGMLootRate		=	pReward->GetMap()->GetLootRate();				// 全服双倍，0-5 倍数
	INT		nFatigueRate	=	pReward->GetEarnRate();							// 防沉迷， 0-10000映射为0-1的分数 倍数
	INT		nAddOn			=	pReward->GetAttValue(addType);		// 自身掉率加成
	FLOAT	fVNBLootAdd		=	pReward->GetVNBLootRate();
	FLOAT	fSpiRate		=	pReward->GetSpiRate();							// 精力值加成

	return fGMLootRate * (FLOAT(nFatigueRate) / 10000.0f) * (1.0f + FLOAT(nAddOn) / 10000.0f + fVNBLootAdd) * decreaseFactor * fSpiRate;
}

//-------------------------------------------------------------------------------------------------------
// 资源掉率加成
//-------------------------------------------------------------------------------------------------------
FLOAT LootMgr::GetResLootChanceAdd(Creature* pCreature, Role* pReward)
{
	if( !P_VALID(pCreature) || !P_VALID(pReward) ) return 1.0f;

	INT		nFatigueRate	=	pReward->GetEarnRate();							// 防沉迷， 0-10000映射为0-1的分数 倍数
	INT		nAddOn			=	pReward->CalGatherRate(pCreature);				// 采集加成

	return (FLOAT(nFatigueRate) / 10000.0f) * (1.0f + FLOAT(nAddOn) / 10000.0f);
}

//-------------------------------------------------------------------------------------------------------
// 可调查地物掉率加成
//-------------------------------------------------------------------------------------------------------
FLOAT LootMgr::GetInvesLootChanceAdd(Role* pReward)
{
	return 1.0f;
}

//----------------------------------------------------------------------------------------
// 得到某个掉落的所属
//----------------------------------------------------------------------------------------
VOID LootMgr::GetLootOwner(Creature* pCreature, Role* pPicker, BOOL bTeam, DWORD& dwOwnerID, Role* &pOwnerRole, DWORD& dwTeamID)
{
	if( pCreature->IsTaggedOnly() )
	{
		pOwnerRole = pCreature->GetTaggedOwnerRole();
		dwOwnerID = GT_INVALID;
		if( P_VALID(pOwnerRole) )
			dwOwnerID = pOwnerRole->GetID();
		dwTeamID = GT_INVALID;
		return;
	}
	
	// 全体所属
	if( ECTT_All == pCreature->GetTaggedType() )
	{
		dwTeamID	=	GT_INVALID;
		dwOwnerID	=	GT_INVALID;
		pOwnerRole	=	NULL;
	}
	// 非全体所属
	else
	{
		if( bTeam )
		{
			dwTeamID = pPicker->GetTeamID();

			if( P_VALID(dwTeamID) )
			{
				const Team* pTeam = g_groupMgr.GetTeamPtr(dwTeamID);
				if( P_VALID(pTeam) )
				{
					pOwnerRole = pTeam->GetTeamPickRole(pCreature);
				}
				else
				{
					pOwnerRole = NULL;
				}

				if( P_VALID(pOwnerRole) )
				{
					dwOwnerID = pOwnerRole->GetID();
				}
				else
				{
					dwOwnerID = GT_INVALID;
				}
			}
			else
			{
				pOwnerRole = pPicker;
				dwOwnerID = pPicker->GetID();
			}
		}
		else
		{
			dwTeamID	=	GT_INVALID;
			dwOwnerID	=	pPicker->GetID();
			pOwnerRole	=	pPicker;
		}
	}
}

//-----------------------------------------------------------------------------------------
// 各种掉落方式
//-----------------------------------------------------------------------------------------
BOOL LootMgr::LootItem(Creature* pCreature, tagItem* &pItem, ELootTo eLootTo, Role* pOwnerRole, DWORD dwOwnerID, DWORD dwTeamID, INT& nIndex, ELootMode lootMode)
{
	if( !P_VALID(pCreature) || !P_VALID(pItem) ) return FALSE;

	// 得到地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	if( ELM_LootToBag == lootMode )
	{
		if( !P_VALID(pOwnerRole) || E_Success != pOwnerRole->GetItemMgr().Add2Bag(pItem, ELCID_Loot, TRUE) )
		{
			::Destroy(pItem);
			return FALSE;
		}
		Team::BroadcastLootItemMsgInTeamExceptPicker(pOwnerRole,pItem);
		return TRUE;
	}

	// 根据掉落方式
	switch(eLootTo)
	{
		// 强制掉落背包
	case ELT_ForceBag:
		{
			if( !P_VALID(pOwnerRole) || E_Success != pOwnerRole->GetItemMgr().Add2Bag(pItem, ELCID_Loot, TRUE) )
			{
				::Destroy(pItem);
				return FALSE;
			}
			// Jason 2009-11-28 判断物品等级，并广播到战斗频道中
			//BOOL canbroadcast = FALSE;
			//if( MIsEquipment(pItem->dwTypeID) )
			//{
			//	if( pItem->pEquipProto->byQuality != EIQ_White )
			//		canbroadcast = TRUE;
			//}
			//else if( pItem->pProtoType->byQuality != EIQ_White )
			//{
			//	canbroadcast = TRUE;
			//}
			//if(canbroadcast)
				Team::BroadcastLootItemMsgInTeamExceptPicker(pOwnerRole,pItem);
			return TRUE;
		}
		break;

		// 先背包
	case ELT_BagFirst:
		{
			if( P_VALID(pOwnerRole) )
			{
				if( E_Success == pOwnerRole->GetItemMgr().Add2Bag(pItem, ELCID_Loot, TRUE) )
				{
					// Jason 2009-11-28 判断物品等级，并广播到战斗频道中
					//BOOL canbroadcast = FALSE;
					//if( MIsEquipment(pItem->dwTypeID) )
					//{
					//	if( pItem->pEquipProto->byQuality != EIQ_White )
					//		canbroadcast = TRUE;
					//}
					//else if( pItem->pProtoType->byQuality != EIQ_White )
					//{
					//	canbroadcast = TRUE;
					//}
					//if(canbroadcast)
						Team::BroadcastLootItemMsgInTeamExceptPicker(pOwnerRole,pItem);

					return TRUE;
				}
				else
				{
					Vector3 vPos;
					if( !GetPutDownPos(pCreature, vPos, nIndex) )
					{
						::Destroy(pItem);
						return FALSE;
					}
					pMap->PutDownItem(pCreature, pItem, dwOwnerID, dwTeamID, vPos);

					return TRUE;
				}
			}
			else
			{
				Vector3 vPos;
				if( !GetPutDownPos(pCreature, vPos, nIndex) )
				{
					::Destroy(pItem);
					return FALSE;
				}
				pMap->PutDownItem(pCreature, pItem, dwOwnerID, dwTeamID, vPos);

				return TRUE;
			}
		}
		break;

		// 掉落地面
	case ELT_Ground:
		{
			Vector3 vPos;
			if( !GetPutDownPos(pCreature, vPos, nIndex) )
			{
				::Destroy(pItem);
				return FALSE;
			}
			pMap->PutDownItem(pCreature, pItem, dwOwnerID, dwTeamID, vPos);
			return TRUE;
		}
		break;

		// 默认
	default:
		{
			ASSERT(0);
			return FALSE;
		}
		break;
	}
}

//-------------------------------------------------------------------------------------------------------
// 金钱掉落
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::DropMoneyTo( const tagCreatureLoot* pLootProto, Creature* pCreature, ELootTo eLootTo, Role* pPicker, FLOAT fChanceAdd, INT &nIndex )
{
	if( !P_VALID(pLootProto) ) return FALSE;
	if( !P_VALID(pCreature) ) return FALSE;

	// 得到怪物所在地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	// 得到实际掉落的钱数
	INT nMoney = INT(IUTIL->RandomInRange(pLootProto->nMinMoney, pLootProto->nMaxMoney) * fChanceAdd);
	if( nMoney <= 0 ) return FALSE;

	//2007-07-21 Alex: 根据游戏币掉落的堆数，平均散落掉落游戏币  

	DWORD dwGroupNum=pLootProto->dw_num;
	if(dwGroupNum<=0){dwGroupNum=1;}

	INT nAvgMoney=floor((double)nMoney/dwGroupNum);
	
	for(INT i=0;i<dwGroupNum;i++){ 

		// 得到掉落位置
		Vector3 vPos(0.0f, 0.0f, 0.0f);
		BOOL bHasPositon = GetPutDownPos(pCreature, vPos, nIndex);
		if( !bHasPositon ) return FALSE;

		pMap->PutDownMoney(pCreature, nAvgMoney, pPicker, vPos);
		++nIndex;
	}
   
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 集合掉落
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::DropSetTo(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, BOOL bTeam, FLOAT fChanceAdd, INT &nIndex, ELootMode lootMode)
{
	if( !P_VALID(pLoot) ) return FALSE;
	if( !P_VALID(pCreature) ) return FALSE;
	if( !P_VALID(pPicker) ) return FALSE;

	if( ELT_Set != pLoot->eLootType ) return FALSE;

	// 得到怪物地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	// 如果已经掉满了，则不掉落了
	if( nIndex >= MAX_LOOT_POS ) return FALSE;

	// 得到掉落集合
	const tagLootItemSet* pItemSet = GetLootItemSet(pLoot->dwItemID);
	if( !P_VALID(pItemSet) ) return FALSE;

	// 掉落几件
	INT nDropNum = IUTIL->RandomInRange(pLoot->nMin, pLoot->nMax);

	// 集合中有效物品
	TList<INT> listValid;
	for(INT i = 0; i < MAX_ITEM_SET_NUM; ++i)
	{
		if( 0 == pItemSet->ItemSet[i].dwItemID || pItemSet->ItemSet[i].nItemNum <= 0)
			continue;

		listValid.PushBack(i);
	}
	if( listValid.Size() <= 0 )
		return FALSE;

	// 对于每一项
	for(INT i = 0; i < nDropNum && nIndex < MAX_LOOT_POS; ++i)
	{
		INT nSetIndex = GT_INVALID;	// 获得索引
		listValid.RandPeek(nSetIndex);

		// 得到物品属性
		tagItemProto* pProto = NULL;
		if( MIsEquipment(pItemSet->ItemSet[nSetIndex].dwItemID) )
		{
			pProto = g_attRes.GetEquipProto(pItemSet->ItemSet[nSetIndex].dwItemID);
		}
		else
		{
			pProto = g_attRes.GetItemProto(pItemSet->ItemSet[nSetIndex].dwItemID);
		}
		if( !P_VALID(pProto) ) continue;
		if(IsShieldingItem(pItemSet->ItemSet[nSetIndex].dwItemID) )
			continue;

		// 生成物品
		tagItem* pItem = ItemCreator::Create(EICM_Loot, pCreature->GetID(), pProto->dwTypeID, pItemSet->ItemSet[nSetIndex].nItemNum);
		if( !P_VALID(pItem) ) continue;

		// 找到本次掉落的所属
		DWORD dwTeamID	= GT_INVALID;
		Role* pOwnerRole = NULL;
		DWORD dwOwnerID = GT_INVALID;
		GetLootOwner(pCreature, pPicker, bTeam, dwOwnerID, pOwnerRole, dwTeamID);

		// 掉落物品
		if( LootItem(pCreature, pItem, eLootTo, pOwnerRole, dwOwnerID, dwTeamID, nIndex,lootMode) )
		{
			++nIndex;
		}
	}

	return TRUE;
}

BOOL	LootMgr::DropSetToEx(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, 
									BOOL bTeam, FLOAT fChanceAdd, INT &nIndex, ELootMode lootMode,TLootItemEvent func)
{
	if( !P_VALID(pLoot) ) return FALSE;
	if( !P_VALID(pCreature) ) return FALSE;
	if( !P_VALID(pPicker) ) return FALSE;

	if( ELT_Set != pLoot->eLootType ) return FALSE;

	// 得到怪物地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	// 如果已经掉满了，则不掉落了
	if( nIndex >= MAX_LOOT_POS ) return FALSE;

	// 得到掉落集合
	const tagLootItemSet* pItemSet = GetLootItemSet(pLoot->dwItemID);
	if( !P_VALID(pItemSet) ) return FALSE;

	// 掉落几件
	INT nDropNum = IUTIL->RandomInRange(pLoot->nMin, pLoot->nMax);

	// 集合中有效物品
	TList<INT> listValid;
	for(INT i = 0; i < MAX_ITEM_SET_NUM; ++i)
	{
		if( 0 == pItemSet->ItemSet[i].dwItemID || pItemSet->ItemSet[i].nItemNum <= 0)
			continue;

		listValid.PushBack(i);
	}
	if( listValid.Size() <= 0 )
		return FALSE;

	// 对于每一项
	for(INT i = 0; i < nDropNum && nIndex < MAX_LOOT_POS; ++i)
	{
		INT nSetIndex = GT_INVALID;	// 获得索引
		listValid.RandPeek(nSetIndex);

		// 得到物品属性
		tagItemProto* pProto = NULL;
		if( MIsEquipment(pItemSet->ItemSet[nSetIndex].dwItemID) )
		{
			pProto = g_attRes.GetEquipProto(pItemSet->ItemSet[nSetIndex].dwItemID);
		}
		else
		{
			pProto = g_attRes.GetItemProto(pItemSet->ItemSet[nSetIndex].dwItemID);
		}
		if( !P_VALID(pProto) ) continue;
		if(IsShieldingItem(pItemSet->ItemSet[nSetIndex].dwItemID) )
			continue;

		const WorldScript* pScript = g_ScriptMgr.GetWorldScript();
		if (P_VALID(pScript))
		{
			if(!pScript->OnLoot(pCreature->GetID(), pPicker->GetID(), pLoot->eLootType, pProto->dwTypeID, pLoot->dwItemID))
				continue;
		}

		// 生成物品
		tagItem* pItem = ItemCreator::Create(EICM_Loot, pCreature->GetID(), pProto->dwTypeID, pItemSet->ItemSet[nSetIndex].nItemNum);
		if( !P_VALID(pItem) ) continue;

		// 找到本次掉落的所属
		DWORD dwTeamID	= GT_INVALID;
		Role* pOwnerRole = NULL;
		DWORD dwOwnerID = GT_INVALID;
		GetLootOwner(pCreature, pPicker, bTeam, dwOwnerID, pOwnerRole, dwTeamID);

		// 掉落物品
		if( LootItem(pCreature, pItem, eLootTo, pOwnerRole, dwOwnerID, dwTeamID, nIndex,lootMode) )
		{
			++nIndex;
			if( P_VALID(func) )
				func(pOwnerRole,pCreature,pItem);
		}	
	}

	return TRUE;
}
//-------------------------------------------------------------------------------------------------------
// 物品掉落
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::DropItemTo(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, BOOL bTeam, FLOAT fChanceAdd, INT &nIndex, ELootMode lootMode)
{
	if( !P_VALID(pLoot) || !P_VALID(pCreature) || !P_VALID(pPicker) )
		return FALSE;

	if( ELT_Item != pLoot->eLootType ) return FALSE;

	// 得到怪物地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	// 如果已经掉满了，则不掉落了
	if( nIndex >= MAX_LOOT_POS ) return FALSE;

	// 得到物品属性
	tagItemProto* pProto = NULL;
	if( MIsEquipment(pLoot->dwItemID) )
	{
		pProto = g_attRes.GetEquipProto(pLoot->dwItemID);
	}
	else
	{
		pProto = g_attRes.GetItemProto(pLoot->dwItemID);
	}
	if( !P_VALID(pProto) ) return FALSE;
	
	// 得到物品数目
	if(IsShieldingItem(pLoot->dwItemID) )
		return FALSE;
	INT16 n16RandNum = IUTIL->RandomInRange(pLoot->nMin, pLoot->nMax);
	if( n16RandNum <= 0 ) return FALSE;

	// 生成物品
	tagItem* pItem = ItemCreator::Create(EICM_Loot, pCreature->GetID(), pProto->dwTypeID, n16RandNum);
	if( !P_VALID(pItem) ) return FALSE;

	// 找到本次掉落的所属
	DWORD dwTeamID	= GT_INVALID;
	Role* pOwnerRole = NULL;
	DWORD dwOwnerID = GT_INVALID;

	GetLootOwner(pCreature, pPicker, bTeam, dwOwnerID, pOwnerRole, dwTeamID);

	// 掉落物品
	BOOL bRet = LootItem(pCreature, pItem, eLootTo, pOwnerRole, dwOwnerID, dwTeamID, nIndex, lootMode);

	if( bRet ) ++nIndex;

	return bRet;
}
BOOL	LootMgr::DropItemToEx(const tagLoot* pLoot, Creature* pCreature, ELootTo eLootTo, Role* pPicker, 
							 BOOL bTeam, FLOAT fChanceAdd, INT &nIndex, ELootMode lootMode,TLootItemEvent func)
{
	if( !P_VALID(pLoot) || !P_VALID(pCreature) || !P_VALID(pPicker) )
		return FALSE;

	if( ELT_Item != pLoot->eLootType ) return FALSE;

	// 得到怪物地图
	Map* pMap = pCreature->GetMap();
	if( !P_VALID(pMap) ) return FALSE;

	// 如果已经掉满了，则不掉落了
	if( nIndex >= MAX_LOOT_POS ) return FALSE;

	const WorldScript* pScript = g_ScriptMgr.GetWorldScript();
	if (P_VALID(pScript))
	{
		if(!pScript->OnLoot(pCreature->GetID(), pPicker->GetID(), pLoot->eLootType, pLoot->dwItemID))
			return FALSE;
	}

	// 得到物品属性
	if(IsShieldingItem(pLoot->dwItemID) )
		return FALSE;
	tagItemProto* pProto = NULL;
	if( MIsEquipment(pLoot->dwItemID) )
	{
		pProto = g_attRes.GetEquipProto(pLoot->dwItemID);
	}
	else
	{
		pProto = g_attRes.GetItemProto(pLoot->dwItemID);
	}
	if( !P_VALID(pProto) ) return FALSE;

	// 稀有物品在世界中的个数已经达到了最大值，则不掉了
	if (EISF_GlobalLimitedItem == pProto->eSpecFunc)
	{
		if (g_GlobalLimitedItemMgr.IsLimitedItemFull(pLoot->dwItemID))
		{
			return FALSE;
		}
	}

	// 得到物品数目
	INT16 n16RandNum = IUTIL->RandomInRange(pLoot->nMin, pLoot->nMax);
	if( n16RandNum <= 0 ) return FALSE;

	// 生成物品
	tagItem* pItem = ItemCreator::Create(EICM_Loot, pCreature->GetID(), pProto->dwTypeID, n16RandNum);
	if( !P_VALID(pItem) ) return FALSE;

	// 找到本次掉落的所属
	DWORD dwTeamID	= GT_INVALID;
	Role* pOwnerRole = NULL;
	DWORD dwOwnerID = GT_INVALID;

	GetLootOwner(pCreature, pPicker, bTeam, dwOwnerID, pOwnerRole, dwTeamID);

	// 掉落物品
	BOOL bRet = LootItem(pCreature, pItem, eLootTo, pOwnerRole, dwOwnerID, dwTeamID, nIndex, lootMode);

	if( bRet )
	{
		++nIndex;
		if( P_VALID(func) )
			func(pPicker,pCreature,pItem);
	}

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 任务物品掉落
//-------------------------------------------------------------------------------------------------------
BOOL LootMgr::DropQuestItem(Role* pPicker, const tagItemProto* pItemProto, DWORD dwCreatureID)
{
	if( !P_VALID(pPicker) ) return FALSE;
	if( !P_VALID(pItemProto) ) return FALSE;

	// 找到该物品对应的任务
	UINT16	u16QuestID = (UINT16)pItemProto->dwQuestID;

	// 玩家当前任务是否有该任务
	Quest* pQuest = pPicker->GetQuest(u16QuestID);
	if( !P_VALID(pQuest) ) return FALSE;

	// 检测该任务的任务物品是否已经满了
	if( pQuest->IsQuestItemFull(pItemProto->dwTypeID) ) return FALSE;

	// 生成任务物品
	tagItem* pItem = ItemCreator::Create(EICM_Loot, dwCreatureID, pItemProto->dwTypeID, 1);
	if( !P_VALID(pItem) ) return FALSE;

	// 在任务背包里面加入
	if( pPicker->GetItemMgr().Add2QuestBag(pItem, ELCID_Loot) )
	{
		::Destroy(pItem);
		return FALSE;
	}

	return TRUE;
}

VOID LootMgr::Update()
{
	CHECK_TIME()
	if(m_dwReloadFlag)
	{
		if(m_dwReloadFlag & 0x01)
		{
			m_mapCreatureLoot.ResetIterator();
			tagCreatureLoot* pCreatureLoot = NULL;
			while (m_mapCreatureLoot.PeekNext(pCreatureLoot))
			{
				SAFE_DEL(pCreatureLoot);
			}
			m_mapCreatureLoot.Clear();
			LoadCreatureLoot();			
		}
		if(m_dwReloadFlag & 0x02)
		{
			m_mapLootItemSet.ResetIterator();
			tagLootItemSet* pLootItemSet = NULL;
			while (m_mapLootItemSet.PeekNext(pLootItemSet))
			{
				SAFE_DEL(pLootItemSet);
			}
			m_mapLootItemSet.Clear();
			LoadItemSetLoot();
		}
		if(m_dwReloadFlag & 0x04)
		{
			m_mapLootQuestItem.ResetIterator();
			tagLootQuestItem* pLootQuestItem = NULL;
			while (m_mapLootQuestItem.PeekNext(pLootQuestItem))
			{
				SAFE_DEL(pLootQuestItem);
			}
			m_mapLootQuestItem.Clear();
			LoadQuestItemLoot();
		}
		InterlockedExchange((LPLONG)&m_dwReloadFlag, 0);
	}
}

void LootMgr::RegisterCmd()
{
	TObjRef<Console> pConsole = "Console";
	pConsole->Register(_T("reloadloot"), m_Trunk.sfp1(&LootMgr::Reload), _T("reloadloot 1/2/3"), 1);
	pConsole->Register(_T("setlootkillcount"), m_Trunk.sfp1(&LootMgr::SetLootKillCount), _T("setlootkillcount lootid value"), 2);
}

DWORD LootMgr::Reload(LPCTSTR pszWhich)
{
	if(!P_VALID(pszWhich)) return m_dwReloadFlag;
	DWORD flag = (DWORD)_wtoi(pszWhich);
	if(flag>0 && flag <=3)
	{
		flag = m_dwReloadFlag  | (1 << (flag-1));
		InterlockedExchange((LPLONG)&m_dwReloadFlag, flag);
	}
	return m_dwReloadFlag;
}

DWORD LootMgr::SetLootKillCount(LPCTSTR pszWhich, LPCTSTR pszValue)
{
	if(!P_VALID(pszWhich) || !P_VALID(pszValue)) return m_dwReloadFlag;
	tagCreatureLoot* pCreatureLoot = m_mapCreatureLoot.Peek(_wtoi(pszWhich));
	DWORD value	 = (DWORD)_wtoi(pszValue);
	if(P_VALID(pCreatureLoot))
	{
		InterlockedExchange((LPLONG)&pCreatureLoot->dwKillCount, value);
		return pCreatureLoot->dwKillCount;
	}
	return GT_INVALID;
}
