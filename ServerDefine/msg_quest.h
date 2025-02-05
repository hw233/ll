//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: msg_quest.h
// author: 
// actor:
// data: 2008-09-11
// last:
// brief: 任务相关
//-----------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/quest_board.h"

#pragma pack(push)
#pragma pack(1)
//-----------------------------------------------------------------------------


#include "../WorldDefine/msg_common_errorcode.h"
#include "../WorldDefine/QuestDef.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 接取任务
//-----------------------------------------------------------------------------
CMD_START(NDBC_AcceptQuest)						// 接取任务 World ---> DB
	DWORD			dwRoleID;					// 角色ID
	tagAcceptQuest  acceptQuest;	
CMD_END;

CMD_START(NDBS_AcceptQuest)						// 接取任务 World <--- DB 暂时没有返回值
CMD_END;

//-----------------------------------------------------------------------------
// 完成任务
//-----------------------------------------------------------------------------
CMD_START(NDBC_CompleteQuest)					// 完成任务 World ---> DB
	DWORD				dwRoleID;				// 角色ID
	tagQuestDoneSave	QuestDone;
CMD_END;

//-----------------------------------------------------------------------------
// 完成日常活动任务
//-----------------------------------------------------------------------------
CMD_START(NDBC_DailyCompleteQuest)					// 完成任务 World ---> DB
DWORD				dwRoleID;				// 角色ID
tagQuestDoneSave	QuestDone;
CMD_END;

//-----------------------------------------------------------------------------
//清零日常活动任务
//-----------------------------------------------------------------------------
CMD_START(NDBC_ClearDailyCompleteQuest)					// 完成任务 World ---> DB
DWORD				dwRoleID;				// 角色ID
DWORD               dwCount;
tagQuestDoneSave	pData[1];
CMD_END;

CMD_START(NDBS_CompleteQuest)					// 完成任务 World <--- DB 暂时没有返回值
CMD_END;

//------------------------------------------------------------------------------
// 删除任务
//------------------------------------------------------------------------------
CMD_START(NDBC_DiscardQuest)					// 放弃任务 World ---> DB
	DWORD	dwRoleID;							// 角色ID
	UINT16	u16QuestID;							// 任务ID
CMD_END;

CMD_START(NDBS_DiscardQuest)					// 放弃任务 World <--- DB
CMD_END;

//------------------------------------------------------------------------------
// 任务板任务相关
//------------------------------------------------------------------------------
CMD_START(NDBC_QuestBoardSaveAllQuest)
	UINT16								n16QuestCnt;
	tagBoardQuestSaveData		pData[1];
CMD_END

CMD_START(NDBC_QuestBoardSaveOneQuest)
	tagBoardQuestSaveData		sQuestSaveData;
CMD_END

#pragma pack(pop)
