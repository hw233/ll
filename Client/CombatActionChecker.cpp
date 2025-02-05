#include "StdAfx.h"
#include "CombatActionChecker.h"
#include "RoleMgr.h"
#include "Player.h"
#include "NPC.h"
#include "Door.h"
#include "LocalPlayer.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\skill_define.h"
#include "..\WorldDefine\role_att.h"
#include "SkillMgr.h"
#include "ItemProtoData.h"
#include "MapMgr.h"
#include "TeamSys.h"
#include "ActivityPvPMgr.h"
#include "..\WorldDefine\msg_motion.h"
#include "SocialMgr.h"
#include "LocalPlayer.h"
#include "PetManager.h"
#include "GuildMgr.h"
#include "..\WorldDefine\pk_define.h"
#include "MapLogicData.h"
#include "SkillProtoData.h"
#include "HolyManMgr.h"

const DWORD FriendPlayerMask=ETF_Self|ETF_Teammate|ETF_Guildmate|ETF_Brother|ETF_Teacher|ETF_Lover|ETF_Couple;
const DWORD Rider_Skill_ID = 9000101;
CombatActionChecker::CombatActionChecker(void)
{
}

CombatActionChecker::~CombatActionChecker(void)
{
}

CombatActionChecker g_combatActionChecker;
CombatActionChecker* CombatActionChecker::Inst()
{
	return &g_combatActionChecker;
}

void CombatActionChecker::Init()
{

}

void CombatActionChecker::Destroy()
{

}

bool CombatActionChecker::IfCanUseSkill( DWORD skillID,DWORD targetID,bool bShowErrMsg )
{
	const tagSkillProtoClient *pSkillProto2 = SkillProtoData::Inst()->FindSkillProto(skillID);
	if (!P_VALID(pSkillProto2) )	return false;
	
	const tagSkillData* pSkillData=SkillMgr::Inst()->GetSkillDataByTypeID(skillID);
	if(!P_VALID(pSkillData) &&  pSkillProto2->nType2 != ESSTE_SoulSubSkill )
		return false;

	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//--摆摊状态下不能使用技能
	const DWORD RoleStateMask=ERS_Stall;
	if(pSelf->GetRoleState()&RoleStateMask)
		return false;

	const tagSkillProtoClient* pClientProto = NULL;
	if(pSkillProto2->nType2 == ESSTE_SoulSubSkill)
		pClientProto = pSkillProto2;
	else
		pClientProto = pSkillData->pProto;

	//--被动技能不能使用
	if(pClientProto->eUseType==ESUT_Passive)
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_Passive);
		return false;
	}

	// -- 非变身状态下不能使用变身技能
	if ( !RoleMgr::Inst()->IsTransforming(pSelf) && pClientProto->nType2 == ESSTE_Transform)
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Cannotuse_TransSkill")]);
		return false;
	}

	// --职业限制
	if (  !(pClientProto->eClassLimit & ( 1 << (pSelf->GetPlayerClass() - 1) )))
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillSvrErr22")]);
		return false;
	}

	// -- 地图限制的技能
	if (ESSF_SkillMap == pClientProto->eSpecFunc)
	{
		if (1 ==pClientProto->nSpecFuncVal1)	// 普通地图
		{
			DWORD dwMapId = MapMgr::Inst()->GetCurMapID();
			const tagMapLogicData *mapLogicData = MapLogicData::Inst()->FindMapLogicData(dwMapId);
			if (P_VALID(mapLogicData) && mapLogicData->eMapType != EMT_Normal)
			{
				if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Cannotuse_MapNotRight")]);
				return false;
			}
		}
		else if (2 ==pClientProto->nSpecFuncVal1)	// 副本地图
		{
			DWORD dwMapId = MapMgr::Inst()->GetCurMapID();
			const tagMapLogicData *mapLogicData = MapLogicData::Inst()->FindMapLogicData(dwMapId);
			if (P_VALID(mapLogicData) && mapLogicData->eMapType == EMT_Normal)
			{
				if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Cannotuse_MapNotRight")]);
				return false;
			}
		}
		else if (3 ==pClientProto->nSpecFuncVal1)	// 读表索引地图
		{
			DWORD dwMapId = MapMgr::Inst()->GetCurMapID();
			const tagSkillableMaps* pData = SkillProtoData::Inst()->FindSkillMapProto(pClientProto->nSpecFuncVal2);
			if (P_VALID(pData))
			{
				bool bFound = false;
				for (int i=0;i < MAX_SKILL_MAP; i++)
				{
					if(dwMapId == pData->dwMapID[i])
						bFound = true;
				}
				if(!bFound)
				{
					if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("Cannotuse_MapNotRight")]);
					return false;
				}
			}
		}
	}
    //如果已有骑乘类型buff，则先debuff，不使用技能
    if (pClientProto->nType2 == ESSTE_PetSkill)
    {
        TList<tagRoleBuff*>& buffList = RoleMgr::Inst()->GetLocalPlayer()->GetBuffList();
        tagRoleBuff* pRoleBuff = NULL;
        buffList.ResetIterator();
        while(buffList.PeekNext(pRoleBuff))
        {
            for (int i = 0; i < MAX_BUFF_PER_SKILL; ++i)
            {
                if (P_VALID(pRoleBuff) && pClientProto->dwBuffID[i] == pRoleBuff->dwBuffTypeID)
                {
                    tagNC_CancelBuff cmd;
                    cmd.dwBuffTypeID = pRoleBuff->dwBuffTypeID;
                    TObjRef<NetSession>()->Send( &cmd );

                    return false;
                }

            }
        }
    }

	//如果为骑乘技能，检查是否有乘骑，若没有则不能使用技能
	if ( pClientProto->dwID == Rider_Skill_ID )
	{
		 map<DWORD, tagPet*>::const_iterator iterPet = PetManager::Inst()->GetPetList()->begin(); 
		 for (;iterPet!=PetManager::Inst()->GetPetList()->end();iterPet++)
		 {
			 if (PetManager::Inst()->IsPetPreparing(iterPet->first))
			 {
				 break;
			 }
		 }
		 //检查是否有乘骑,到end则表明无骑乘就绪
		 if (iterPet==PetManager::Inst()->GetPetList()->end()) 
		 {
			 CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("No_Rider_Is_prepare")]);
			 return FALSE;
		 }
	}

	//--检查自身状态
	const DWORD dwSelfStateFlag=GetRoleStateFlag(pSelf);
	if( (dwSelfStateFlag&pClientProto->dwSelfStateLimit)!=dwSelfStateFlag  )
	{
		if(bShowErrMsg)
		{
			PlayerStateLimitMseeage( dwSelfStateFlag );
		}
		return false;
	}

	if(pSelf->IsSwimState())
	{
		if(bShowErrMsg)
		{
			if(pSelf->GetRoleState(ERS_Mount))
				CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_SelfStateLimit);
			else
				CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_SwimLimit);
		}
		return false;
	}

	//--性别检查
	if ( pClientProto->eSexLimit == ESSL_Man && RoleMgr::Inst()->GetLocalPlayer()->GetPlayerSex() == 0)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ManLim")]);
		return false;
	}
	if ( pClientProto->eSexLimit == ESSL_Woman && RoleMgr::Inst()->GetLocalPlayer()->GetPlayerSex() == 1)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_WomenLim")]);
		return false;
	}

	//--武器检查
	if(pClientProto->nWeaponLimit!=0
		&&pClientProto->nWeaponLimit!=pSelf->GetRWeaponType()
		&&pClientProto->nWeaponLimit!=pSelf->GetLWeaponType())
	{
		if(bShowErrMsg)
		{
			WeaponLimitMessage(pClientProto->nWeaponLimit);
		}
		return false;
	}

	//--前置BUFF
	if(P_VALID(pClientProto->dwBuffLimitID)
		&&!P_VALID(pSelf->FindBuff(pClientProto->dwBuffLimitID)))
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_BuffLimit")]);
		return false;
	}

	//--天神下凡技能检测
	if( skillID/100 == 10113 || 
		skillID/100 == 10114 ||
		skillID/100 == 10115 ||
		skillID/100 == 10116 ||
		skillID/100== 10117 )
	{
		if (pSelf->IsCrazyState())
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_InCrasyState")]);
			return false;
		}
		
	}

	//--消耗检测
	if(pClientProto->nSkillCost[ESCT_HP]>0
		&&pSelf->GetAttribute(ERA_HP)< GetCostByType( pSelf,pClientProto,ESCT_HP ) )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_HPLim")]);
		return false;
	}
	if(pClientProto->nSkillCost[ESCT_MP]>0
		&&pSelf->GetAttribute(ERA_MP)< GetCostByType( pSelf,pClientProto,ESCT_MP ) )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_MPLim")]);
		return false;
	}	
	if(pClientProto->nSkillCost[ESCT_Rage]>0
		&&pSelf->GetAttribute(ERA_Rage)< GetCostByType( pSelf,pClientProto,ESCT_Rage ) )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_RageLim")]);
		return false;
	}
	if(pClientProto->nSkillCost[ESCT_Endurance]>0
		&&pSelf->GetAttribute(ERA_Endurance)< GetCostByType( pSelf,pClientProto,ESCT_Endurance ) )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_EnduranceLim")]);
		return false;
	}
	if(pClientProto->nSkillCost[ESCT_Valicity]>0
		&&pSelf->GetAttribute(ERA_Vitality)< GetCostByType( pSelf,pClientProto,ESCT_Valicity ) )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_VitalityLim")]);
		return false;
	}

	//--技能指向类型检测
	if(pClientProto->eOPType==ESOPT_Explode
		&&pClientProto->fOPDist==0)//以自已为中心的爆炸技能?
	{

	}
	else
	{
		Role* pTarget=RoleMgr::Inst()->FindRole(targetID);
		if(pTarget==NULL)
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_TargetNotExist);
			return false;
		}

		//--目标类型检查
		DWORD targetType=GetTargetType(pTarget);
		if( (pClientProto->dwTargetLimit&targetType) == 0)
		{
			if(bShowErrMsg)
			{
				TargetTypeLimitMessage( targetType );
			}
			return false;
		}
		//--敌我判断(NPC优先判断)
		if( !pClientProto->bIndependent&& IsIndependent(pTarget,targetType))
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_Cantindependent")]);
			return false;
		}
		if( !pClientProto->bFriendly&& IsFriend(pTarget,targetType))
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_CantFriend")]);
			return false;
		}
		if( !pClientProto->bHostile&& IsEnemy(pTarget,targetType))
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_Canthostile")]);
			return false;
		}
		//--目标状态检查
		const DWORD dwTargetStateFlag=GetRoleStateFlag(pTarget);
		if( (dwTargetStateFlag&pClientProto->dwTargetStateLimit)!=dwTargetStateFlag  )
		{
			if(bShowErrMsg)
			{
				TargetStateLimitMessage( dwTargetStateFlag );
			}
			return false;
		}

		//--PK相关
		//自身处于保护模式时，不能对PVP模式的玩家使用技能(在战场外)
		if(pTarget!=pSelf
			&& pTarget->IS_KIND_OF(Player))
		{
			Player* pPlayer=(Player*)pTarget;
			bool bIsArena = IsInArena(pPlayer);
			BOOL bInPvP = pSelf->IsInState(ES_PvPAttack) || pSelf->IsInState(ES_PvPDefence);
			bInPvP |= pPlayer->IsInState(ES_PvPAttack) || pPlayer->IsInState(ES_PvPDefence);
			if( (pSelf->GetRoleState()&ERS_Safeguard)!=0
				&& (pPlayer->GetRoleState()&ERS_PVP)!=0 && !bIsArena && ! bInPvP)
			{
				if(bShowErrMsg)
					CombatSysUtil::Inst()->ShowActionCheckMsg((DWORD)EEC_PVP);
				return false;
			}
		}

		//--目标前置BUFF
		if(P_VALID(pClientProto->dwTargetBuffLimitID)
			&&!P_VALID(pTarget->FindBuff(pClientProto->dwTargetBuffLimitID)))
		{
			if(bShowErrMsg)CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_SkillBuffLimit);
			return false;
		}

		//--射线检测
		if(pTarget!=pSelf)
		{
			if(!RayCollideTarget(pTarget))
			{
				if(bShowErrMsg)
					CombatSysUtil::Inst()->ShowActionCheckMsg((DWORD)EEC_TargetBlocked);
				return false;
			}
		}
	}

// 	BOOL bIsAwake = HolyManMgr::Inst()->IsHolyAwake();
// 	if ( HolyManMgr::Inst()->IsHolyAwakeSkillID( pSkillData->dwSkillID ) )
// 	{
// 		INT dwValue = HolyManMgr::Inst()->GetHolyManValue();
// 		//元神召唤技能需判断元神值大于25
// 		if ( dwValue < CALLSOUL_MIN_SOULVAL )
// 		{
// 			HolyManMgr::Inst()->ShowErrorTips(33);
// 			return FALSE;
// 		}
// 		if ( bIsAwake ) //已处于 觉醒状态
// 		{
// 			HolyManMgr::Inst()->ShowErrorTips(31);
// 			return FALSE;
// 		}
// 
// 		LocalPlayer *pLP = RoleMgr::Inst()->GetLocalPlayer();
// 		if ( P_VALID(pLP) &&
// 			(pLP->GetRoleState()&ERS_PrisonArea
// 			|| pLP->GetRoleState()&ERS_Mount
// 			|| pLP->GetRoleState()&ERS_Mount2
// 			|| pLP->GetRoleState()&ERS_WaterWalk
// 			|| pLP->GetRoleState()&ERS_Stall 
// 			|| pLP->GetRoleState()&ERS_Transform ) )
// 		{
// 			HolyManMgr::Inst()->ShowErrorTips(32);
// 			return FALSE;
// 		}
// 	}
// 	else if ( pSkillData->dwSkillID == MOUNT_SKILL )
// 	{
// 		if ( bIsAwake ) //已处于 觉醒状态
// 		{
// 			HolyManMgr::Inst()->ShowErrorTips(35);
// 			return FALSE;
// 		}
// 	}
	return true;
}

bool CombatActionChecker::IsInPVPArea( Player* pTarget )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	return pSelf->GetRoleState()&ERS_PVPArea
		&& pTarget->GetRoleState()&ERS_PVPArea;
}

bool CombatActionChecker::IsInSafeArea( Player* pTarget )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	return pSelf->GetRoleState()&ERS_SafeArea
		|| pTarget->GetRoleState()&ERS_SafeArea;
}

bool CombatActionChecker::IsInArena(Player * pTarget)
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	return pSelf->GetRoleState(ERS_Arena)
		&& pTarget->GetRoleState(ERS_Arena);
}

DWORD CombatActionChecker::GetTargetType( Role* pTarget )
{
	DWORD ret=0;

	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//自身
	if(pTarget==pSelf)
		ret|=ETF_Self;

	//队友
	if(TeamSys::Inst()->IsTeammate(pTarget->GetID()))
		ret|=ETF_Teammate;

	//情侣
	if(false)//todo
		ret|=ETF_Lover;

	//配偶
	if( pSelf->GetPlayerLoverID() == pTarget->GetID() )
		ret|=ETF_Couple;

	//结拜
	if(false)//todo
		ret|=ETF_Brother;

	//玩家
	if(pTarget->IS_KIND_OF(Player))
	{
		ret|=ETF_Player;
		//同门
		if( pSelf->GetRoleGuildID() == ((Player*)pTarget)->GetRoleGuildID() && pSelf->GetRoleGuildID() != GT_INVALID )
			ret|=ETF_Guildmate;

		//师徒
		if( RoleMgr::Inst()->IsInSameMasterGuild( (Player*)pTarget ) )
			ret|=ETF_Teacher;
	}

	if(pTarget->IS_KIND_OF(NPC))
	{
		NPC* pNPC=(NPC*)pTarget;
		//宠物
		if(pNPC->IsPet())
			ret|=ETF_Pet;

		//NPC
		if(pNPC->IsNPC())
			ret|=ETF_NPC;

		//普通怪物
		if(pNPC->IsMonster())
			ret|=ETF_NormalMonster;

		//boss
		if(false)
			ret|=ETF_Boss;

		//巢穴
		if(false)
			ret|=ETF_Nest;

		const tagCreatureProto* pProto=pNPC->GetProto();
		if(P_VALID(pProto))
		{
			if(pProto->eType==ECT_GameObject)
			{
				if(pProto->nType2==EGOT_Gather)
				{
					//自然资源
					if(pProto->nType3==EGT_Mine
						||pProto->nType3==EGT_Herb)
					{
						ret|=ETF_NatuRes;
					}
					//人造资源
					else
					{
						ret|=ETF_ManRes;
					}
				}
				else if(pProto->nType2==EGOT_Normal)
				{
					ret|=ETF_NormalGameObject;
				}
				else if(pProto->nType2==EGOT_QuestInves || pProto->nType2==EGOT_CommonInves)
				{
					ret|=ETF_InvesGameObject;
				}
				else if(pProto->nType2==EGOT_QuestTalk)
				{
					ret|=ETF_QuestTalkGameObject;
				}
				else if(pProto->nType2 == EGOT_Remnant)
				{
					ret|= ETF_Remnant;
				}
				else if(pProto->nType2 == EGOT_FairySoul)
				{
					ret|= ETF_FairySoul;
				}
				else if(pProto->nType2 == EGOT_FairyHeritage)
				{
					ret|= ETF_FairyHeritage;
				}
			}
		}

		//城门
		if(false)
			ret|=ETF_Door;

		//建筑
		if(false)
			ret|=ETF_Building;
	}

	return ret;
}

bool CombatActionChecker::IsEnemy( Role* pTarget,DWORD targetType )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	
	// 城战判断优先级最高！！！！ 
	// 本地玩家处于城战进攻状态
	if (pSelf->IsInState(ES_CSAttack))
	{
		// 如果目标处于防守方状态,说明是敌人
		if (pTarget->IsInState(ES_CSDefence))
			return true;
		// 如果目标处于进攻方，说明是友军
		if (pTarget->IsInState(ES_CSAttack))
			return false;
	}
	// 本地玩家处于城战防御状态
	if (pSelf->IsInState(ES_CSDefence))
	{
		// 如果目标处于进攻状态,说明是敌人
		if (pTarget->IsInState(ES_CSAttack))
			return true;
		// 如果目标处于防御状态，说明是友军
		if (pTarget->IsInState(ES_CSDefence))
			return false;
	}

	// 竞技场判断
	if (pSelf->IsInState(ES_PvPAttack))
	{
		// 如果目标处于防守方状态,说明是敌人
		if (pTarget->IsInState(ES_PvPDefence))
			return true;
		// 如果目标处于进攻方，说明是友军
		if (pTarget->IsInState(ES_PvPAttack))
			return false;
	}
	// 本地玩家处于防御状态
	if (pSelf->IsInState(ES_PvPDefence))
	{
		// 如果目标处于进攻状态,说明是敌人
		if (pTarget->IsInState(ES_PvPAttack))
			return true;
		// 如果目标处于防御状态，说明是友军
		if (pTarget->IsInState(ES_PvPDefence))
			return false;
	}

	//玩家
	if(targetType&ETF_Player)
	{
		ASSERT(pTarget->IS_KIND_OF(Player));
		Player* pPlayer=(Player*)pTarget;

		bool bInPVPArea=IsInPVPArea(pPlayer);
		bool bInSafeArea=IsInSafeArea(pPlayer);
		bool bInArena=IsInArena(pPlayer);

		

		//自已
		if(targetType&ETF_Self)
		{
			return false;
		}
		// 夺宝奇兵
		if( (pPlayer->GetRoleState() & ERS_HoldChest ) && pSelf->GetRoleLevel() >= 20)
			return true;
		else if( IsActivityPKMap( MapMgr::Inst()->GetCurMapID() ) )
			return true;
		else if( IsTeamPKMap( MapMgr::Inst()->GetCurMapID() ) )
		{
			if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) )
				return false;
		}

		if(IsTianXuanMap( MapMgr::Inst()->GetCurMapID() ))	//天选试炼场
		{
			if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) )
				return false;
			else
				return true;
		}

		// 服务器要求这么做，暂时不加新的状态, 比武场a04,a05
		else if( IsContestMap( MapMgr::Inst()->GetCurMapID() ) ) //( MapMgr::Inst()->GetCurMapID() == 2092184628 || MapMgr::Inst()->GetCurMapID() == 2092184884 )
		{
			if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) )
				return false;
			else if(pSelf->GetRoleState()&ERS_Safeguard)
			{
				return false;
			}
			else if( pSelf->GetRoleState()&ERS_PeaceModel ) // 练级
			{
				if( pPlayer->GetRoleState()&ERS_Safeguard || 
					pPlayer->GetRoleState() & ERS_PKEX  || 
				    pPlayer->GetRoleState() & ERS_PeaceModel )
					return false;
			}
			else if(pSelf->GetRoleState()&ERS_PK) // 杀戮
			{
				if(pPlayer->GetRoleState()&ERS_Safeguard)
					return false;
			}
			else if(pSelf->GetRoleState()&ERS_PKEX) // 对抗
			{
				if( pPlayer->GetRoleState()&ERS_Safeguard || 
					pPlayer->GetRoleState() & ERS_PeaceModel )
					return false;
			}
			else
			{
				return false;
			}
		}
		//友方
		else if(targetType&FriendPlayerMask)
		{
			//英雄乱武战场里队友也是敌人
			if(bInArena && ActivityPvPMgr::Inst()->GetPvPType()==0)
				return true;
			//群雄逐鹿队友不是敌人
			if(bInArena && ActivityPvPMgr::Inst()->GetPvPType()==1)
				return false;
			if(!bInPVPArea)
				return false;
		}
		//目标处于牢狱状态
		else if(pPlayer->GetRoleState()&ERS_PrisonArea)
		{
			return false;
		}
		//其它
		else
		{
			//战场判断敌人的优先级最高
			if( bInArena )
				return true;
			if( bInSafeArea ) 
				//&& ( (pPlayer->GetRoleState() & ERS_PK) == 0 && (pPlayer->GetRoleState() & ERS_PKEX) == 0 && (pPlayer->GetRoleState() & ERS_PeaceModel) == 0 ) 
				//&& ( !(pSelf->GetRoleState()&ERS_PK) ) )
			{
				return false;
			}

			//工会战
			DWORD PlayerGuildID = pPlayer->GetRoleGuildID();
			std::set<DWORD> GuildWarSet = GuildMgr::Inst()->GetWarGuildSet();
			std::set<DWORD>::iterator it = GuildWarSet.find(PlayerGuildID);
			if (it != GuildWarSet.end())
			{
				return true;
			}
				
			if(!bInPVPArea)
			{
// 				bool isInGuildWar = false;
// 				DWORD PlayerGuildID = pPlayer->GetRoleGuildID();
// 				std::set<DWORD> GuildWarSet = GuildMgr::Inst()->GetWarGuildSet();
// 				std::set<DWORD>::iterator it = GuildWarSet.find(PlayerGuildID);
// 				if (it != GuildWarSet.end())
// 					isInGuildWar = true;

				if(pSelf->GetRoleState()&ERS_Safeguard)
				{
					return false;
				}
// 				else if (isInGuildWar)	//工会战模式
// 				{
// 					return true;
// 				}
				else if( pSelf->GetRoleState()&ERS_PeaceModel && !bInSafeArea ) // 练级
				{
					if( pPlayer->GetRoleState()&ERS_Safeguard || 
						pPlayer->GetRoleState() & ERS_PKEX  || 
						pPlayer->GetRoleState() & ERS_PeaceModel )
						return false;
				}
				else if(pSelf->GetRoleState()&ERS_PK && !bInSafeArea )
				{
					if(pPlayer->GetRoleState()&ERS_Safeguard)
						return false;
				}
				else if(pSelf->GetRoleState()&ERS_PKEX && !bInSafeArea ) // 对抗
				{
					if( pPlayer->GetRoleState()&ERS_Safeguard || 
						pPlayer->GetRoleState() & ERS_PeaceModel )
						return false;
				}
				else
				{
					return false;
				}
			}
		}
	}

	//宠物
	if(targetType&ETF_Pet)
	{
		return false;
	}

	//NPC
	if(targetType&ETF_NPC)
	{
		//if((pSelf->GetRoleState()&ERS_PK)==0)
		//{
		return false;
		//}
	}

	//普通怪物
	if(targetType&ETF_NormalMonster)
	{
	}

	//boss
	if(targetType&ETF_Boss)
	{
	}

	//巢穴
	if(targetType&ETF_Nest)
	{
	}

	//自然资源
	if(targetType&ETF_NatuRes)
	{
		return false;
	}

	//人造资源
	if(targetType&ETF_ManRes)
	{
		return false;
	}

	//普通可交互地物
	if(targetType&ETF_NormalGameObject)
	{
		return false;
	}

	//可调查地物
	if(targetType&ETF_InvesGameObject)
	{
		return false;
	}

	//任务对话地物
	if(targetType&ETF_QuestTalkGameObject)
	{
		return false;
	}

	//城门
	if(targetType&ETF_Door)
	{
		//todo
	}

	//建筑
	if(targetType&ETF_Building)
	{
		//todo
	}
	
	if (targetType&ETF_Remnant)
	{
		return false;
	}

	if (targetType&ETF_FairySoul)
	{
		return false;
	}

	if (targetType&ETF_FairyHeritage)
	{
		return false;
	}

	return true;
}

bool CombatActionChecker::IsFriend( Role* pTarget,DWORD targetType )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//玩家
	if(targetType&ETF_Player)
	{
		ASSERT(pTarget->IS_KIND_OF(Player));
		Player* pPlayer=(Player*)pTarget;

		bool bInPVPArea=IsInPVPArea(pPlayer);
		bool bInSafeArea=IsInSafeArea(pPlayer);

		// 夺宝奇兵
		if(( pPlayer->GetRoleState() & ERS_HoldChest) && pSelf->GetRoleLevel() >= 20 )
			return false;

		//工会战
		bool bInGuildWar = false;
		DWORD PlayerGuildID = pPlayer->GetRoleGuildID();
		std::set<DWORD> GuildWarSet = GuildMgr::Inst()->GetWarGuildSet();
		std::set<DWORD>::iterator it = GuildWarSet.find(PlayerGuildID);
		if (it != GuildWarSet.end())
		{
			bInGuildWar =  true;
		}

		//自已
		if(targetType&ETF_Self)
		{
		}
		else if((pSelf->IsInState(ES_CSAttack) || pSelf->IsInState(ES_CSDefence)) && (pPlayer->IsInState(ES_CSAttack) || pPlayer->IsInState(ES_CSDefence)) ) // 城战中
		{
			return (pSelf->IsInState(ES_CSAttack) && pPlayer->IsInState(ES_CSAttack)) || (pSelf->IsInState(ES_CSDefence) && pPlayer->IsInState(ES_CSDefence));
		}
		else if((pSelf->IsInState(ES_PvPAttack) || pSelf->IsInState(ES_PvPDefence)) && (pPlayer->IsInState(ES_PvPAttack) || pPlayer->IsInState(ES_PvPDefence)) ) // 竞技场内
		{
			return (pSelf->IsInState(ES_PvPAttack) && pPlayer->IsInState(ES_PvPAttack)) || (pSelf->IsInState(ES_PvPDefence) && pPlayer->IsInState(ES_PvPDefence));
		}
		else if( IsActivityPKMap( MapMgr::Inst()->GetCurMapID() ) )
			return false;
		// 服务器要求这么做，暂时不加新的状态, 比武场a04,a05
		else if( IsContestMap( MapMgr::Inst()->GetCurMapID() ) || IsTeamPKMap( MapMgr::Inst()->GetCurMapID() ) || IsTianXuanMap( MapMgr::Inst()->GetCurMapID() ) ) // ( MapMgr::Inst()->GetCurMapID() == 2092184628 || MapMgr::Inst()->GetCurMapID() == 2092184884 )
		{
			if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) )
				return true;
			else
				return false;
		}
		//友方
		else if(targetType&FriendPlayerMask)
		{
			//双方都在群雄逐鹿战场时认为为队友,在英雄乱武战场时所有人都是敌人
			if( pSelf->GetRoleState(ERS_Arena) && pPlayer->GetRoleState(ERS_Arena) && ActivityPvPMgr::Inst()->GetPvPType()==1 )
				return true;
            else if( pSelf->GetRoleState(ERS_Arena) && pPlayer->GetRoleState(ERS_Arena) && ActivityPvPMgr::Inst()->GetPvPType()==0 )
                return false;

			if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) 
				|| ( pSelf->GetRoleGuildID() == pPlayer->GetRoleGuildID() && pSelf->GetRoleGuildID() != GT_INVALID )
				|| pSelf->GetPlayerLoverID() == pPlayer->GetID() 
				|| RoleMgr::Inst()->IsInSameMasterGuild(pPlayer) )
				return true;

			if(pPlayer->GetRoleState()&ERS_PVP)
				if( TeamSys::Inst()->IsTeammate(pPlayer->GetID()) 
					|| ( pSelf->GetRoleGuildID() == pPlayer->GetRoleGuildID() && pSelf->GetRoleGuildID() != GT_INVALID )
					|| pSelf->GetPlayerLoverID() == pPlayer->GetID()
					|| RoleMgr::Inst()->IsInSameMasterGuild(pPlayer) )
					return true;
				else
					return false;
		}
		else if (bInGuildWar)
		{
			return false;
		}
		else if(bInSafeArea)
			return true;
		else if( pSelf->GetRoleState()&ERS_PeaceModel ) // 练级
		{
			if( pPlayer->GetRoleState()&ERS_Safeguard || 
				pPlayer->GetRoleState() & ERS_PKEX  || 
				pPlayer->GetRoleState() & ERS_PeaceModel )
				return true;
		}
		else if(pSelf->GetRoleState()&ERS_PK )
		{
			if(pPlayer->GetRoleState()&ERS_Safeguard)
				return true;
		}
		else if(pSelf->GetRoleState()&ERS_PKEX ) // 对抗
		{
			if( pPlayer->GetRoleState()&ERS_Safeguard || 
				pPlayer->GetRoleState() & ERS_PeaceModel )
				return true;
		}
		//其它
		//else
		//{
			return false;
		//}
	}

	//宠物
	if(targetType&ETF_Pet)
	{
		//todo
	}

	//NPC
	if(targetType&ETF_NPC)
	{
		//if(pSelf->GetRoleState()&ERS_PK)
		//{
		//	return false;
		//}
	}

	//普通怪物
	if(targetType&ETF_NormalMonster)
	{
		return false;
	}

	//boss
	if(targetType&ETF_Boss)
	{
		return false;
	}

	//巢穴
	if(targetType&ETF_Nest)
	{
		return false;
	}

	//自然资源
	if(targetType&ETF_NatuRes)
	{
		return false;
	}

	//人造资源
	if(targetType&ETF_ManRes)
	{
		return false;
	}

	//普通可交互地物
	if(targetType&ETF_NormalGameObject)
	{
		return false;
	}

	//普通可交互地物
	if(targetType&ETF_NormalGameObject)
	{
		return false;
	}

	//可调查地物
	if(targetType&ETF_InvesGameObject)
	{
		return false;
	}

	//城门
	if(targetType&ETF_Door)
	{
		return false;
	}

	//建筑
	if(targetType&ETF_Building)
	{
		return false;
	}

	return true;
}

bool CombatActionChecker::IsIndependent( Role* pTarget,DWORD targetType )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//玩家
	if(targetType&ETF_Player)
	{
		ASSERT(pTarget->IS_KIND_OF(Player));
		Player* pPlayer=(Player*)pTarget;

		bool bInPVPArea=IsInPVPArea(pPlayer);
		bool bInSafeArea=IsInSafeArea(pPlayer);

		// 夺宝奇兵
		if( (pPlayer->GetRoleState() & ERS_HoldChest)  && pSelf->GetRoleLevel() >= 20)
			return false;

		// 竞技场判断
		if (pSelf->IsInState(ES_PvPAttack) || pSelf->IsInState(ES_PvPDefence))
		{
			if (pTarget->IsInState(ES_PvPDefence) || pTarget->IsInState(ES_PvPAttack))
				return false;
		}

		//自已
		if(targetType&ETF_Self)
		{
			return false;
		}
		//友方
		else if(targetType&FriendPlayerMask)
		{
			return false;
		}
		//其它
		else
		{
			if(bInPVPArea)
				return false;

			if(bInSafeArea)
				return false;

			//工会战
			DWORD PlayerGuildID = pPlayer->GetRoleGuildID();
			std::set<DWORD> GuildWarSet = GuildMgr::Inst()->GetWarGuildSet();
			std::set<DWORD>::iterator it = GuildWarSet.find(PlayerGuildID);
			if (it != GuildWarSet.end())
			{
				return false;
			}

			if( pSelf->GetRoleState()&ERS_PeaceModel ) // 练级
			{
				//if( pPlayer->GetRoleState()&ERS_Safeguard || 
				//	pPlayer->GetRoleState() & ERS_PKEX  || 
				//	pPlayer->GetRoleState() & ERS_PeaceModel )
				//	return true;
				//else
				//	return false;

				if( (pPlayer->GetRoleState()&ERS_Safeguard || 
					pPlayer->GetRoleState() & ERS_PKEX  || 
					pPlayer->GetRoleState() & ERS_PeaceModel ) && (!pPlayer->GetRoleState() & ERS_PK))
					return true;
				else
					return false;
			}
			else if(pSelf->GetRoleState()&ERS_PK)
			{
				if(pPlayer->GetRoleState()&ERS_Safeguard)
					return true;
				else
					return false;
			}
			else if(pSelf->GetRoleState()&ERS_PKEX) // 对抗
			{
				if( pPlayer->GetRoleState()&ERS_Safeguard || 
					pPlayer->GetRoleState() & ERS_PeaceModel )
					return true;
				else
					return false;
			}
			else
				return false;
		}
	}

	//宠物
	if(targetType&ETF_Pet)
	{
		//todo
	}

	//NPC
	if(targetType&ETF_NPC)
	{
		if(pSelf->GetRoleState()&ERS_PK)
		{
			return false;
		}
	}

	//普通怪物
	if(targetType&ETF_NormalMonster)
	{
		return false;
	}

	//boss
	if(targetType&ETF_Boss)
	{
		return false;
	}

	//巢穴
	if(targetType&ETF_Nest)
	{
		return false;
	}

	//自然资源
	if(targetType&ETF_NatuRes)
	{
		return true;
	}

	//人造资源
	if(targetType&ETF_ManRes)
	{
		return true;
	}

	//普通可交互地物
	if(targetType&ETF_NormalGameObject)
	{
		return true;
	}

	//普通可交互地物
	if(targetType&ETF_NormalGameObject)
	{
		return true;
	}

	//可调查地物
	if(targetType&ETF_InvesGameObject)
	{
		return true;
	}

	//城门
	if(targetType&ETF_Door)
	{
		//todo
	}

	//建筑
	if(targetType&ETF_Building)
	{
		//todo
	}

	return true;
}

bool CombatActionChecker::IfCanAttack( DWORD targetID )
{
	Role* pTarget=RoleMgr::Inst()->FindRole(targetID);
	if(pTarget==NULL)
		return false;

	DWORD targetType=GetTargetType(pTarget);
	return IsEnemy(pTarget,targetType);
}

bool CombatActionChecker::IfCanUseItem( DWORD itemID,DWORD targetID,bool bShowErrMsg )
{
	const tagItemProto* pItemData=ItemProtoData::Inst()->FindItemProto(itemID);
	if(!P_VALID(pItemData))
		return false;

	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//--摆摊状态下不能使用物品
	const DWORD RoleStateMask=ERS_Stall;
	if(pSelf->GetRoleState()&RoleStateMask)
		return false;

	//--检查自身状态
	const DWORD dwSelfStateFlag=GetRoleStateFlag(pSelf);
	if( (dwSelfStateFlag&pItemData->dwStateLimit)!=dwSelfStateFlag  )
	{
		if(bShowErrMsg)CombatSysUtil::Inst()->ShowActionCheckMsg(EEC_CanNotUseItem);
		return false;
	}

	return true;
}

bool CombatActionChecker::IfCanMove(bool bShowErrMsg)
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//--检查属性是否初始化（移动速度等）
	if( !pSelf->IsAttributeInited() )
		return false;

	//--检查角色状态
	if(pSelf->IsInState(ES_Dizzy) || pSelf->IsInState(ES_Tie) || pSelf->IsInState(ES_Spor))
		return false;

	//--检查玩家状态
	const DWORD RoleStateMask=ERS_Stall;
	if(pSelf->GetRoleState()&RoleStateMask)
		return false;

	return true;
}

bool CombatActionChecker::IfCanTalkNPC(bool bShowErrMsg)
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//--检查角色状态
	if(pSelf->IsInState(ES_Dizzy) || pSelf->IsInState(ES_Tie) || pSelf->IsInState(ES_Spor))
		return false;

	//--检查玩家状态
	const DWORD RoleStateMask=ERS_Stall;
	if(pSelf->GetRoleState()&RoleStateMask)
		return false;

	return true;
}

bool CombatActionChecker::IfCanPickGroundItem( bool bShowErrMsg )
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();

	//--检查角色状态
	if(pSelf->IsInState(ES_Dizzy) || pSelf->IsInState(ES_Tie) || pSelf->IsInState(ES_Spor) || pSelf->IsDead())
		return false;

	//--检查玩家状态
	const DWORD RoleStateMask=ERS_Stall;
	if(pSelf->GetRoleState()&RoleStateMask)
		return false;

	return true;
}

bool CombatActionChecker::IfTargetIsValid(DWORD targetID,DWORD dwTargetLimit,BOOL bHostile,BOOL bFriendly,BOOL bIndependent)
{
	//--目标是否存在？
	Role* pTarget=RoleMgr::Inst()->FindRole(targetID);
	if(pTarget==NULL)
	{
		return false;
	}

	//--目标类型检查
	DWORD targetType=GetTargetType(pTarget);
	if( (dwTargetLimit&targetType) == 0)
	{
		return false;
	}

	//--敌我判断
	if( (bFriendly&&IsFriend(pTarget,targetType))
		|| (bHostile&&IsEnemy(pTarget,targetType))
		|| (bIndependent&&IsIndependent(pTarget,targetType)) )
	{
	}
	else
	{
		return false;
	}

	return true;
}

bool CombatActionChecker::RayCollideTarget( Role* pTarget )
{
	Vector3 src;
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	src=pSelf->GetPos();
	src.y+=pSelf->GetRoleSize().y;

	Vector3 dest=pTarget->GetPos();
	dest.y+=pTarget->GetRoleSize().y;

	Ray ray;
	ray.origin=src;
	ray.dir=dest-src;
	Vec3Normalize(ray.dir);
	ray.length=Vec3Dist(src,dest);
	//if (pTarget->IsStaticClass(_T("Door")))
	//{
	return (!MapMgr::Inst()->GetNavMap()->GetCollider()->RayCollideBoxAndTrn(ray, NavSceneNode::EFlag_NotDoor));
	//}
	//else
	//{
	//	return !MapMgr::Inst()->GetNavMap()->GetCollider()->RayCollideBoxAndTrn(ray);
	//}
	//return !MapMgr::Inst()->GetNavMap()->GetCollider()->RayCollideBoxAndTrn(ray);
}

DWORD CombatActionChecker::GetRoleStateFlag( Role* pRole )
{
	DWORD dwFlag=0;

	if(pRole->IsInState(ES_Dead))
		dwFlag|=ESF_Dead;
	else
		dwFlag|=ESF_NoDead;

	if(pRole->IsInState(ES_Dizzy))
		dwFlag|=ESF_Dizzy;
	else
		dwFlag|=ESF_NoDizzy;

	if(pRole->IsInState(ES_Tie))
		dwFlag|=ESF_Tie;
	else
		dwFlag|=ESF_NoTie;

	if(pRole->IsInState(ES_Spor))
		dwFlag|=ESF_Spor;
	else
		dwFlag|=ESF_NoSpor;

	if(pRole->IsInState(ES_Invincible))
		dwFlag|=ESF_Invincible;
	else
		dwFlag|=ESF_NoInvincible;

	if(pRole->IsInState(ES_Lurk))
		dwFlag|=ESF_Lurk;
	else
		dwFlag|=ESF_NoLurk;

	if(pRole->IsInState(ES_DisArm))
		dwFlag|=ESF_DisArm;
	else
		dwFlag|=ESF_NoDisArm;

	if(pRole->IsInState(ES_NoSkill))
		dwFlag|=ESF_NoSkill;
	else
		dwFlag|=ESF_NoNoSkill;

	if( pRole->IS_KIND_OF(Player) )
	{
		Player* pPlayer = (Player*)pRole;

		if(pPlayer->GetRoleState(ERS_Mount))
			dwFlag|=ESF_Mount;
		else
			dwFlag|=ESF_NoMount;

		//if(pPlayer->GetRoleState(ERS_Mount2))
		//	dwFlag|=ESF_Mount2;
		//else
		//	dwFlag|=ESF_NoMount2;

		if(pPlayer->GetRoleState(ERS_PrisonArea))
			dwFlag|=ESF_Prison;
		else
			dwFlag|=ESF_NoPrison;

		if(pPlayer->GetRoleState(ERS_Commerce))
			dwFlag|=ESF_Commerce;
		else
			dwFlag|=ESF_NoCommerce;
	}

	return dwFlag;
}

bool CombatActionChecker::IfCanPlayStyleAction(bool bMutual, DWORD dwFriendVal, DWORD targetID,bool bShowErrMsg)
{
	LocalPlayer* pSelf=RoleMgr::Inst()->GetLocalPlayer();
	if( !P_VALID(pSelf) || !pSelf->IsAttributeInited() )
		return false;

	// 备战状态，不可播放个性动作
	if( pSelf->IsReadyState() )
		return false;

	if(pSelf->IsSwimState())
	{
		if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_Wimming );
		return false;
	}

	// 交互动作
	if( bMutual )
	{
		// 没有目标
		if( !P_VALID(targetID) )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_RoleNotFround );
			return false;
		}

		Role* pTarget = RoleMgr::Inst()->FindRole( targetID );
		if( !P_VALID(pTarget) )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_RoleNotFround );
			return false;
		}

		// 目标是自己或不是玩家
		if( pSelf->GetID() == targetID || !pTarget->IS_KIND_OF(Player) )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
			return false;
		}

		// 可攻击目标
		if( IfCanAttack( targetID ) )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
			return false;
		}

		Player* pPlayer = (Player*)pTarget;

		// 目标不是异性
		if( !pPlayer->IsAttributeInited() || pSelf->GetAvatarAtt().bySex == pPlayer->GetAvatarAtt().bySex )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_SameSex );
			return false;
		}

		// 自身状态检查
		if( ( !pSelf->IsStyleActionState() && !pSelf->IsIdleState() ) || 
			EActionState_Normal != pSelf->GetActionState() )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
			return false;
		}

		// 目标状态检查
		if( pPlayer->IsReadyState() || 
			( !pPlayer->IsStyleActionState() && !pPlayer->IsIdleState() ) || 
			EActionState_Normal != pPlayer->GetActionState() )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
			return false;
		}
		
		// 友好度检查
		SocialMgr* pSocialMgr = (SocialMgr*)TObjRef<GameFrameMgr>()->GetFrame(_T("SocialMgr"));
		if( !P_VALID(pSocialMgr) || NULL == pSocialMgr->GetData() )
		{
			if( bShowErrMsg )
			{
				if(dwFriendVal>0)CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_FriendValNotEnough );
				else CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
			}
			return false;
		}

		const tagFriendInfo* pFriendInfo = pSocialMgr->GetData()->GetFriend( targetID );
		if(dwFriendVal>0)
		{
			if( !P_VALID(pFriendInfo) || pFriendInfo->dwFriVal < dwFriendVal )
			{
				if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_FriendValNotEnough );
				return false;
			}
		}
		else
		{
			if(!P_VALID(pFriendInfo))
			{
				if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_CanotActive );
				return false;
			}
		}			

		// 距离检查
		if( Vec3DistSq( pSelf->GetPos(), pTarget->GetPos() ) > MAX_MOTION_DIST * MAX_MOTION_DIST )
		{
			if( bShowErrMsg )	CombatSysUtil::Inst()->ShowPlayStyleActionErrMsg( E_Motion_DistanceNotEnough );
			return false;
		}
	}
	return true;
}

INT CombatActionChecker::GetCostByType( LocalPlayer* pSelf, const tagSkillData* pSkillData,ESkillCostType eSCT )
{
	INT nCost = pSkillData->pProto->nSkillCost[eSCT];

	if ( nCost > 100000 && ESCT_Rage != eSCT )
	{
		ERoleAttribute  nIndex = ERA_Null;
		float fTmpCost(0);
		fTmpCost = (float)nCost;

		if ( ESCT_HP == eSCT )
		{
			nIndex = ERA_MaxHP;
		}
		else if (ESCT_MP == eSCT)
		{
			nIndex = ERA_MaxMP;
		}
		else if (ESCT_Endurance == eSCT )
		{
			nIndex = ERA_MaxEndurance;
		}
		else if (ESCT_Valicity == eSCT )
		{
			nIndex = ERA_MaxVitality;
		}

		float fChangeRate = ((fTmpCost/100)-1000)/100;
		nCost = float(pSelf->GetAttribute(nIndex))*(fChangeRate); 
	}

	return nCost;
}

INT CombatActionChecker::GetCostByType( LocalPlayer* pSelf, const tagSkillProtoClient* pSkillProto, ESkillCostType eSCT )
{
	INT nCost = pSkillProto->nSkillCost[eSCT];

	if ( nCost > 100000 && ESCT_Rage != eSCT )
	{
		ERoleAttribute  nIndex = ERA_Null;
		float fTmpCost(0);
		fTmpCost = (float)nCost;

		if ( ESCT_HP == eSCT )
		{
			nIndex = ERA_MaxHP;
		}
		else if (ESCT_MP == eSCT)
		{
			nIndex = ERA_MaxMP;
		}
		else if (ESCT_Endurance == eSCT )
		{
			nIndex = ERA_MaxEndurance;
		}
		else if (ESCT_Valicity == eSCT )
		{
			nIndex = ERA_MaxVitality;
		}

		float fChangeRate = ((fTmpCost/100)-1000)/100;
		nCost = float(pSelf->GetAttribute(nIndex))*(fChangeRate); 
	}

	return nCost;
}

VOID CombatActionChecker::WeaponLimitMessage( INT limitType )
{
	switch(limitType)
	{
	case EITE_Sword:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_SwordLim")]);
		break;
	case EITE_Blade:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_BladeLim")]);
		break;
	case EITE_Wand:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_WandLim")]);
		break;
	case EITE_Bow:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_BowLim")]);
		break;
	case EITE_Faqi:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_FaqiLim")]);
		break;
	case EITE_Qin:
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_QinLim")]);
		break;
	default:
		break;
	}
}

VOID CombatActionChecker::TargetTypeLimitMessage( DWORD targetType )
{
	if (targetType & ETF_Self )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Self")]);
	}
	else if (targetType & ETF_Teammate)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Teammate")]);
	}
	else if (targetType & ETF_Lover)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Lover")]);
	}
	else if (targetType & ETF_Couple)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Couple")]);
	}
	else if (targetType & ETF_Brother)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Brother")]);
	}
	else if (targetType & ETF_Teacher)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Teacher")]);
	}
	else if (targetType & ETF_Pet)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Pet")]);
	}
	else if (targetType & ETF_NPC)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_NPC")]);
	}
	else if (targetType & ETF_NormalMonster)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_NormalMonster")]);
	}
	else if (targetType & ETF_Boss)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Boss")]);
	}
	else if (targetType & ETF_Nest)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Nest")]);
	}
	else if (targetType & ETF_NatuRes)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_NatuRes")]);
	}
	else if (targetType & ETF_ManRes)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_ManRes")]);
	}
	else if (targetType & ETF_Door)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Door")]);
	}
	else if (targetType & ETF_Building)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_Building")]);
	}
	else if (targetType & ETF_NormalGameObject)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_NormalGameObject")]);
	}
	else if (targetType & ETF_InvesGameObject)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_InvesGameObject")]);
	}
	else if (targetType & ETF_QuestTalkGameObject)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ETF_QuestTalkGameObject")]);
	}
	else
	{

	}
}

VOID CombatActionChecker::TargetStateLimitMessage( DWORD dwTargetStateFlag )
{
	if (dwTargetStateFlag & ESF_Dead )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Dead")]);
	}
	else if (dwTargetStateFlag & ESF_Dizzy)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Dizzy")]);
	}
	else if (dwTargetStateFlag & ESF_Spor)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Spor")]);
	}
	else if (dwTargetStateFlag & ESF_Tie)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Tie")]);
	}
	else if (dwTargetStateFlag & ESF_Invincible)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Invincible")]);
	}
	else if (dwTargetStateFlag & ESF_NoSkill)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_NoSkill")]);
	}
	else if (dwTargetStateFlag & ESF_DisArm)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_DisArm")]);
	}
	else if (dwTargetStateFlag & ESF_Lurk)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Lurk")]);
	}
	else if (dwTargetStateFlag & ESF_Mount)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Mount")]);
	}
	else if (dwTargetStateFlag & ESF_Prison)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Prison")]);
	}
	else if (dwTargetStateFlag & ESF_Mount2)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Mount2")]);
	}
	else if (dwTargetStateFlag & ESF_Commerce)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_Commerce")]);
	}
	else
	{

	}
}

VOID CombatActionChecker::PlayerStateLimitMseeage( DWORD dwSelfStateFlag )
{
	if (dwSelfStateFlag & ESF_Dead )
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpDead")]);
	}
	else if (dwSelfStateFlag & ESF_Dizzy)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpDizzy")]);
	}
	else if (dwSelfStateFlag & ESF_Spor)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpSpor")]);
	}
	else if (dwSelfStateFlag & ESF_Tie)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpTie")]);
	}
	else if (dwSelfStateFlag & ESF_Invincible)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpInvincible")]);
	}
	else if (dwSelfStateFlag & ESF_NoSkill)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpNoSkill")]);
	}
	else if (dwSelfStateFlag & ESF_DisArm)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpDisArm")]);
	}
	else if (dwSelfStateFlag & ESF_Lurk)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpLurk")]);
	}
	else if (dwSelfStateFlag & ESF_Mount)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpMount")]);
	}
	else if (dwSelfStateFlag & ESF_Prison)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpPrison")]);
	}
	else if (dwSelfStateFlag & ESF_Mount2)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpMount2")]);
	}
	else if (dwSelfStateFlag & ESF_Commerce)
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[_T("SkillError_ESF_LpCommerce")]);
	}
	else
	{

	}

}