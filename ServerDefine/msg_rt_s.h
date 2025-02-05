//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_rt_s.h
// author: 
// actor:
// data: 2008-04-27
// last:
// brief: 运维服务端与游戏世界之间的消息定义
//-----------------------------------------------------------------------------
#pragma once
#include "../ServerDefine/login_define.h"
#include "../ServerDefine/rt_define.h"
#include "../ServerDefine/gm_tool_define.h"
#include "../WorldDefine/base_define.h"

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 连接初始消息 发自游戏世界，发到运维服务器（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NSC_LoginService)
	DWORD			dwSectionID;					// 大区ID
	DWORD			dwWorldID;						// 游戏世界ID
	EServerType		eType;							// 类型
CMD_END;

//-----------------------------------------------------------------------------
// 初始连接确认消息 发自运维服务器（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_LoginService)
	DWORD			dwErrorCode;					// 0:成功
CMD_END;

//-----------------------------------------------------------------------------
// 连接初始消息 发自运维服务端（GMserver），发到游戏世界本地运维服务（RTService）
//-----------------------------------------------------------------------------
CMD_START(NLSS_LoginService)
	EServerType		eType;							// 类型
CMD_END;

//-----------------------------------------------------------------------------
// 初始连接确认消息 发自游戏世界本地运维服务（RTService）,发到运维服务端（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NLSC_LoginService)
	DWORD			dwErrorCode;					// 0:成功
CMD_END;

//-----------------------------------------------------------------------------
// 关闭游戏世界服务器的消息，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_Close)

CMD_END

//-----------------------------------------------------------------------------
// 关闭游戏世界服务器的确认消息，发自游戏世界,发到运维服务端（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NSC_Close)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 强行关闭游戏世界服务器的消息，发自运维服务端（GMserver），发到游戏世界本地运维服务（RTService）
//-----------------------------------------------------------------------------
CMD_START(NLSS_Close)
	
CMD_END

//-----------------------------------------------------------------------------
// 强行关闭游戏世界服务器的确认消息，发自游戏世界本地运维服务（RTService）,发到运维服务端（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NLSC_Close)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 开启游戏世界服务器的消息，发自运维服务端（GMserver），发到游戏世界本地运维服务（RTService）
//-----------------------------------------------------------------------------
CMD_START(NLSS_Start)

CMD_END

//-----------------------------------------------------------------------------
// 开启游戏世界服务器的确认消息，发自游戏世界本地运维服务（RTService）,发到运维服务端（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NLSC_Start)
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 定时返回给该运维服务程序的服务器状态消息，发自游戏世界，发到运维服务端（GMserver）
//-----------------------------------------------------------------------------
CMD_START(NSC_WorldInfo)
	//TCHAR			szSectionName[X_SHORT_NAME];
	//TCHAR			szWorldName[X_SHORT_NAME];
	//EServerType		eType;
	EWorldStatus	eStatus;
	INT				nOnlineNum;					//游戏世界在线人数 4
	INT				nMaxOnlineNum;				//游戏世界最大在线人数 4
	INT				nOnlineNumDisplay;			//显示的在线人数 4
CMD_END

//-----------------------------------------------------------------------------
// 双倍设置	，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_Double)
	DWORD			dwClientID;	
	EDoubleType		eDType;
	DWORD			dwLastTime;						//时间 tagDWORDTime
	DWORD			dwOpenTime;						//开放时间
	DWORD			dwRatio;						//比例
CMD_END

CMD_START(NSC_Double)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 取消双倍设置	，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_CancelDouble)
	DWORD			dwClientID;	

CMD_END

CMD_START(NSC_CancelDouble)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 自动循环公告 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_AutoNotice)
	DWORD			dwClientID;
	TCHAR			szContent[Notice_Content];		//内容
	EColorType		eType;							//颜色
	INT				nCirInterval;					//循环间隔
	//INT				nCirNum;					//循环次数
CMD_END

CMD_START(NSC_AutoNotice)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 右下角公告 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_RightNotice)								
	DWORD			dwClientID;
	TCHAR			szTitle[Notice_Title];			//标题
	TCHAR			szContent[Notice_Content];		//内容
	TCHAR			szLink[Notice_Link];			//链接的网站
CMD_END

CMD_START(NSC_RightNotice)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 取消右下角	，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
//CMD_START(NSS_CancelRightNotice)
//	DWORD			dwClientID;	
//CMD_END
//
//CMD_START(NSC_CancelRightNotice)
//	DWORD			dwClientID;
//	DWORD			dwErrorCode;
//CMD_END

//-----------------------------------------------------------------------
// 系统指令消息 ，发自运维服务端（GMserver），发到游戏世界的RTservice
//-----------------------------------------------------------------------
CMD_START(NLSS_SystemCmd)								
	DWORD			dwClientID;
	TCHAR			szCmd[System_Cmd_Length];
CMD_END

CMD_START(NLSC_SystemCmd)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 修改最大在线人数的消息 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_MaxNum)								
	DWORD			dwClientID;
	INT				nMaxNum;
CMD_END

CMD_START(NSC_MaxNum)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 更新商城 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_UpdateMall)								
	DWORD			dwClientID;
CMD_END

CMD_START(NSC_UpdateMall)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 更新神之竞标 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_UpdateGodBid)								
	DWORD			dwClientID;
CMD_END

CMD_START(NSC_UpdateGodBid)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 更新砸蛋 ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_UpdateGoldEggPct)								
	DWORD			dwClientID;
CMD_END

CMD_START(NSC_UpdateGoldEggPct)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
// 更新item_needlog ，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------
CMD_START(NSS_ReloadItemNeedLogPct)								
DWORD			dwClientID;
CMD_END

CMD_START(NSC_ReloadItemNeedLogPct)
DWORD			dwClientID;
DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 封号 发自GMServer 发往loginserver
//-----------------------------------------------------------------------------
CMD_START(NSS_ForbidAccount)
	DWORD			dwClientID;
	TCHAR			szWorld[X_SHORT_NAME];
	BOOL			bForbid; //0:解封 1:封
	TCHAR           szAccount[1];
CMD_END

//-----------------------------------------------------------------------------
// 封号 发自loginserver 发往GMServer 
//-----------------------------------------------------------------------------
CMD_START(NSC_ForbidAccount)
	DWORD			dwClientID;
	TCHAR           szAccount[X_SHORT_NAME];
	BOOL			bForbid; //0:解封 1:封
	DWORD			dwErrorCode;
CMD_END


//-----------------------------------------------------------------------------
// 封IP 发自GMServer 发往loginserver
//-----------------------------------------------------------------------------
CMD_START(NSS_ForbidIP)
	DWORD			dwClientID;
	DWORD			dwIP;
	BOOL			bForbid; //0:解封 1:封
CMD_END

//-----------------------------------------------------------------------------
// 封IP 发自loginserver 发往GMServer 
//-----------------------------------------------------------------------------
CMD_START(NSC_ForbidIP)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 聊天频道自动公告，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_AutoChatNotice)
	DWORD			dwClientID;
	TCHAR			szNotice[X_HUGE_STRING];		//内容
CMD_END

CMD_START(NSC_AutoChatNotice)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 取消聊天频道自动公告	，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
//CMD_START(NSS_CancelAutoChatNotice)
//	DWORD			dwClientID;	
//CMD_END
//
//CMD_START(NSC_CancelAutoChatNotice)
//	DWORD			dwClientID;
//	DWORD			dwErrorCode;
//CMD_END

//-----------------------------------------------------------------------------
// 清空百宝袋，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_ClearBaibao)
	DWORD			dwClientID;
	TCHAR			szOwnerName[X_SHORT_NAME];		// 角色名字
CMD_END

CMD_START(NSC_ClearBaibao)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 角色禁言，发自运维服务端（GMserver），发到游戏世界
//-----------------------------------------------------------------------------
CMD_START(NSS_RoleSpeak)
	DWORD			dwClientID;
	TCHAR			szRoleName[X_SHORT_NAME];		// 角色名字
	bool			bAction;						// 0 禁言 1解禁
CMD_END

CMD_START(NSC_RoleSpeak)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

CMD_START(NSS_SetFackNumer)
	float			fFackRate;
CMD_END

//-----------------------------------------------------------------------------
// 检查要增加的道具ID是否有效
//-----------------------------------------------------------------------------
CMD_START(NSS_ItemTypeID)
	DWORD			dwClientID;
	DWORD			dwTypeID;					// 物品的TypeID
	DWORD			dwSectionID;
	DWORD			dwWorldID;
	DWORD			dwAccountID;				
	DWORD			dwRoleID;
	INT				nItemNum;
	BYTE			byQlty;
CMD_END

CMD_START(NSC_ItemTypeID)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
	DWORD			dwSectionID;
	DWORD			dwWorldID;
	DWORD			dwTypeID;					// 物品的TypeID
	DWORD			dwAccountID;				
	DWORD			dwRoleID;
	INT				nItemNum;
	BYTE			byQlty;
CMD_END

//-----------------------------------------------------------------------------
// 检查要增加的道具ID是否有效
//-----------------------------------------------------------------------------
CMD_START(NSS_ClearAttPoint)
	DWORD			dwClientID;
	DWORD			dwRoleID;
	//TCHAR			szRoleName[X_SHORT_NAME];
CMD_END

CMD_START(NSC_ClearAttPoint)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 修改帮派成员等级
//-----------------------------------------------------------------------------
CMD_START(NSS_GuildChgMemLvl)
	DWORD			dwClientID;
	DWORD			dwRoleID;
	INT				nLvl;
	TCHAR			szGuildName[X_SHORT_NAME];
CMD_END

CMD_START(NSC_GuildChgMemLvl)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 修改帮派金钱
//-----------------------------------------------------------------------------
CMD_START(NSS_GuildChgFund)
	DWORD			dwClientID;
	TCHAR			szGuildName[X_SHORT_NAME];
	INT32			n32Fund;
CMD_END

CMD_START(NSC_GuildChgFund)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 修改帮派状态
//-----------------------------------------------------------------------------
CMD_START(NSS_GuildChgState)
	DWORD			dwClientID;
	DWORD			dwState;
	TCHAR			szGuildName[X_SHORT_NAME];
CMD_END

CMD_START(NSC_GuildChgState)
	DWORD			dwClientID;
	DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------------
// 修改帮派据点战占领状态
//-----------------------------------------------------------------------------
CMD_START(NSS_GuildGodMiracle)
DWORD			dwClientID;
DWORD			dwMiracleType;//类型
DWORD			dwGuildID;
CMD_END

CMD_START(NSC_GuildGodMiracle)
DWORD			dwClientID;
DWORD			dwErrorCode;
CMD_END

//-----------------------------------------------------------------------
//  增加需要记录PK log的角色
//-----------------------------------------------------------------------
CMD_START(NSS_AddNeedPKLogRole)
	DWORD	dwClientID;
	DWORD	dwRoleID;
	TCHAR	szRoleName[X_SHORT_NAME];
CMD_END

CMD_START(NSC_AddNeedPKLogRole)
	DWORD	dwClientID;
	DWORD	dwErrorCode;
CMD_END

//添加过滤词
CMD_START(NSS_AddFilterWords)
DWORD			dwClientID;
TCHAR			szFilterWords[X_SHORT_NAME];
CMD_END

CMD_START(NSC_AddFilterWords)
DWORD			dwClientID;
DWORD			dwErrorCode;
CMD_END

CMD_START(NSS_DelFilterWords)
DWORD			dwClientID;
TCHAR			szFilterWords[X_SHORT_NAME];
CMD_END

CMD_START(NSC_DelFilterWords)
DWORD			dwClientID;
DWORD			dwErrorCode;
CMD_END

#pragma pack(pop)