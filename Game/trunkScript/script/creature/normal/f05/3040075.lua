----scriptdata[0]标记进副本是否刷怪
----scriptdata[1]标记进副本是否刷怪
----scriptdata[2]标记进副本是否刷怪
----scriptdata[3]标记进副本是否刷怪


function c3040075_OnEnterCombat(MapID, InstanceID, CreatureID)

	--cre.MonsterSay(MapID, InstanceID, CreatureID, 10025)
	local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    	--同步仇恨给范围为40个格子，高度为20的有方怪物
   	 g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 40, 20)
end

aux.RegisterCreatureEvent(3040075, 2, "c3040075_OnEnterCombat")






function c3040075_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 10064)
	local a = math.random(1,100)
	if a>50 then
		local x,y,z=unit.GetPosition(MapID, InstanceID,TargetID)
		map.MapCreateCreature(MapID, InstanceID, 3040090, x, y, z)
		map.MapCreateCreature(MapID, InstanceID, 3040090, x, y, z)
		map.MapCreateCreature(MapID, InstanceID, 3040090, x, y, z)
	end

	map.SetMapScriptData(MapID, InstanceID, 1, 1, 1)
	
	local bossdeath_2=map.GetMapScriptData(MapID,InstanceID,1,2)
	local bossdeath_3=map.GetMapScriptData(MapID,InstanceID,1,3)
	local bossdeath_4=map.GetMapScriptData(MapID,InstanceID,1,4)
	
	if bossdeath_2 == 1 and bossdeath_3 == 1 and bossdeath_4 == 1 then
		map.OpenCloseDoor(MapID, InstanceID, 4588, 1)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100086)
		--门打开
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100085)
		--门不开
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	end
	
		


end

aux.RegisterCreatureEvent(3040075, 4, "c3040075_OnDie")
