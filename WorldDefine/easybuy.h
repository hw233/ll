#pragma once

//-----------------------------------------------------------------------------
// 常量定义
//-----------------------------------------------------------------------------

// 一个界面可以挂载的最大商品数量
const int UI_MAX_ITEM = 3;

//-----------------------------------------------------------------------------
// 枚举定义
//-----------------------------------------------------------------------------

// 需要挂载商品的界面
enum EasyBuyUIType
{
	EBUT_Start = -1,
	EBUT_NULL,
	EBUT_RevivalUI,			// 复活界面
	EBUT_SkillUI,			// 人物技能界面
	EBUT_BagUI,				// 背包界面
	EBUT_AttUI,				// 人物属性界面
	EBUT_PetUI,				// 宠物界面
	EBUT_ConsolidateUI,		// 强化界面
	EBUT_TaskUI,			// 任务界面
	EBUT_storageUI,			// 仓库界面
	EBUT_SocialUI,          // 好友列表界面
	EBUT_END,
};
