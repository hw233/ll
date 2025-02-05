--1级员工福利包
function I4700038_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070057, 19, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070058, 25, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700039, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051607, 99, -1, 8, 420)  --初级加工心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051607, 99, -1, 8, 420)  --初级加工心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051608, 99, -1, 8, 420)  --初级铸造心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051608, 99, -1, 8, 420)  --初级铸造心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051609, 99, -1, 8, 420)  --初级裁缝心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3051609, 99, -1, 8, 420)  --初级裁缝心得（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050001, 100, -1, 8, 420)  --原矿
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
end

function I4700038_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 11) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700038, 1, "I4700038_GiftBag")
aux.RegisterItemEvent(4700038, 0, "I4700038_CanUseGiftBag")

--30级员工福利包
function I4700039_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700040, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020116, 2, -1, 8, 420)   --绑定+10
	role.AddRoleItem(MapID, InstanceID, TargetID, 4222004, 1, -1, 8, 420)	--熊猫飞车（呆）
	role.AddRoleItem(MapID, InstanceID, TargetID, 4211403, 1, -1, 8, 420)	--旺财
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070008, 1, -1, 8, 420)   --随身仓库
	role.AddRoleItem(MapID, InstanceID, TargetID, 5600007, 30, -1, 8, 420)	--超级经验符（包天）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070056, 999, -1, 8, 420)	--悬赏令
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070085, 1, -1, 8, 420)	--5000赠券
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)	--战斗神符[包周]
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070022, 20, -1, 8, 420)	--绑定妖精秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 4622106, 1, -1, 8, 420)   --骑术6级技能书
	role.AddRoleItem(MapID, InstanceID, TargetID, 4622210, 1, -1, 8, 420)	--骑术精通6级技能书
	role.AddRoleItem(MapID, InstanceID, TargetID, 4630001, 1, -1, 8, 420)	--悬赏使者
	role.AddRoleItem(MapID, InstanceID, TargetID, 4630101, 1, -1, 8, 420)	--空间行者
	role.AddRoleItem(MapID, InstanceID, TargetID, 4630201, 1, -1, 8, 420)	--商人守护者
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700278, 10, -1, 8, 420)  --经验宝瓶（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
	role.AddRoleItem(MapID, InstanceID, TargetID, 4080079, 999, -1, 8, 420)  --北洲之雪
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820087, 999, -1, 8, 420)  --信仰神符（绑定）

	local Vocation = role.GetRoleClass(TargetID)
	if Vocation == 1 or	Vocation == 2 or Vocation == 3 or Vocation == 6 or Vocation == 7 or Vocation == 8 or Vocation == 9	then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8160020, 1, 5, 8, 420)
	elseif Vocation == 4 or Vocation == 5 or Vocation == 10 or Vocation == 11 or Vocation == 12 or Vocation == 13 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8170020, 1, 5, 8, 420)
	end
end

function I4700039_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 14) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700039, 1, "I4700039_GiftBag")
aux.RegisterItemEvent(4700039, 0, "I4700039_CanUseGiftBag")

--50级员工福利包
function I4700040_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700041, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020116, 8, -1, 8, 420) 	--绑定+10
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020118, 2, -1, 8, 420) 	--绑定+12
	role.AddRoleItem(MapID, InstanceID, TargetID, 4400031, 20, -1, 8, 420)	--精炼妖精秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700278, 50, -1, 8, 420)  --经验宝瓶（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820081, 999, -1, 8, 420)  --天神代币
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820061, 999, -1, 8, 420)  --欢乐币
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
    local Vocation = role.GetRoleClass(TargetID)
	if Vocation == 1 or	Vocation == 2 or Vocation == 3 or Vocation == 6 or Vocation == 7 or Vocation == 8 or Vocation == 9	then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8010021, 1, 5, 8, 420)  --传承之刃
		role.AddRoleItem(MapID, InstanceID, TargetID, 8020021, 1, 5, 8, 420)	--传承之弓
		role.AddRoleItem(MapID, InstanceID, TargetID, 3030121, 10, -1, 8, 420)	--绑定三级太阳石
	elseif Vocation == 4 or Vocation == 5 or Vocation == 10 or Vocation == 11 or Vocation == 12 or Vocation == 13 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8030021, 1, 5, 8, 420)	--传承之琴
		role.AddRoleItem(MapID, InstanceID, TargetID, 8040021, 1, 5, 8, 420)	--传承之器
		role.AddRoleItem(MapID, InstanceID, TargetID, 3030123, 10, -1, 8, 420)	--绑定三级月光石
	end
end

function I4700040_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 11) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700040, 1, "I4700040_GiftBag")
aux.RegisterItemEvent(4700040, 0, "I4700040_CanUseGiftBag")

--60级员工福利包
function I4700041_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700042, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 5600007, 30, -1, 8, 420)  --超级经验符（包天）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070085, 1, -1, 8, 420)	--5000赠券
	role.AddRoleItem(MapID, InstanceID, TargetID, 3010020, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3010025, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700278, 50, -1, 8, 420)  --经验宝瓶（绑定
	role.AddRoleItem(MapID, InstanceID, TargetID, 4800039, 20, -1, 8, 420)  --60伪魔兑换券
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820061, 999, -1, 8, 420)  --欢乐币
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820035, 10, -1, 8, 420)  --上古典籍
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
	 local Vocation = role.GetRoleClass(TargetID)
	if Vocation == 1 or	Vocation == 2 or Vocation == 3 or Vocation == 6 or Vocation == 7 or Vocation == 8 or Vocation == 9	then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8013007, 1, 5, 8, 420)  --魔神武器
		role.AddRoleItem(MapID, InstanceID, TargetID, 8023007, 1, 5, 8, 420)
	elseif Vocation == 4 or Vocation == 5 or Vocation == 10 or Vocation == 11 or Vocation == 12 or Vocation == 13 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8033007, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8043007, 1, 5, 8, 420)
	end
end

function I4700041_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 12) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700041, 1, "I4700041_GiftBag")
aux.RegisterItemEvent(4700041, 0, "I4700041_CanUseGiftBag")

--70级员工福利包
function I4700042_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700043, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020121, 10, -1, 8, 420)  --绑定+15
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020061, 20, -1, 8, 420)	--中级吸魂石
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700278, 50, -1, 8, 420)  --经验宝瓶（绑定
	role.AddRoleItem(MapID, InstanceID, TargetID, 4800131, 999, -1, 8, 420) --修炼场门票（绑定
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820061, 999, -1, 8, 420)  --欢乐币
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820035, 30, -1, 8, 420)  --上古典籍
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
--[[	 local Vocation = role.GetRoleClass(TargetID)
	if Vocation == 1 or	Vocation == 2 or Vocation == 3 or Vocation == 6 or Vocation == 7 or Vocation == 8 or Vocation == 9	then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8013007, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8023007, 1, 5, 8, 420)
	elseif Vocation == 4 or Vocation == 5 or Vocation == 10 or Vocation == 11 or Vocation == 12 or Vocation == 13 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8033007, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8043007, 1, 5, 8, 420)
	end]]
end

function I4700042_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 8) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700042, 1, "I4700042_GiftBag")
aux.RegisterItemEvent(4700042, 0, "I4700042_CanUseGiftBag")

--80级员工福利包
function I4700043_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020061, 20, -1, 8, 420)  --中级吸魂石
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020016, 10, -1, 8, 420)  --高级吸魂石
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020124, 5, -1, 8, 420)  --绑定+18
	role.AddRoleItem(MapID, InstanceID, TargetID, 3020126, 2, -1, 8, 420)  --绑定+20
	role.AddRoleItem(MapID, InstanceID, TargetID, 4400031, 50, -1, 8, 420)  --精炼妖精秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070056, 999, -1, 8, 420)	--悬赏令[免费]
	role.AddRoleItem(MapID, InstanceID, TargetID, 5600007, 100, -1, 8, 420)	--超级经验符（包天）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)	--战斗神符（包周）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3070014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3010021, 1, -1, 8, 420)	--超级圣药
	role.AddRoleItem(MapID, InstanceID, TargetID, 3010026, 1, -1, 8, 420)	--超级圣水
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700278, 99, -1, 8, 420)  --经验宝瓶（绑定
	role.AddRoleItem(MapID, InstanceID, TargetID, 4800041, 20, -1, 8, 420)  --80伪魔兑换�
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820061, 999, -1, 8, 420)  --欢乐币�
	role.AddRoleItem(MapID, InstanceID, TargetID, 4700230, 15, -1, 8, 420)   --9级成长装备箱
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820255, 200, -1, 8, 420)  --成长之书（绑定）
	role.AddRoleItem(MapID, InstanceID, TargetID, 4820035, 50, -1, 8, 420)  --上古典籍
	role.AddRoleItem(MapID, InstanceID, TargetID, 3060062, 2, -1, 8, 420)  --妖精潜能秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 3060058, 10, -1, 8, 420)  --稳固妖精秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 3060059, 20, -1, 8, 420)  --高等妖精秘药
	role.AddRoleItem(MapID, InstanceID, TargetID, 3060061, 1, -1, 8, 420)  --至尊秘药（员工专用，未对外道具）
	role.AddRoleItem(MapID, InstanceID, TargetID, 3050002, 999, -1, 8, 420)  --矿渣
	role.AddRoleItem(MapID, InstanceID, TargetID, 3030115, 50, -1, 8, 420)  --4级宝石拆除器
	local Vocation = role.GetRoleClass(TargetID)
	if Vocation == 1 or	Vocation == 2 or Vocation == 3 or Vocation == 6 or Vocation == 7 or Vocation == 8 or Vocation == 9	then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8013009, 1, 5, 8, 420)  --80魔神武器
		role.AddRoleItem(MapID, InstanceID, TargetID, 8023009, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 3030306, 10, -1, 8, 420)--炙热太阳石[4级]

	elseif Vocation == 4 or Vocation == 5 or Vocation == 10 or Vocation == 11 or Vocation == 12 or Vocation == 13 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8033009, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8043009, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 3030326, 10, -1, 8, 420)--皎洁月光石[4级]
	end
end

function I4700043_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 34) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(4700043, 1, "I4700043_GiftBag")
aux.RegisterItemEvent(4700043, 0, "I4700043_CanUseGiftBag")

