#include "stdafx.h"
#include "pet_exchange.h"
#include "../WorldDefine/ItemDefine.h"
#include "pet_define.h"
#include "att_res.h"
#include "../WorldDefine/msg_pet.h"
#include "../WorldDefine/msg_pet_exchange.h"

//-----------------------------------------------------------------------------
// 构造&析构函数
//-----------------------------------------------------------------------------
PetExchangeMgr::PetExchangeMgr()
{
	m_pExData		   = NULL;
	m_dwTgtRoleID	   = GT_INVALID;
	m_byFollowPetCount = 0;
	m_byRidingPetCount = 0;
}

PetExchangeMgr::~PetExchangeMgr()
{
	SAFE_DEL(m_pExData);
}

//-----------------------------------------------------------------------------
// 创建交易空间 -- 如果已存在，则重新初始化
//-----------------------------------------------------------------------------
VOID PetExchangeMgr::CreateData()
{
	if(NULL == m_pExData)
	{
		m_pExData = new tagPetExchangeData;
	}

	ASSERT(m_pExData != NULL);

	ZeroMemory(m_pExData, sizeof(tagPetExchangeData));
	m_byFollowPetCount = 0;
	m_byRidingPetCount = 0;
}

//-----------------------------------------------------------------------------
// 向交易数据结构中添加交易物品 -- 返回 -- 成功:插入位置下标；失败:GT_INVALID
//-----------------------------------------------------------------------------
INT32 PetExchangeMgr::AddPet(DWORD dwPetID,DWORD dwPetTypeID)
{
	ASSERT(m_pExData != NULL);

	INT32 nInsIndex = GT_INVALID;
	for(INT32 i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		if(m_pExData->dwPetIDs[i] == dwPetID)
		{
			IMSG(_T("Add the same pet<id: %lu> to exchange!!!!\r\n"), dwPetID);
			return GT_INVALID;
		}
		else if(0 == m_pExData->dwPetIDs[i] && GT_INVALID == nInsIndex)
		{
			nInsIndex = i;
		}
	}

	if(nInsIndex != GT_INVALID)
	{
		m_pExData->dwPetIDs[nInsIndex] = dwPetID;

		++m_pExData->byPetNum;
	}
	const tagPetProto*  pPetProto = g_attRes.GetPetProto(dwPetTypeID);
	if ( P_VALID(pPetProto) )
	{
		//跟随宠物
		if( 6 == pPetProto->nType3)
		{
			m_byFollowPetCount++;
		}
		else
		{
			m_byRidingPetCount++;
		}
	}


	return E_Pets_Success;
}

//-----------------------------------------------------------------------------
// 从交易物品中取出指定物品 -- 成功返回E_Success，失败返回相应错误码
//-----------------------------------------------------------------------------
DWORD PetExchangeMgr::DecPet(DWORD dwPetID,DWORD dwPetTypeID)
{
	ASSERT(m_pExData != NULL);


	for(INT32 i=0; i<MAX_EXCHANGE_ITEM; ++i)
	{
		if(m_pExData->dwPetIDs[i] == dwPetID)
		{
			m_pExData->dwPetIDs[i] = 0;
			--m_pExData->byPetNum;

			const tagPetProto*  pPetProto = g_attRes.GetPetProto(dwPetTypeID);
			if ( P_VALID(pPetProto) )
			{
				//跟随宠物
				if( 6 == pPetProto->nType3)
				{
					m_byFollowPetCount--;
				}
				else
				{
					m_byRidingPetCount--;
				}
			}
			return E_Success;
		}
	}

	return GT_INVALID;
}