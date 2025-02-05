//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: map_mgr.h
// author: aslan
// actor:
// data: 2008-6-27
// last:
// brief: 地图管理器
// remark: 每个地图管理器生成一条线程，对该地图中的所有人物及其他对象进行更新操作，同时完成地图间同步
//------------------------------------------------------------------------------------------------------
#pragma once

#include "mutex.h"
#include "map.h"


struct tagMapInfo;
struct tagInstance;
class Map;
class MapInstance;
class MapCreator;
class Role;
class MapRestrict;

class MapMgr
{
	friend class MapRestrictNormal;
	friend class MapRestrictInstance;
	friend class MapRestrictStable;
	friend class MapRestrictPVP;

public:
	MapMgr();
	~MapMgr();

	//-------------------------------------------------------------------------------
	// 初始化，更新和销毁
	//-------------------------------------------------------------------------------
	BOOL				Init(tagMapInfo* pInfo);
	DWORD				ThreadUpdate();
	VOID				Destroy();

	//--------------------------------------------------------------------------------------------------------
	// 线程管理
	//--------------------------------------------------------------------------------------------------------
	static VOID			StopThread()					{ InterlockedExchange((LPLONG)&m_bTerminate, TRUE); }
	tstring&			GetThreadName()					{ return m_strThreadName; }

	//--------------------------------------------------------------------------------------------------------
	// 各种Get
	//--------------------------------------------------------------------------------------------------------
	Map*				GetSingleMap()					{ return m_pSingleMap; }
	MapInstance*		GetInstance(DWORD dwInstanceID) { return m_mapInstance.Peek(dwInstanceID); }
	const tagMapInfo*	GetMapInfo()					{ return m_pInfo; }
	
	// 此函数为地图s60,s57特殊提供
	MapInstance* GetOneInstance()
	{
		if(m_mapInstance.Empty()) 
			return NULL; 
		else 
		{
			MapInstance* p = NULL;
			TMap<DWORD, MapInstance*>::TMapIterator it = m_mapInstance.Begin();
			m_mapInstance.PeekNext(it, p);
			if (P_VALID(p))
			{
				return p;
			}
		}

		return NULL;
	}

	BOOL		IsNormalMap()	{ return EMT_Normal == m_pInfo->eType; }
	BOOL		IsInstanceMap()	{ return EMT_Instance == m_pInfo->eType; }

	//--------------------------------------------------------------------------------------------------------
	// 副本的创建和删除副本
	//--------------------------------------------------------------------------------------------------------
	MapInstance*		CreateInstance(Role* pCreator, DWORD dwMisc);
	BOOL				CreateScriptInstance(DWORD dwInstanceID);
	VOID				DestroyInstance(MapInstance* pInstance);
	INT					GetCurInstanceNum()	{ return m_nInstanceNum; }
	VOID				ChangeInstanceNum(INT nNum)		{ m_nInstanceNum += nNum; }

	//--------------------------------------------------------------------------------------------------------
	// 副本的入口及出口判断
	//--------------------------------------------------------------------------------------------------------
	Map*				CanEnter(Role* pRole, DWORD dwMisc=GT_INVALID);
	Map*				CanTakeOverWhenOnline(Role* pRole, DWORD& dwOutMapID, Vector3& vOut);

	//--------------------------------------------------------------------------------------------------------
	// 玩家传出副本
	//--------------------------------------------------------------------------------------------------------
	VOID				RoleInstanceOut(Role* pRole);

	// 统计副本内的所有人数
	INT					GetInstancesRoleNum();


private:
	TSFPTrunk<MapMgr>			m_Trunk;
	TObjRef<Thread>				m_pThread;
	tstring						m_strThreadName;					// 线程名称
	static VOLATILE BOOL		m_bTerminate;						// 线程停止标志

	const tagMapInfo*			m_pInfo;							// 地图基本属性定义
	Map*						m_pSingleMap;						// 如果是普通地图，则该指针指向该唯一地图
	TMap<DWORD, MapInstance*>   m_mapInstance;						// 如果是副本地图，则管理基于该地图创建的所有副本

	MapRestrict*				m_pRestrict;						// 策略对象
	INT							m_nInstanceNum;						// 当前已经创建的副本数量

	// 要延迟攻击防御处理的容器 Jason 2010-7-17 v1.0.0
	multimap<DWORD,DWORD>		m_mmAttackDefenseChangeInfo;

public:
	DWORD	GetMapID(VOID) const;

	// Jason 各个地图线程调用，不再同一张图的玩家预警信息处理
	VOID DelayedRemoveAttackDefenseRelationship(DWORD srcID,DWORD desID);
	// 在map_creator的update操作最后调用，这个是同步的，地图线程不能调用
	VOID DealAttackDefenseRelationship();
};
