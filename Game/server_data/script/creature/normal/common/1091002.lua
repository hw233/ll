--怪物喊话脚本

-- 怪物进入战斗喊话
function c1091002_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=75 then
		--75%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 30005)
       end
end

aux.RegisterCreatureEvent(1091002, 2, "c1091002_OnEnterCombat")

