--怪物喊话脚本

-- 怪物进入战斗喊话
function c1060046_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=30 then
		--30%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 20176)
       end
end

aux.RegisterCreatureEvent(1060046, 2, "c1060046_OnEnterCombat")