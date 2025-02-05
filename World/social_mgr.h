//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: social_mgr.h
// author: wjiang
// actor:
// data: 2008-12-03
// last:
// brief: 事件管理类
//-------------------------------------------------------------------------------------------------------
#pragma once
#include "event_mgr.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/ItemDefine.h"

class SocialMgr : public EventMgr<SocialMgr>
{
public:
	BOOL	Init();
	~SocialMgr() {}
	VOID	Update();
	
	// 好友相关
	VOID	OnMakeFriendFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnCancelFriendFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnUpdateFriGrpFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnInsertBlkListFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnDeleteBlkListFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnSendGiftFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnSendGiftReplyFunc(DWORD dwSenderID, VOID* pEventMessage);

	// 同步tagRoleInfo中玩家等级
	VOID	OnSynRoleLevelFunc(DWORD dwSenderID, VOID* pEventMessage);

	// 删除一个仇敌
	VOID	OnDeleteEnemyFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	DeleteEnemy(DWORD dwDestRoleID, DWORD dwSrcRoleID);

	// 插入一个仇敌
	VOID	InsertEnemy(Role* pRole, DWORD dwEnemyId);

	VOID	SendLogoutToFriend(Role *pRole);
	VOID	SendLoginToFriend(Role *pRole);

	// 仇敌上线，通知玩家
	VOID	SendEnemyLoginToRole(Role *pRole);

	VOID MakeFriend(DWORD dwSenderID, VOID* pEventMessage, BOOL bSendMakeFriNotice = TRUE);
private:
	static	VOID RegisterSocialEventFunc();

private:
	BYTE	CancelFriend(Role *pSrcRole, DWORD dwSrcRoleID, DWORD dwDestRoleID);
	VOID	UpdateGiftVec();								
	VOID	SendAllFriendMessage(Role *pRole, VOID *pMsg, DWORD dwSize, BOOL bLogin);
	DWORD	CanSendGift(Role* pSrcRole, Role* pDestRole, DWORD dwSrcRoleID, DWORD dwDestRoleID, INT64 n64ItemID, tagItem* &pItem);

private:
	vector<tagSendGift>			vecSendGift;			// 发送的礼物
};

extern SocialMgr g_socialMgr;