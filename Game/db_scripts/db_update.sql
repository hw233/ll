############################## 1.1.25 start #########################################

##2009-06-23
#account_common中添加AccountName字段
alter table `account_common` add column `AccountName` char (36)   NOT NULL  COMMENT '账号名' after `AccountID`;

############################## 1.1.25 end ###########################################

############################## 1.2.0 start ###########################################

##2009-07-06
#增加物品划拨表
CREATE TABLE `bill_item` (                 
             `token_id` char(50) NOT NULL COMMENT '令牌ID',          
             `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',  
             `item_id` int(10) unsigned NOT NULL COMMENT '物品ID',     
             `item_num` int(11) NOT NULL COMMENT '物品数量',             
             PRIMARY KEY (`token_id`)                 
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;       

##2009-07-06
#增加元宝划拨表
CREATE TABLE `bill_yuanbao` (                       
                `token_id` char(50) NOT NULL COMMENT '令牌ID',  
                `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',          
                `yuanbao` int(11) NOT NULL COMMENT '元宝数量',                       
                PRIMARY KEY (`token_id`)                          
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
              
##2009-07-07
#增加宠物数据表
CREATE TABLE `pet_data` (                                                                                       
	`pet_id` int(11) unsigned NOT NULL COMMENT '宠物id',                                                        
	`pet_name` varchar(32) NOT NULL COMMENT '宠物名称',                                                       
	`master_id` int(11) unsigned NOT NULL COMMENT '主人id',                                                     
	`type_id` int(11) unsigned NOT NULL COMMENT '原型id',                                                       
	`quality` tinyint(1) unsigned NOT NULL COMMENT '宠物品质',                                                
	`aptitude` tinyint(3) unsigned NOT NULL COMMENT '宠物资质',                                               
	`cur_spirit` int(4) NOT NULL COMMENT '当前灵力',                                                          
	`mountable` tinyint(1) NOT NULL COMMENT '能否骑乘',                                                       
	`cur_exp` int(20) NOT NULL DEFAULT '0' COMMENT '当前经验',                                                
	`step` tinyint(1) NOT NULL DEFAULT '0' COMMENT '阶',                                                         
	`grade` tinyint(1) NOT NULL DEFAULT '1' COMMENT '等',                                                        
	`talent_count` tinyint(4) NOT NULL DEFAULT '0' COMMENT '当前天资计数',                                  
	`wuxing_energy` tinyint(4) NOT NULL DEFAULT '0' COMMENT '五行力',                                          
	`pet_state` tinyint(1) NOT NULL DEFAULT '0' COMMENT '宠物状态',                                           
	PRIMARY KEY (`pet_id`)                                                                                        
) ENGINE=MyISAM DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='宠物表' ;

##2009-07-08
#增加VIP摊位表
CREATE TABLE `vip_stall` (                                       
             `StallID` tinyint(2) unsigned NOT NULL COMMENT 'VIP摊位序号',  
             `RoleID` int(10) unsigned NOT NULL COMMENT '所有者ID',         
             `RemainTime` int(10) NOT NULL COMMENT '剩余时间',              
             PRIMARY KEY (`StallID`)                                        
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

##2009-07-10
#宠物数据表增加潜能字段
alter table `pet_data` add column `potential` smallint(4) UNSIGNED NOT NULL COMMENT '当前潜能' after `aptitude`;

#2009-7-15 roledata表增加脚本数据blob
alter table roledata add column `ScriptData` blob NULL COMMENT '脚本数据' after `RemoveTime`;

##2009-7-17
#为装备增加潜力值影响系数
alter table `equip` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;
alter table `equip_baibao` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;
alter table `equip_del` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;

##2009-7-21
#增加宠物技能表格
create table `pet_skill`( 
	`petskill_typeid` int(11) NOT NULL COMMENT '宠物技能id', 
	`petid` int(11) NOT NULL COMMENT '宠物id', 
	`para1` int(11) ZEROFILL COMMENT '参数1', 
	`para2` int(11) ZEROFILL COMMENT '参数2', 
	PRIMARY KEY (`petskill_typeid`, `petid`)
	)  ;

##2009-7-22
#修改宠物数据表格
alter table `pet_data` drop column `mountable`;
						

############################## 1.2.0 end ############################################


############################## 1.2.1 start ##########################################
##2009-7-23
#修改角色名贴表
alter table `visiting_card` change `Level` `Level` tinyint(3) UNSIGNED default '1' NOT NULL comment '等级';

##2009-7-28
#修改宠物技能表
alter table `pet_skill` change `petskill_typeid` `petskill_typeid` int(11) UNSIGNED NOT NULL comment '宠物技能id', 
						change `petid` `petid` int(11) UNSIGNED NOT NULL comment '宠物id',
						change `para1` `para1` int(11) UNSIGNED NULL  comment '参数1', 
						change `para2` `para2` int(11) UNSIGNED NULL  comment '参数2';

#2009-7-29 roledata表增加角色开启宝箱计数
alter table roledata add column `TreasureSum` tinyint(3) unsigned default '0' NOT NULL COMMENT '角色开启宝箱数' after `ScriptData`;

##2009-08-03
#修改仓库保存金钱格式
alter table `account_common` drop column `WareGold`;
alter table `account_common` change `WareSilver` `WareSilver` int(20) default '0' NOT NULL;

#追加帮派属性字段
alter table `guild` add column `LeaderID` int(10) UNSIGNED NOT NULL COMMENT '帮主ID' after `FounderNameID`;
alter table `guild` add column `RemainSpreadTimes` int(3) DEFAULT '0' NOT NULL COMMENT '帮务发布剩余次数';
alter table `guild` add column `Commendation` bool DEFAULT '0' NOT NULL COMMENT '跑商嘉奖状态';

#追加帮派成员属性
alter table `guild_member` add column `CanUseGuildWare` bool DEFAULT '0' NOT NULL COMMENT '操作帮派仓库权限';

#创建帮派跑商容器表
CREATE TABLE `guild_commodity` (                                                                 
                   `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                                        
                   `guild_id` int(10) unsigned NOT NULL COMMENT '所在帮派ID',                                 
                   `role_level` smallint(3) unsigned NOT NULL DEFAULT '1' COMMENT '开始跑商时玩家等级',  
                   `tael` int(10) NOT NULL DEFAULT '0' COMMENT '当前商银数量',                              
                   `goods` tinyblob COMMENT '商货信息',                                                       
                   PRIMARY KEY (`role_id`)                                                                        
                 ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#创建帮派技能表
CREATE TABLE `guild_skill` (                                         
               `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',           
               `skill_id` int(10) unsigned NOT NULL COMMENT '技能ID',           
               `progress` int(10) unsigned NOT NULL COMMENT '升级进度',       
               `level` int(2) unsigned NOT NULL COMMENT '当前等级',           
               `researching` tinyint(1) NOT NULL COMMENT '当前正在研究中'  
             ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#创建帮派设施升级表
CREATE TABLE `guild_upgrade` (                                           
                 `guild_id` int(10) unsigned NOT NULL COMMENT '设施所属帮派',     
                 `type` int(2) unsigned NOT NULL COMMENT '帮派设施类型',          
                 `level` int(2) unsigned NOT NULL COMMENT '设施等级',               
                 `progress` int(10) unsigned NOT NULL COMMENT '升级进度',           
                 `item_type_1` int(10) unsigned NOT NULL COMMENT '需求物品类型',  
                 `item_neednum_1` int(10) unsigned NOT NULL COMMENT '需求的数量',  
                 `item_type_2` int(10) unsigned NOT NULL,                               
                 `item_neednum_2` int(10) unsigned NOT NULL,                            
                 `item_type_3` int(10) unsigned NOT NULL,                               
                 `item_neednum_3` int(10) unsigned NOT NULL,                            
                 `item_type_4` int(10) unsigned NOT NULL,                               
                 `item_neednum_4` int(10) unsigned NOT NULL                             
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
               
#创建帮派跑商排行榜表
CREATE TABLE `commerce_rank` (                                             
                 `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                  
                 `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',                 
                 `times` int(10) NOT NULL DEFAULT '0' COMMENT '完成跑商次数',       
                 `tael` int(10) NOT NULL DEFAULT '0' COMMENT '为帮派贡献的商银',  
                 PRIMARY KEY (`role_id`)                                                  
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
               
						
						alter table `blacklist` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '角色id', 
						change `BlackID` `BlackID` int(10) UNSIGNED NOT NULL comment '被角色id列入黑名单的id';


##2009-8-5
#增加数据库表注释
alter table `clan_data` change `RepXuanYuan` `RepXuanYuan` int(11) UNSIGNED default '0' NOT NULL comment '轩辕声望', 
						change `ConXuanYuan` `ConXuanYuan` int(11) UNSIGNED default '0' NOT NULL comment '轩辕贡献', 
						change `ActCntXuanYuan` `ActCntXuanYuan` tinyint(2) UNSIGNED default '0' NOT NULL comment '轩辕剩余激活次数', 
						change `RepShenNong` `RepShenNong` int(11) UNSIGNED default '0' NOT NULL comment '神农声望', 
						change `ConShenNong` `ConShenNong` int(11) UNSIGNED default '0' NOT NULL comment '神农贡献', 
						change `ActCntShenNong` `ActCntShenNong` tinyint(2) UNSIGNED default '0' NOT NULL comment '神农剩余激活次数', 
						change `RepFuXi` `RepFuXi` int(11) UNSIGNED default '0' NOT NULL comment '伏羲声望', 
						change `ConFuXi` `ConFuXi` int(11) UNSIGNED default '0' NOT NULL comment '伏羲贡献', 
						change `ActCntFuXi` `ActCntFuXi` tinyint(2) UNSIGNED default '0' NOT NULL comment '伏羲剩余激活次数', 
						change `RepSanMiao` `RepSanMiao` int(11) UNSIGNED default '0' NOT NULL comment '三苗声望', 
						change `ConSanMiao` `ConSanMiao` int(11) UNSIGNED default '0' NOT NULL comment '三苗贡献', 
						change `ActCntSanMiao` `ActCntSanMiao` tinyint(2) UNSIGNED default '0' NOT NULL comment '三苗剩余激活次数', 
						change `RepJiuLi` `RepJiuLi` int(11) UNSIGNED default '0' NOT NULL comment '九黎声望', 
						change `ConJiuLi` `ConJiuLi` int(11) UNSIGNED default '0' NOT NULL comment '九黎贡献', 
						change `ActCntJiuLi` `ActCntJiuLi` tinyint(2) UNSIGNED default '0' NOT NULL comment '九黎剩余激活次数', 
						change `RepYueZhi` `RepYueZhi` int(11) UNSIGNED default '0' NOT NULL comment '月氏声望', 
						change `ConYueZhi` `ConYueZhi` int(11) UNSIGNED default '0' NOT NULL comment '月氏贡献', 
						change `ActCntYueZhi` `ActCntYueZhi` tinyint(2) UNSIGNED default '0' NOT NULL comment '月氏剩余激活次数', 
						change `RepNvWa` `RepNvWa` int(11) UNSIGNED default '0' NOT NULL comment '女娲声望', 
						change `ConNvWa` `ConNvWa` int(11) UNSIGNED default '0' NOT NULL comment '女娲贡献', 
						change `ActCntNvWa` `ActCntNvWa` tinyint(2) UNSIGNED default '0' NOT NULL comment '女娲剩余激活次数', 
						change `RepGongGong` `RepGongGong` int(11) UNSIGNED default '0' NOT NULL comment '共工声望', 
						change `ConGongGong` `ConGongGong` int(11) UNSIGNED default '0' NOT NULL comment '共工贡献', 
						change `ActCntGongGong` `ActCntGongGong` tinyint(2) UNSIGNED default '0' NOT NULL comment '共工剩余激活次数';
						
alter table `enemy` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '玩家id', 
					change `EnemyID` `EnemyID` int(10) UNSIGNED NOT NULL comment '仇敌id';

alter table `friend` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '玩家id', 
					change `FriendID` `FriendID` int(10) UNSIGNED NOT NULL comment '好友id', 
					change `GroupID` `GroupID` tinyint(2) NOT NULL comment '好友所属群组';
					
alter table `item` change `SerialNum` `SerialNum` bigint(20) NOT NULL comment '64位id', 
					change `Num` `Num` smallint(5) default '1' NOT NULL comment '数量', 
					change `TypeID` `TypeID` int(10) UNSIGNED NOT NULL comment '类型id', 
					change `Bind` `Bind` tinyint(4) NOT NULL comment '是否绑定', 
					change `CreatorID` `CreatorID` int(10) UNSIGNED default '4294967295' NOT NULL comment '创建者id';
					
alter table `item_cdtime` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '角色id', 
					change `CDTime` `CDTime` blob NULL  comment '冷却时间blob，格式为（typeid，冷却时间）';
					
alter table `task` change `Monster0Num` `Monster0Num` int(10) default '0' NOT NULL comment '已打怪数量1', 
					change `StartTime` `StartTime` int(10) UNSIGNED default '4294967295' NOT NULL comment '开始时间', 
					change `ScriptData` `ScriptData` blob NULL  comment '脚本数据', 
					change `DynamicTarget` `DynamicTarget` blob NULL ;
					
alter table `task_done` change `TaskID` `TaskID` int(10) UNSIGNED NOT NULL comment '任务id', 
					change `Times` `Times` smallint(6) UNSIGNED NOT NULL comment '一个时期内完成的任务次数', 
					change `StartTime` `StartTime` int(10) UNSIGNED default '4294967295' NOT NULL comment '第一个任务开始时间';

alter table `visiting_card` change `City` `City` char(6) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '城市';

##2009-8-5
#增加金牌网吧数据库表
CREATE TABLE `vip_netbar` (                                                                                  
              `accountid` int(11) unsigned NOT NULL COMMENT '账号id',                                                  
              `login_time` int(11) unsigned NOT NULL COMMENT '上次在金牌网吧登录时间',                        
              PRIMARY KEY (`accountid`)                                                                                  
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='金牌网吧'  ;

##2009-8-11
#固定活动脚本数据
create table `activity` (    `dwID` int (10)   NOT NULL ,  
			`ScriptData` blob   NULL  , 
			PRIMARY KEY ( `dwID` )  
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='固定活动数据';      

##2009-8-20
#修改宠物表
alter table `pet_data` change `potential` `potential` int(4) UNSIGNED NOT NULL comment '当前潜能';

##2009-9-23
#修改帮派默认值
alter table `guild` change `HoldCity0` `HoldCity0` tinyint(3) UNSIGNED default '0' NOT NULL comment '帮派当前辖属城市编号';
alter table `guild` change `HoldCity1` `HoldCity1` tinyint(3) UNSIGNED default '0' NOT NULL;
alter table `guild` change `HoldCity2` `HoldCity2` tinyint(3) UNSIGNED default '0' NOT NULL;
update guild set HoldCity0=0,HoldCity1=0,HoldCity2=0;

############################## 1.2.1 end ############################################

############################## 1.2.2 start ##########################################


#增加角色摊位成长所需字段
alter table `roledata` add column `StallLevel` tinyint(3) UNSIGNED DEFAULT '1' NOT NULL COMMENT '摊位等级';
alter table `roledata` add column `StallDailyExp` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '摊位当天经验';
alter table `roledata` add column `StallCurExp` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '摊位当前等级经验';
alter table `roledata` add column `StallLastTime` int(10) UNSIGNED DEFAULT '4325376' NOT NULL COMMENT '摊位最近更新时间(默认2000年1月1日)';

##2009-09-11
#添加宠物锁定字段
alter table `pet_data` add column `pet_lock` bool DEFAULT '0' NOT NULL COMMENT '宠物锁定' after `pet_state`;


##2009-09-16
#留言表
CREATE TABLE `left_msg` (                                                                                    
            `msg_id` int(11) unsigned NOT NULL COMMENT '留言日期时间',                                           
            `roleid` int(11) unsigned NOT NULL COMMENT '玩家id',                                                     
            `msg_data` tinyblob COMMENT '留言内容',                                                                
            PRIMARY KEY (`msg_id`,`roleid`)                                                                            
          ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='留言数据' ;
          
##2009-09-21
#删除equip_del主键
Alter table `equip_del` drop primary key;

##2009-09-28
#修改角色职业和英雄职业的默认值为0
alter table `roledata` drop column `Class`; 
alter table `roledata` add column `Class` tinyint (3) DEFAULT '1' NOT NULL after `RebornMapID`;
alter table `roledata` drop column `ClassEx`; 
alter table `roledata` add column `ClassEx` tinyint (3) DEFAULT '0' NOT NULL after `Class`;

##2009-10-14
#修改角色职业和英雄职业的默认值为0
alter table `visiting_card` change `HeadUrl` `HeadUrl` varchar(100) character set utf8 collate utf8_general_ci NULL  comment '头像url';

#2009-10-14
# rebuild table 'item_del'
drop table `item_del`;
CREATE TABLE `item_del` LIKE `item`;
Alter table `item_del` drop primary key;

#2009-10-19
# 增加一个新的天资字段
alter table `roledata` add column `TalentType4` tinyint(2) NOT NULL default '-1' COMMENT '天资类型' after `TalentType3`;
alter table `roledata` add column `TalentVal4` smallint(4) NOT NULL default '0' COMMENT '天资投放值' after `TalentVal3`;

#2009-10-19
# 增加元宝划拨日志表
CREATE TABLE `bill_yuanbao_log` (
  `token_id` char(50) NOT NULL DEFAULT '' COMMENT '令牌ID',
  `account_id` int(11) unsigned DEFAULT NULL,
  `yuanbao` int(11) DEFAULT NULL,
  `time` char(50) DEFAULT NULL,
  PRIMARY KEY (`token_id`),
  UNIQUE KEY `account_id` (`account_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='元宝划拨log';  

##2009-10-20
#修改角色名贴中
update visiting_card set HeadUrl=null where HeadUrl='N/A';

##2009-10-26
#修改宠物表中资质的范围
alter table `pet_data` change `aptitude` `aptitude` int(4) UNSIGNED NOT NULL comment '宠物资质';

##2009-10-29
#修改索引属性去掉unique限制
alter table `bill_yuanbao_log` drop key `account_id`, add key `account_id` ( `account_id` );

##2009-10-30
#增加item划拨log
CREATE TABLE `bill_item_log` (                 
 `token_id` char(50) NOT NULL COMMENT '令牌ID',          
 `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',  
 `item_id` int(10) unsigned NOT NULL COMMENT '物品ID',     
 `item_num` int(11) NOT NULL COMMENT '物品数量',
 `time` char(50) DEFAULT NULL,             
 PRIMARY KEY (`token_id`),
 KEY `account_id` (`account_id`)               
) ENGINE=MyISAM DEFAULT CHARSET=utf8; 

#删除划拨表(会清除之前的数据,慎用!!!)
drop table `bill_item_log`;
drop table `bill_yuanbao_log`;
drop table `bill_item`;
drop table `bill_yuanbao`;

#增加中央划拨表
CREATE TABLE `centralbilling` (                                    
                  `TOKENID` int(11) NOT NULL COMMENT '令牌ID',                   
                  `USERNAME` varchar(50) NOT NULL COMMENT '帐号',                
                  `WORLDID` int(11) DEFAULT NULL COMMENT '服ID',                  
                  `ZONEID` int(11) NOT NULL COMMENT '区ID',                       
                  `YUANORITEM` tinyint(1) NOT NULL COMMENT '0 元宝；1 道具',  
                  `ITEMCODE` int(11) DEFAULT NULL COMMENT '道具编号',          
                  `QTY` int(11) NOT NULL DEFAULT '0' COMMENT '数量',             
                  `STATE` int(11) NOT NULL DEFAULT '0' COMMENT '状态',           
                  `INSERTDATE` datetime NOT NULL COMMENT '插入时间',           
                  PRIMARY KEY (`TOKENID`)                                          
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE TABLE `centralbilling_log` LIKE `centralbilling`;

############################## 1.2.2 end ############################################

############################## 1.2.3 start ##########################################
##2009-11-5
alter table `roledata` change `CloseSGTime` `CloseSGTime` char (20)  DEFAULT '2000-01-01 00:00:00' NOT NULL  COMMENT '上次关闭PK保护的时间'

############################## 1.2.3 end ############################################

############################## 1.2.4 start ##########################################

##2009-11-18
#修改了宠物表中几个字段的类型
alter table `pet_data` change `talent_count` `talent_count` int(4) default '0' NOT NULL, change `wuxing_energy` `wuxing_energy` int(4) default '0' NOT NULL, change `pet_state` `pet_state` int(1) default '0' NOT NULL;
#数据库增加索引
ALTER TABLE `item_del` ADD KEY (`SerialNum`);
ALTER TABLE `yuanbaoorder` ADD KEY (`OrderMode`);
ALTER TABLE `yuanbaoorder` ADD KEY (`RoleID`);
ALTER TABLE `guild_upgrade` ADD KEY (`guild_id`);	
ALTER TABLE `guild_skill` ADD KEY (`guild_id`);
ALTER TABLE `guild_commodity` ADD KEY (`guild_id`);
ALTER TABLE `commerce_rank` ADD KEY (`guild_id`);
ALTER TABLE `roledata` ADD KEY (`RemoveFlag`);
ALTER TABLE `item_needlog` ADD KEY (`NeedLog`);
ALTER TABLE `group_purchase` ADD KEY (`GuildID`);
ALTER TABLE `group_purchase` ADD KEY (`RoleID`);
ALTER TABLE `group_purchase` ADD KEY (`MallID`);
ALTER TABLE `pet_data` ADD KEY (`master_id`);
ALTER TABLE `yuanbaoorder` ADD KEY (`StartTime`);
ALTER TABLE `account` ADD KEY (`worldname_crc`);
ALTER TABLE `roledata` ADD KEY (`rolename`);
ALTER TABLE `account_common` ADD KEY (`AccountName`);
ALTER TABLE `task_done` ADD KEY (`RoleID`);

############################## 1.2.4 end ############################################
############################## 1.2.5 start ##########################################
#2009-11-23 roledata表增加角色戾气值
alter table roledata add column `Hostility` tinyint(1) unsigned default '0' NOT NULL COMMENT '戾气值' after `StallLastTime`;

#2009-11-26 roledata表增加关闭玩家戾气为0时回城复活所加上的pk保护倒计tick
alter table roledata add column `DeadUnSetSafeGuardCountDown` smallint(4) unsigned default '0' NOT NULL COMMENT '关闭玩家戾气为0时回城复活所加上的pk保护倒计tick' after `Hostility`;

#2009-11-28 roledata表增加记录式传送符
alter table roledata add column  `ItemTransportMapID` int(10) unsigned NOT NULL DEFAULT '3017298127' COMMENT '记录式传送符mapid' after `DeadUnSetSafeGuardCountDown`;
alter table roledata add column  `ItemTransportX` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符X坐标2200*50' after `ItemTransportMapID`;
alter table roledata add column  `ItemTransportZ` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符Z坐标2200*50' after `ItemTransportX`;
alter table roledata add column  `ItemTransportY` float unsigned NOT NULL DEFAULT '332300' COMMENT '记录式传送符Y坐标6646*50' after `ItemTransportZ`;

alter table roledata change `ItemTransportX` `ItemTransportX` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符X坐标2200*50';
alter table roledata change `ItemTransportZ` `ItemTransportZ` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符Z坐标2200*50';
alter table roledata change `ItemTransportY` `ItemTransportY` float unsigned NOT NULL DEFAULT '332300' COMMENT '记录式传送符Y坐标6646*50';

#2009-12-15 account_common 修改库银类型
alter table `account_common` change `WareSilver` `WareSilver` bigint(20) default '0' NOT NULL;

#2009-12-17 roledata表修改DeadUnSetSafeGuardCountDown字段
alter table roledata change `DeadUnSetSafeGuardCountDown` `DeadUnSetSafeGuardCountDown` smallint(5) signed NOT NULL default '-100' COMMENT '关闭玩家戾气为0时回城复活所加上的pk保护倒计tick';
############################## 1.2.5 end ############################################



############################## 1.3.0 begin ############################################
#2009-09-17
#添加夫妻字段
alter table `roledata` add column `LoverID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '爱人对应的roleid没结婚就是4294967295'after `ItemTransportY`;
alter table `roledata` add column `HaveWedding` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否举行过婚礼' after `LoverID`;

# 装备增加潜力值提升次数
alter table `equip` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
alter table `equip_baibao` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
alter table `equip_del` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
##2009-12-03
#roledata表中增加实力值和实力排名字段
alter table `roledata` add column `Strength` mediumint(8) DEFAULT '0' NOT NULL COMMENT '实力值' after `AgilityAdded`;
alter table `roledata` add column `StrengthNum` smallint(3) DEFAULT '0' NOT NULL COMMENT '实力排名' after `Strength`;
alter table `roledata` add column `Heroism` mediumint(8) DEFAULT '0' NOT NULL COMMENT '英雄值' after `StrengthNum`;
alter table `roledata` add column `Wit` mediumint(8) DEFAULT '0' NOT NULL COMMENT '谋略值' after `Heroism`;
alter table `roledata` add column `Errantry` mediumint(8) DEFAULT '0' NOT NULL COMMENT '侠义值' after `Wit`;
alter table `roledata` add column `Valor` mediumint(8) DEFAULT '0' NOT NULL COMMENT '政战值' after `Errantry`;

# 创建实力排行记录表
CREATE TABLE `strength_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Strength` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '实力值',
	`RoleName` varchar(32) NOT NULL COMMENT '角色名字', 
	PRIMARY KEY  (`RoleName`)
	);

# 装备紫色品级鉴定几率
alter table `equip` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
alter table `equip_baibao` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
alter table `equip_del` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
# 2009-12-06
# 清空宠物技能冷却时间
update pet_skill set para1=4294967295 where para1<>4294967295;

# Jason 2009-12-7 增加神龙赐福，相关字段在roledata表内。
alter table `roledata` add column `LastLessingTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上次领取神龙赐福的时间' after `HaveWedding`;
alter table `roledata` add column `LastLessingLevel` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '领取到了多少级' after `LastLessingTime`;

alter table `roledata` add column `NeedPrisonRevive` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否需要牢狱复活' after `LastLessingLevel`;

#
alter table `roledata` change `DeadUnSetSafeGuardCountDown` `DeadUnSetSafeGuardCountDown` smallint(5) default '-100' NOT NULL;

############################## 1.3.0 end   ############################################



############################## 1.3.1 begin ############################################
# Jason 外部链接表
/*Table structure for table `external_links` */

DROP TABLE IF EXISTS `external_links`;

CREATE TABLE `external_links` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `LinkName` varchar(10) NOT NULL DEFAULT '"NoName"' COMMENT '??????',
  `LinkURL` varchar(256) DEFAULT NULL COMMENT '????URL',
  PRIMARY KEY (`ID`),
  KEY `LinkName` (`LinkName`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;


# 名帖中增加更名记录字段
alter table visiting_card add column `NameHistory` varchar(500) NOT NULL DEFAULT 'N/A' COMMENT '更名记录' after `Signature`;

# 记录改名记录的表  
CREATE TABLE `name_history` (               
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`name_1` varchar(32) NOT NULL DEFAULT '0',
	`name_2` varchar(32) NOT NULL DEFAULT '0',
	`name_3` varchar(32) NOT NULL DEFAULT '0',
	`name_4` varchar(32) NOT NULL DEFAULT '0',
	`name_5` varchar(32) NOT NULL DEFAULT '0',
	`name_6` varchar(32) NOT NULL DEFAULT '0',
	`name_7` varchar(32) NOT NULL DEFAULT '0',
	`name_8` varchar(32) NOT NULL DEFAULT '0',
	`name_9` varchar(32) NOT NULL DEFAULT '0',
	`name_10` varchar(32) NOT NULL DEFAULT '0',
	PRIMARY KEY  (`RoleID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='该表记录角色最近使用的十个名字'; 


# 为声望表clan_data中增加字段 TotalFame	和 FamePm
alter table clan_data add column TotalFame int(20) UNSIGNED DEFAULT '0' NOT NULL COMMENT '各氏族声望总和' after RoleID;
alter table clan_data add column FamePm smallint(3) UNSIGNED DEFAULT '0' NOT NULL COMMENT '各氏族声望总和' after TotalFame;

# 在roledata表中增加名次字段
alter table roledata add column LevelPm smallint(3) UNSIGNED DEFAULT '0' NOT NULL COMMENT '等级排名' after Level;

# 创建声望排行记录表
CREATE TABLE `fame_rankings` (                     
	`Num` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Fame` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '声望',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	KEY  (`Num`)
	);

# 创建等级排行记录表
CREATE TABLE `level_rankings` (                     
	`Num` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Level` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '等级',
	`RoleName` varchar(32) NOT NULL COMMENT '角色名字', 
	KEY  (`Num`)
	);

# 创建宠物排行记录表
CREATE TABLE `pet_rankings` (                     
	`Num` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Pet` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '宠物实力值',
	`PetName` varchar(32) NOT NULL COMMENT '宠物名字', 
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	KEY  (`Num`)
	);

		
# 在roledata表中增加装备实力字段
#alter table roledata add column EquipValue mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备实力值' after Valor;	
#alter table roledata add column EquipPm smallint(3) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备实力排名' after EquipValue;	

# 在pet_data表中增加宠物实力和排名字段
alter table pet_data add column pet_value mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '宠物实力值' after pet_name;	
alter table pet_data add column pet_pm smallint(3) UNSIGNED DEFAULT '0' NOT NULL COMMENT '宠物实力排名' after pet_value;	

# 在pet_skill表中增加索引
alter table `pet_skill` add index `petid` (`petid`);
alter table `pet_skill` engine = MyISAM;

# 创建装备实力排行记录表
CREATE TABLE `equip_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Equip` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '装备实力值',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	KEY (`Num`)
	);
	
# 在roledata表中增加装备实力字段
alter table roledata add column EquipValue mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备实力值' after Valor;	
alter table roledata add column EquipPm smallint(3) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备实力排名' after EquipValue;		

# 表clan_data中添加字段，标识改角色是否被删除
alter table clan_data add column RemoveFlag tinyint(1) NOT NULL default '0' COMMENT '是否处于删除状态' after TotalFame;

alter table fame_rankings add column RoleName varchar(32) COMMENT '角色名字' after RoleID;

# roledata表中增加一个字段-最近一次升级的时间，用于等级排名
alter table roledata add column LastUpgrade int(10) unsigned NOT NULL default '0' COMMENT '最近一次升级的时间，DWORD格式' after LevelPm;

alter table roledata add column	WorkedLevel bigint(20) unsigned NOT NULL DEFAULT '1' COMMENT '加成过的等级值，用于等级排名' after Level;
alter table level_rankings drop column TrueLevel;
alter table level_rankings drop column Level;
alter table level_rankings add column Level bigint(20) unsigned NOT NULL DEFAULT '1' COMMENT '加成过的等级值，用于等级排名' after PreNum;

# 在roledata表中删除就有的实力系统相关字段
alter table roledata drop column Heroism;
alter table roledata drop column Wit;
alter table roledata drop column Errantry;
alter table roledata drop column Valor;


#  城市	
DROP TABLE IF EXISTS `city`;

CREATE TABLE `city` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '城市id',
  `guild_id` int(10) unsigned DEFAULT '4294967295' COMMENT '帮派id',
  `defence` int(10) DEFAULT '3000' COMMENT '防御度',
  `eudemon_tally` int(10) DEFAULT '0' COMMENT '守护神契合度',
  `tax_rate` int(10) DEFAULT '0' COMMENT '税率',
  `tax_rate_time` int(10) unsigned DEFAULT '0' COMMENT '最近一次更新税率的时间',
  `taxation` int(10) DEFAULT '0' COMMENT '税金',
  `prolificacy` int(10) DEFAULT '500' COMMENT '生产力',
  `signup_list` blob COMMENT '报名列表',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='城市';
			
# 宠物排行榜表中增加宠物ID字段
alter table pet_rankings add column PetID int(11) unsigned NOT NULL COMMENT '宠物id' after Pet;
			
#宠物删除标志
alter table `pet_data` add column `RemoveFlag` bool DEFAULT '0' NULL COMMENT '删除标志位' after `pet_lock`;
	
	
#游戏设置 游戏选项
alter table `account_common` add column `PersonalSet` blob COMMENT '游戏设置 游戏选项';

#排行
alter table `fame_rankings` ENGINE = MYISAM;
alter table `fame_rankings` DEFAULT CHARACTER SET utf8;
alter table `fame_rankings` change `RoleName` RoleName  varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '角色名字';

alter table `level_rankings` ENGINE = MYISAM;
alter table `level_rankings` DEFAULT CHARACTER SET utf8;
alter table `level_rankings` change `RoleName` RoleName  varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '角色名字';
	
alter table `pet_rankings` ENGINE = MYISAM;
alter table `pet_rankings` DEFAULT CHARACTER SET utf8;
alter table `pet_rankings` change `PetName` PetName  varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '宠物名称';
	
alter table `equip_rankings` ENGINE = MYISAM;
alter table `equip_rankings` DEFAULT CHARACTER SET utf8;
		
alter table `strength_rankings` ENGINE = MYISAM;
alter table `strength_rankings` DEFAULT CHARACTER SET utf8;
alter table `strength_rankings` change `RoleName` RoleName  varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '角色名';
			
alter table `fame_rankings` drop column RoleName;

create table fame_rankings_pre like fame_rankings;
create table equip_rankings_pre like equip_rankings;
create table level_rankings_pre like level_rankings;
create table strength_rankings_pre like strength_rankings;
create table pet_rankings_pre like pet_rankings;


############################## 1.3.1 end   ############################################



############################## 1.3.2 begin ############################################
# Jason 2010-1-9 v1.3.2 表roledata增加在线每日在线奖励相关字段
alter table roledata add column `LastLessingLoongDate` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '最后一次神龙赐福时间' after `NeedPrisonRevive`;
alter table roledata add column `CurrentDayOnlineTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '当日在线时间' after `LastLessingLoongDate`;
alter table roledata add column `OneDayFirstLoginedTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '当日首次登陆时间' after `CurrentDayOnlineTime`;
alter table roledata add column `OnlineRewardPerDayRecTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '当日领取在线奖励时间' after `OneDayFirstLoginedTime`;

# 在item表中增加物品锁定相关的字段
alter table item add column UnlockTime int(10) NOT NULL DEFAULT '0' COMMENT '物品被解锁的三日之内，记录解锁的时间，其他情况下该时间为0' after LockStat;

# Jason 2010-1-15 v1.3.2 roledata表中增加离线挂机相关字段
alter table roledata add column `OfflineExperienceRewardFlag` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '是否离线挂机' after `OnlineRewardPerDayRecTime`;

# 装备署名相关字段
alter table equip add column `Signed` tinyint(2) NOT NULL default '0' COMMENT '是否被署名' after PotValModPct;
alter table equip add column `Signature` varchar(10) COMMENT '署名内容' after Signed;
alter table equip add column `SignRoleID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '署名者id' after Signature;  

# Jay 2010-1-15 v1.3.2 表roledata增加精力值字段
alter table roledata add column `Spirit` mediumint(8) NOT NULL default '300' COMMENT '精力' after `VIPPoint`;
alter table roledata add column `FixSpirit` bigint(20) NOT NULL default '0' COMMENT '精力冷却时间' after `Spirit`;

# 由于equip表修改，equip_baibao和equip_del做相应的修改
alter table equip_baibao add column `Signed` tinyint(2) NOT NULL default '0' COMMENT '是否被署名' after PotValModPct;
alter table equip_baibao add column `Signature` varchar(10) COMMENT '署名内容' after Signed;
alter table equip_baibao add column `SignRoleID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '署名者id' after Signature; 

alter table equip_del add column `Signed` tinyint(2) NOT NULL default '0' COMMENT '是否被署名' after PotValModPct;
alter table equip_del add column `Signature` varchar(10) COMMENT '署名内容' after Signed;
alter table equip_del add column `SignRoleID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '署名者id' after Signature; 

# 实力系统-为角色增加可消耗实力值属性
alter table roledata add column ConsumptiveStrength mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '角色可消耗实力值' after EquipPm;
alter table roledata add column KillCount mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '角色击杀的非白名玩家数' after ConsumptiveStrength;
alter table roledata add column AdvanceStrength mediumint(8) UNSIGNED DEFAULT '0' NOT NULL COMMENT '角色进阶实力值' after EquipPm;
# 增加师徒字段
alter table roledata add column `MasterID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '师傅ID' after `OnlineRewardPerDayRecTime`;

alter table roledata add column `JingWuPoint` mediumint(8) NOT NULL default '0' COMMENT '精武点数' after `MasterID`;
alter table roledata add column `QinWuPoint` mediumint(8) NOT NULL default '0' COMMENT '勤武点数' after `JingWuPoint`;

# 师徒数据
create table `master` (   
		 `dwID` int (10)   NOT NULL ,  
		 `JingWuPoint` mediumint(8)  unsigned DEFAULT '0' COMMENT '精武点数',
		 `QinWuPoint` mediumint(8)   unsigned DEFAULT '0' COMMENT '勤武点数',
		 `ApprenticeData`  blob COMMENT '徒弟ID',
		 PRIMARY KEY ( `dwID` )  
		) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='师门数据';    
		
# Jason 2010-1-27 v1.3.2 记录账号登入/登出时间等信息，玩家回归/离线奖励使用
alter table `account_common` add column `LastLoginTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '该账号上次登入时间';                        
alter table `account_common` add column `LastLogoutTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '该账号上次登出时间';                       
alter table `account_common` add column `LastReceiveDailyOfflineRewardTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上次领取离线奖励时间';  
alter table `account_common` add column `LastReceiveRegressionTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上次领取回归奖励时间';          

# item表中添加字段，item_del和item_baibao随之更新
alter table item_del add column UnlockTime int(10) NOT NULL DEFAULT '0' COMMENT '物品被解锁的三日之内，记录解锁的时间，其他情况下该时间为0' after LockStat;
alter table item_baibao add column UnlockTime int(10) NOT NULL DEFAULT '0' COMMENT '物品被解锁的三日之内，记录解锁的时间，其他情况下该时间为0' after LockStat;


#角色相关宠物带容量
alter table roledata add column `PetPocketValve` smallint(6) unsigned NOT NULL DEFAULT '5' COMMENT '宠物带容量';

#仙界技能修炼
alter table roledata add column `MiraclePrcNum` smallint(6) unsigned NOT NULL DEFAULT '100' COMMENT '仙界技能修炼点儿';                                           
alter table roledata add column `MiracleResetTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '仙界技能修炼点儿重置时间';

# Jason 2010-2-7 增加外部链接数据
delete from external_links;
alter table `external_links` change `LinkName` `LinkName`  varchar(30)   NOT NULL DEFAULT 'NoName';        
alter table `external_links` change `LinkURL`  `LinkURL` varchar(512) DEFAULT NULL;
insert into external_links (linkname,linkurl ) values( '攻略站首页', 'http://www.loong3d.com/info/index.html' );
insert into external_links (linkname,linkurl ) values( '新手入门专题', 'http://loong.17173.com/zt/xs/index.shtml');
insert into external_links (linkname,linkurl ) values( '日常活动专题', 'http://loong.17173.com/zt/act/index.html');
insert into external_links (linkname,linkurl ) values( '天资加点模拟器', 'http://zq.17173.com/loong/simulator/');
insert into external_links (linkname,linkurl ) values( '属性加点模拟器', 'http://games.sina.com.cn/o/z/loong/2009-11-30/1011357027.shtml');
insert into external_links (linkname,linkurl ) values( '玩家问题互助' ,'http://bk.17173.com/bkfactory/17173bk_zt/loong/loong_zt.html');
# 增加角色的禁言标识位
alter table roledata add column `SpeakOff` tinyint(3) NOT NULL DEFAULT '0' COMMENT '玩家是否被禁言，1为是';                       

#台湾金牌网吧红利点
CREATE TABLE `bonus` (
  `datetime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '插入时间',
  `accountname` varchar(36) DEFAULT NULL COMMENT '账号名',
  `charname` varchar(36) DEFAULT NULL COMMENT '用户名',
  `ip` varchar(20) DEFAULT NULL COMMENT '玩家的ip',
  `bonuspoint` int(10) DEFAULT NULL COMMENT '红利点'
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#选择人物界面默认选择
alter table `account_common` add column `LastUseRoleID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '该用户上一次选择的角色id' after `LastReceiveRegressionTime`;

#快捷栏保存服务器

CREATE TABLE `shortcut_quickbar` (               
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`quickbardata` blob COMMENT '快捷栏数据',
	PRIMARY KEY  (`RoleID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 

# 增加徒弟升级积分
alter table master add column `LevelUpBonus` mediumint(8) NOT NULL DEFAULT '0' COMMENT '徒弟升级积分' after`QinWuPoint`;   
alter table master add column `RewardNum` tinyint(3) NOT NULL DEFAULT '0' COMMENT '出师奖励次数' after`LevelUpBonus`;  
############################## 1.3.2 end   ############################################

############################## 1.4.0 begin   ############################################
# 增加是否有师徒关系字段
alter table `roledata` add column `MARelation` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否有师徒关系' after `QinWuPoint` ;

# Jay 2010-4-2 添加师徒索引
alter table `roledata` change column `MARelation` `MARelation` int(10) NOT NULL DEFAULT '0' COMMENT '师徒总数量';
ALTER TABLE `roledata` ADD KEY (`masterid`);
update roledata set MARelation=0;
update roledata set MARelation=1 where masterid!=4294967295;
update roledata a set MARelation=MARelation + (select count(0) from (select masterid from roledata) b  where b.masterid=a.roleid);

############################## 1.4.0 end   ############################################

############################## 1.5.0 begin   ############################################

# 龙之试炼系统
alter table roledata add column `TrainStateTable` blob NOT NULL COMMENT '试炼状态表';

# Added by Jay, 2010-3-8
alter table roledata add column `WuXun` int(10) NOT NULL default '0' COMMENT '武勋' after `FixSpirit`;
alter table roledata add column `WuJi` int(10) NOT NULL default '0' COMMENT '武技' after `WuXun`;
# Jason 2010-3-15 v1.5.0神龙赐福扩展
alter table roledata add column `LastLessingLevelEx` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '领取到了多少级';

alter table pet_data add column `birthday` bigint(20) NOT NULL COMMENT '宠物创建时间';
alter table pet_data add column `live` tinyint(2) NOT NULL default '1' COMMENT '宠物生存状态';
alter table pet_data add column `lifeadded` smallint(3) unsigned NOT NULL DEFAULT '0' COMMENT '附加寿命';

# Added by zxj, 2010-3-9
#title表增加 剩余时间 字段,0为非限时称号，-1为限时称号时间用完
alter table `title` add column `TimeRemain` int(1) default '0' COMMENT '剩余时间' after `Count`;

alter table roledata change column `TrainStateTable` `TrainStateTable` blob COMMENT '试炼状态表';

# 竞技场

CREATE TABLE `arena_data` (                                                                
              `serial_id` int(10) unsigned NOT NULL  COMMENT '序号',                        
              `system_id` int(10) unsigned NOT NULL  COMMENT '竞技场系统ID',                
              `serial_val` int(10) unsigned NOT NULL COMMENT '序列号值',                   
              `time_range` varchar(32) NOT NULL  COMMENT '保存的时间段',  
              `hall_persom_num` int(10) unsigned NOT NULL  COMMENT '大厅总人数',            
              `hall_room_num` int(10) unsigned NOT NULL    COMMENT '大厅房间数',              
              `fighting_room_num` int(10) unsigned NOT NULL COMMENT '战斗状态的房间数',    
              `below120Second` int(10) unsigned NOT NULL  COMMENT '低于120秒的场数',          
              `below210Second` int(10) unsigned NOT NULL  COMMENT '低于210秒的场数',        
              `below300Second` int(10) unsigned NOT NULL  COMMENT '低于300秒的场数',        
              `over300Second` int(10) unsigned NOT NULL   COMMENT '高于300秒的场数',         
              PRIMARY KEY (`serial_id`)                                                                
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8  ;

# 龙之试炼
CREATE TABLE `train_state`(
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`TrainID` int(10) unsigned NOT NULL COMMENT '试炼ID',
	`TrainState` tinyint(2) unsigned NOT NULL  default '0' COMMENT '试炼状态(0:未开启; 1:进行中; 2:已通过(已经领取完奖励); 3:已完成(没有领取奖励))',
	PRIMARY KEY (`RoleID`,`TrainID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='已经开启的龙之试炼状态表'; 

# 标识是不是已经做过试炼状态表的数据转移
alter TABLE roledata add column `TrainStateTransfered` tinyint(2) unsigned NOT NULL default '0' COMMENT '试炼状态表做过数据转移(0:没有转移过;1:已经转移过)';


############################## 1.5.0 end   ############################################

############################## 1.5.1 begin   ############################################
# 小龙女查询表

CREATE TABLE `dragongirl_data` (                                                                
              `sign_id` int(10) unsigned NOT NULL  COMMENT '报名号',                        
              `role_id` int(10) unsigned NOT NULL  COMMENT '角色ID',                
              `role_name` varchar(32) NOT NULL  COMMENT '角色名字',  
              `point` int(10) unsigned NOT NULL COMMENT '得分',   
              `serial_id` int(10) unsigned NOT NULL COMMENT '序号ID',  
              `data_count` int(10) unsigned NOT NULL COMMENT '记录的条数',          
              PRIMARY KEY (`serial_id`)                                                                
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8   ;

# 龙卫士查询表

CREATE TABLE `dragondefender_data` (                                                                                    
              `serial_id` int(10) unsigned NOT NULL  COMMENT '序列号ID', 
              `role_id` int(10) unsigned NOT NULL  COMMENT '角色ID',                
              `role_name` varchar(32) NOT NULL  COMMENT '角色名字',  
              `point` int(10) unsigned NOT NULL COMMENT '得分', 
              `data_count` int(10) unsigned NOT NULL COMMENT '记录的条数',
                               
              PRIMARY KEY (`role_id`,`serial_id`)                                                                
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;


alter table roledata add column `GodMiraclePoints` int(10) NOT NULL DEFAULT '0' COMMENT '神迹争夺积分';


############################## 1.5.1 end   ############################################
############################## 2.0.0 begin ############################################

#人物属性-疲劳值
alter table roledata add column `Weary` int(10) NOT NULL default '0' COMMENT '疲劳值' after `WuJi`;

# Jason 2010-4-14 v2.0.0 添加法宝表
CREATE TABLE IF NOT EXISTS `fabao` (                                          
          `SerialNum` bigint(20) NOT NULL COMMENT '装备序列号',         
          `NativeIntelligence` tinyint(4) NOT NULL COMMENT '法宝资质',  
          `Stage` tinyint(4) NOT NULL COMMENT '法宝等阶',               
          `PracticeValue` int(10) NOT NULL COMMENT '修炼值',            
          `AttTypes` tinyblob COMMENT '影响人物属性类型1',              
          `AttGrowings` tinyblob COMMENT '属性增长值1',                 
          `WuxingType` tinyint(4) DEFAULT NULL COMMENT '法宝主属性',    
          `WuxingHoles` tinyblob COMMENT '五行灵槽类型1',               
          `WuxingBeadIDs` tinyblob COMMENT '灵珠typeid1',               
          `SlotAttTypes` tinyblob COMMENT '灵槽契合奖励属性类型',       
          `SlotAttValue` tinyblob COMMENT '灵槽契合奖励属性值',         
          `WuxingMatchReward` tinyblob COMMENT '五行相生奖励',          
          `UseLevel` tinyint(4) NOT NULL COMMENT '使用级别限制',        
          `SlotNum` tinyint(1) NOT NULL COMMENT '灵槽个数',
          PRIMARY KEY (`SerialNum`)                                     
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

# 世博会等活动 通用数据排行表 
CREATE TABLE  `activity_rankdata` (                                                                                    
              `role_id` int(10) unsigned NOT NULL  COMMENT '角色ID',                
              `role_name` varchar(32) NOT NULL  COMMENT '角色名字',  
              `role_level` varchar(32) NOT NULL  COMMENT '角色等级',  
              `datatype1` int(10) unsigned NOT NULL COMMENT '字段1类型',
              `datatype2` int(10) unsigned NOT NULL COMMENT '字段2类型',
              `datatype3` int(10) unsigned NOT NULL COMMENT '字段3类型',
              `datatype4` int(10) unsigned NOT NULL COMMENT '字段4类型',
              `datatype5` int(10) unsigned NOT NULL COMMENT '字段5类型',
              `datatype6` int(10) unsigned NOT NULL COMMENT '字段6类型',        
              `datatype7` int(10) unsigned NOT NULL COMMENT '字段7类型',
              `datatype8` int(10) unsigned NOT NULL COMMENT '字段8类型',             
                PRIMARY KEY (`role_id`)                                                                
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;   

alter table `fabao` add column `AttGrowings0` tinyblob COMMENT '属性增长值0,生成时值' after `AttTypes`;
# 2010-4-16
alter table `fabao` add column `SlotLingzhuMatch` tinyint COMMENT '灵珠契合个数';
alter table `fabao` add column `SlotMacthNdx` tinyblob COMMENT '灵珠契合灵槽索引数组';
alter table `fabao` add column `AttLingzhu` tinyblob COMMENT '灵珠镶嵌奖励属性类型数组';
alter table `fabao` add column `AttLingzhuValue` tinyblob COMMENT '灵珠镶嵌奖励属性类型数组';
# 去掉多余的列
alter table `fabao` drop column `AttLingzhu` ;
alter table `fabao` drop column `AttLingzhuValue` ;

# 日常活动更新表
CREATE TABLE `dailytask_done` (                                                                           
             `RoleID` int(10) unsigned NOT NULL,                                                                
             `TaskID` int(10) unsigned NOT NULL COMMENT '任务id',                                             
             `Times` smallint(6) unsigned NOT NULL COMMENT '一个时期内完成的任务次数',              
             `StartTime` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '任务接取开始时间',  
             PRIMARY KEY (`RoleID`,`TaskID`)                                                                    
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='已做过日常任务';

############################## 2.0.0 end   ############################################
############################## 2.2.0 begin ############################################
# 注，下面的内容已经更改为2.2.0内容
# Jason 2010-5-21 v2.1.2
alter table `roledata` add column `SoaringValue` tinyint(3) unsigned NOT NULL DEFAULT '255' COMMENT '飞升属性';
alter table `equip` add column `SoaringFlag` tinyint(2) NOT NULL DEFAULT '0' COMMENT '飞升标志';
alter table `equip_del` add column `SoaringFlag` tinyint(2) NOT NULL DEFAULT '0' COMMENT '飞升标志';
alter table `equip_baibao` add column `SoaringFlag` tinyint(2) NOT NULL DEFAULT '0' COMMENT '飞升标志';
alter table `roledata` add column `SoaringSkillLearnTimes` tinyint(3) unsigned NOT NULL DEFAULT '100' COMMENT '飞升技能当天学习次数';
# Jason 2010-5-26 v2.1.2
alter table `equip` add column `CastingTimes` tinyint(3) NOT NULL DEFAULT '0' COMMENT '熔铸次数';
alter table `equip_del` add column `CastingTimes` tinyint(3) NOT NULL DEFAULT '0' COMMENT '熔铸次数';
alter table `equip_baibao` add column `CastingTimes` tinyint(3) NOT NULL DEFAULT '0' COMMENT '熔铸次数';
# Jason 2010-5-27 v2.2.0 韧性
alter table `roledata` add column `Toughness` smallint(4) NOT NULL DEFAULT '0' COMMENT '韧性';

## 调整roledata表和master表中的授业点数和师德点数字段
# roledata中的授业点数和师德点数
alter table roledata change `JingWuPoint` `JingWuPoint` int(10) unsigned NOT NULL default '0' COMMENT '精武点数' after `MasterID`;
alter table roledata change `QinWuPoint` `QinWuPoint` int(10) unsigned NOT NULL default '0' COMMENT '勤武点数' after `JingWuPoint`;

# 师徒数据
alter table master change `JingWuPoint` `JingWuPoint` int(10)  unsigned NOT NULL DEFAULT '0' COMMENT '精武点数';
alter table master change `QinWuPoint` `QinWuPoint` int(10)   unsigned NOT NULL DEFAULT '0' COMMENT '勤武点数';

############################## 2.2.0 end   ############################################

############################## F-Plan 1.0.0 ############################################

alter table `account_common` change `WareSize` `WareSize` smallint(6) NOT NULL DEFAULT '40' COMMENT '仓库大小,默认值为40';

alter table `roledata` change `BagSize` `BagSize` smallint(6) NOT NULL default '40' COMMENT '背包大小,默认为40';

alter table `equip` add column 	`RoleAtt2EffectCount` tinyint unsigned NOT NULL default '0' COMMENT '装备对角色的进阶/二级属性影响个数';
alter table `equip_del` add column 	`RoleAtt2EffectCount` tinyint unsigned NOT NULL default '0' COMMENT '装备对角色的进阶/二级属性影响个数';
alter table `equip_baibao` add column 	`RoleAtt2EffectCount` tinyint unsigned NOT NULL default '0' COMMENT '装备对角色的进阶/二级属性影响个数';
# Jason 2010-6-12 
alter table `roledata` add column `ConsolidateTimes` int(10) NOT NULL default '0' COMMENT '角色强化操作总次数'; 
alter table `equip` add column `ConsolidateLevel` tinyint(3) NOT NULL default '0' COMMENT '装备强化等级';
alter table `equip_del` add column `ConsolidateLevel` tinyint(3) NOT NULL default '0' COMMENT '装备强化等级';
alter table `equip_baibao` add column `ConsolidateLevel` tinyint(3) NOT NULL default '0' COMMENT '装备强化等级';

#任务板相关的更新
create table quest_board (
	`RoleID` int(10) unsigned NOT NULL,                                                                
  `QuestID` int(10) unsigned NOT NULL COMMENT '任务id',                                             
  `BoardID` int(10) unsigned NOT NULL default 0 COMMENT '任务板id',         
	`State` tinyint(3) NOT NULL default 0 COMMENT '任务状态 0为未接取 1为已接',
	PRIMARY KEY (`RoleID`,`QuestID`) 
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='任务板任务';
	
alter table roledata add column `CompleteRefreshTime` int(10) NOT NULL default 0 COMMENT '完成任务刷新任务板的次数' after `Toughness`;
alter table roledata add column `IMRefreshTime` int(10) NOT NULL default 0 COMMENT '使用道具刷新任务板的次数' after `CompleteRefreshTime`;
# Jason 2010-6-19 v1.0.0 强化属性影响
alter table `equip` add column `ConsolidateAttEffect` tinyblob COMMENT '强化属性影响';
alter table `equip_baibao` add column `ConsolidateAttEffect` tinyblob COMMENT '强化属性影响';
alter table `equip_del` add column `ConsolidateAttEffect` tinyblob COMMENT '强化属性影响';

# 强制脱离师门申请表
CREATE TABLE `force_break_out_apprentice` (
             `RoleID` int(10) unsigned NOT NULL COMMENT '申请强行脱离师门的徒弟的角色ID',
             `MasterID` int(10) unsigned NOT NULL COMMENT '师傅的角色ID',
			 `BreakoutInsertTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '申请强行脱离师门的时间',
             PRIMARY KEY (`RoleID`)
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='强制脱离师门申请';
           


##2010-6-17
#全区角色名字和帮派名字统一
CREATE TABLE  `role_name` (                                                                                          
              `name` varchar(32) NOT NULL  COMMENT '角色名字',   
               PRIMARY KEY (`name`)                                      
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;  

CREATE TABLE  `guild_name` (                                                                                          
              `name` varchar(32) NOT NULL  COMMENT '角色名字',   
               PRIMARY KEY (`name`)                                      
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 


#公会战宣战关系表
CREATE TABLE `guild_war_declare` (
			  `AttackGuildID` int(10) unsigned NOT NULL COMMENT '宣战公会ID',
			  `DefenceGuildID` int(10) unsigned NOT NULL COMMENT '被宣战公会ID',
			  `WarDeclareTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '宣战时间',
			   PRIMARY KEY (`AttackGuildID`)
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 

#2010-7-7-14 Jason 1.0.0 保存角色属性
alter table `roledata` add column `RoleState` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色状态，如pk状态等';

#删掉公会战宣战关系表的主键，并根据宣战时间创建索引
alter table `guild_war_declare` drop primary key;
alter table `guild_war_declare` add index (`WarDeclareTime`);

#外观显示设置默认为装备模式
alter table `roledata` change `DisplaySet` `DisplaySet` tinyint(3) NOT NULL default '0' COMMENT '外观显示设置(默认显示装备模式)';

#战场进攻竞价报名表
CREATE TABLE `battlefield_attack_signup`(
			`BattleFieldID` tinyint(3) unsigned NOT NULL COMMENT '战场ID',
			`GuildID` int(10) unsigned NOT NULL COMMENT '竞价报名公会ID',
			`BidFund` int(10) unsigned NOT NULL default '0' COMMENT '竞价报名出价',
			KEY `GuildID` (`GuildID`)
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 
			
#战场同盟报名表
CREATE TABLE `battlefield_alliance_signup`(
			`BattleFieldID` tinyint(3) unsigned NOT NULL COMMENT '战场ID',
			`GuildID` int(10) unsigned NOT NULL COMMENT '同盟公会ID',
			`GuildCapType` tinyint(1) unsigned NOT NULL COMMENT '公会争夺判断',
			`BeConfirmed` tinyint(1) unsigned NOT NULL default '0' COMMENT '是否被确认,1表示已经确认了,0表示未确认',
			KEY `GuildID` (`GuildID`)
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;

#公会战宣战关系表
alter table `guild_war_declare` add column `WarEnded` tinyint(3) unsigned NOT NULL default '0' COMMENT '是不是已经结束过了,0表示没有结束过';

#公会技能表增加技能是否激活字段
alter table `guild_skill` add column `active` tinyint(1) NOT NULL default '0' COMMENT '当前已经激活了,0表示未激活';

#跟随宠物和坐骑的栏位上限
alter table `roledata` add column `FollowPetPocketValue` smallint(6) unsigned NOT NULL Default '5' COMMENT '跟随宠物的栏位上限';
alter table `roledata` add column `RidingPetPocketValue` smallint(6) unsigned NOT NULL Default '5' COMMENT '坐骑的栏位上限';

# 去掉原来的宠物栏位上限字段
alter table `roledata` drop column `PetPocketValve` ;

# 2010/8/20
# 记录当前地图上的在线人数
CREATE TABLE `mapolinfo` (              
             `MapID` int(10) unsigned NOT NULL,    
             `IsInst` int(10) DEFAULT '0'         COMMENT '是否为副本',
             `OnlineNum` int(10) DEFAULT '0'      COMMENT '在线人数',
             `OpenNum` mediumint(6) DEFAULT '0'   COMMENT '地图开启个数对副本有意义',
             `RecTime` datetime DEFAULT NULL          COMMENT '记录时间',
             PRIMARY KEY (`MapID`)                 
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8  ;
          
# 标识是不是已经做过试炼状态表的数据转移
alter TABLE roledata change `TrainStateTransfered` `TrainStateTransfered` tinyint(2) unsigned NOT NULL default '1' COMMENT '试炼状态表做过数据转移(0:没有转移过;1:已经转移过),圣魔之血中初始值为1';

# 试炼表增加试炼完成度字段
alter TABLE train_state add column `Completeness` int(10) unsigned default '0' NOT NULL COMMENT '试炼完成度';

# 神铸
ALTER TABLE `equip` add column `LastGodStrengthenLevel`	int(10)	unsigned default '0' NOT NULL COMMENT '上次神铸等级';
ALTER TABLE `equip` add column `GodStrengthenLevel` int(10) unsigned default '0' NOT NULL COMMENT '当前神铸等级';
ALTER TABLE `equip` ADD COLUMN `CurGodStrengthenExp` int(10) unsigned default '0' NOT NULL COMMENT '神铸当前经验值';
ALTER TABLE `equip` ADD COLUMN `GodStrengthenTime` int(10) unsigned default '0'NOT NULL COMMENT '上次神铸时间';

ALTER TABLE `equip_baibao` add column `LastGodStrengthenLevel`	int(10)	unsigned default '0' NOT NULL COMMENT '上次神铸等级';
ALTER TABLE `equip_baibao` add column `GodStrengthenLevel` int(10) unsigned default '0' NOT NULL COMMENT '当前神铸等级';
ALTER TABLE `equip_baibao` ADD COLUMN `CurGodStrengthenExp` int(10) unsigned default '0' NOT NULL COMMENT '神铸当前经验值';
ALTER TABLE `equip_baibao` ADD COLUMN `GodStrengthenTime` int(10) unsigned default '0'NOT NULL COMMENT '上次神铸时间';

ALTER TABLE `equip_del` add column `LastGodStrengthenLevel`	int(10)	unsigned default '0' NOT NULL COMMENT '上次神铸等级';
ALTER TABLE `equip_del` add column `GodStrengthenLevel` int(10) unsigned default '0' NOT NULL COMMENT '当前神铸等级';
ALTER TABLE `equip_del` ADD COLUMN `CurGodStrengthenExp` int(10) unsigned default '0' NOT NULL COMMENT '神铸当前经验值';
ALTER TABLE `equip_del` ADD COLUMN `GodStrengthenTime` int(10) unsigned default '0'NOT NULL COMMENT '上次神铸时间';

# 统计角色每月在线时长(在线时长单位为分钟)
CREATE TABLE `role_online_time_each_month` (
		`AccountID` int(10) unsigned NOT NULL default '0' COMMENT '账号ID',
		`RoleID` int(10) unsigned NOT NULL default '0' COMMENT '角色ID',
		`Year` tinyint(2) unsigned NOT NULL default '0' COMMENT '年份，2000年之后的，例如2001年在这里就是1',
		`January` smallint(2) unsigned NOT NULL default '0' COMMENT '1月份在线时长',
		`February` smallint(2) unsigned NOT NULL default '0' COMMENT '2月份在线时长',
		`March` smallint(2) unsigned NOT NULL default '0' COMMENT '3月份在线时长',
		`April` smallint(2) unsigned NOT NULL default '0' COMMENT '4月份在线时长',
		`May` smallint(2) unsigned NOT NULL default '0' COMMENT '5月份在线时长',
		`June` smallint(2) unsigned NOT NULL default '0' COMMENT '6月份在线时长',
		`July` smallint(2) unsigned NOT NULL default '0' COMMENT '7月份在线时长',
		`August` smallint(2) unsigned NOT NULL default '0' COMMENT '8月份在线时长',
		`September` smallint(2) unsigned NOT NULL default '0' COMMENT '9月份在线时长',
		`October` smallint(2) unsigned NOT NULL default '0' COMMENT '10月份在线时长',
		`November` smallint(2) unsigned NOT NULL default '0' COMMENT '11月份在线时长',
		`December` smallint(2) unsigned NOT NULL default '0' COMMENT '12月份在线时长',
		PRIMARY KEY (`AccountID`,`RoleID`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8  ;

#增加角色VIP等级功能
alter table `roledata` add column `VipLevel` tinyint(1) unsigned NOT NULL Default '0' COMMENT '角色VIP等级,0是无等级，1银，2金，3白金';
alter table `roledata` add column `VipStartTime` int(10) unsigned NOT NULL Default '0' COMMENT 'Vip道具使用时间，没有使用过或者已经过期为0';
alter table `roledata` add column `FreeWorldTalkTimes` smallint(5) unsigned NOT NULL Default '0' COMMENT 'Vip角色已经使用的免费世界喊话的次数';

#增加角色是否能参与排行和妖精实力值字段
alter table `roledata` add column `CanRankFlag` tinyint(1) unsigned NOT NULL Default '1' COMMENT '角色是否能进入排行,0不能，1能';
alter table `roledata` add column `YaoJingValue` int(10) unsigned NOT NULL Default '0' COMMENT '角色妖精实力值';
alter table `strength_rankings` drop Strength;
alter table `strength_rankings` add column `YaoJingValue` int(10) unsigned NOT NULL Default '0' COMMENT '角色妖精实力值';
alter table `strength_rankings` add column `RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID';
alter table `level_rankings` add column `RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID';

# 分类统计各个媒体的在线人数
CREATE TABLE `mediaonline` (              
	    `DistributionID` int(10) unsigned NOT NULL Default '0' COMMENT '媒体ID',
            `OnlineNum` int(10) DEFAULT '0' COMMENT '在线人数',
            `RecTime` datetime DEFAULT NULL COMMENT '记录时间',
             PRIMARY KEY (`DistributionID`)
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8  ;

alter table `strength_rankings_pre` drop Strength;
alter table `strength_rankings_pre` add column `YaoJingValue` int(10) unsigned NOT NULL Default '0' COMMENT '角色妖精实力值';
alter table `strength_rankings_pre` add column `RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID';
alter table `level_rankings_pre` add column `RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID';

############################## F-Plan 1.0.0 end############################################

############################## F-Plan 1.2.0 begin############################################
# 商城限量物品的信息
CREATE TABLE `LimitNumItemInfo`(
            `Id` int(10) unsigned NOT NULL  COMMENT '商品信息序列号',
            `ItemTypeID` int(10) unsigned NOT NULL COMMENT '物品类型id',
            `RemainNum`  int(10) unsigned NOT NULL COMMENT '该物品剩余个数',
            `IsEnd`       tinyint(2) NOT NULL COMMENT '是否销售结束',
            PRIMARY KEY(`Id`)
            )ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 
# 一元计划相关字段
alter table `roledata` add column `Buy50LvlItemFlag` tinyint(1) unsigned NOT NULL Default '0' COMMENT '角色是否已经购买了一元计划活动中50级的装备,0未购买，1已经购买';
alter table `roledata` add column `Buy60LvlItemFlag` tinyint(1) unsigned NOT NULL Default '0' COMMENT '角色是否已经购买了一元计划活动中60级的装备,0未购买，1已经购买';
alter table `roledata` add column `Buy70LvlItemFlag` tinyint(1) unsigned NOT NULL Default '0' COMMENT '角色是否已经购买了一元计划活动中70级的装备,0未购买，1已经购买';
alter table `roledata` add column `Buy80LvlItemFlag` tinyint(1) unsigned NOT NULL Default '0' COMMENT '角色是否已经购买了一元计划活动中80级的装备,0未购买，1已经购买';
############################## F-Plan 1.2.0 end############################################

############################## F-Plan 1.2.1 begin############################################
            
alter table `item` add column `ItemSpecVal1` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值1';
alter table `item` add column `ItemSpecVal2` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值2';
alter table `item_baibao` add column `ItemSpecVal1` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值1';
alter table `item_baibao` add column `ItemSpecVal2` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值2';
alter table `item_del` add column `ItemSpecVal1` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值1';
alter table `item_del` add column `ItemSpecVal2` int(10) unsigned NOT NULL Default '0' COMMENT '物品特殊属性值2';

alter table `fabao` add column `Mood` int(10) Default '999' COMMENT '妖精心情';
alter table `fabao` add column `MoodTicks` int(10) Default '0' COMMENT '妖精心情计数器';
alter table `fabao` add column `MoodBuffID` int(10) unsigned Default '0' COMMENT '妖精心情相关buff';

alter table roledata add column `YListCompleteRefreshTime` int(10) NOT NULL default 0 COMMENT '完成天之行会任务刷新任务板的次数';
alter table roledata add column `YListIMRefreshTime` int(10) NOT NULL default 0 COMMENT '使用道具刷新天之行会任务板的次数';
alter table quest_board add column `BoardType` tinyint(2) unsigned NOT NULL DEFAULT 1 COMMENT '任务板类别, 1为悬赏榜，2为天之行会任务板';

# 妖精重生使用，
CREATE TABLE `fabao_extend_his` (                          
                    `SerialNum` bigint(20) NOT NULL,                         
                    `AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
                    `RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',                        
                    `Stage` tinyint(4) NOT NULL                     COMMENT '妖精等级'        ,
                    `EleInjuryType` tinyint(4) NOT NULL             COMMENT '元素伤害类型'    ,   
                    `EleInjury` int(10) DEFAULT '0'                 COMMENT '元素伤害值'       , 
                    `EleResisFire` int(10) DEFAULT '0'              COMMENT '火元素抗性'        ,
                    `EleResisWater` int(10) DEFAULT '0'             COMMENT '水元素抗性'        ,
                    `EleResisEarth` int(10) DEFAULT '0'             COMMENT '土元素抗性'        ,
                    `EleResisWind` int(10) DEFAULT '0'              COMMENT '风元素抗性'        ,
                    `EleResisBright` int(10) DEFAULT '0'            COMMENT '光明元素抗性'       , 
                    `EleResisDark` int(10) DEFAULT '0'              COMMENT '暗黑元素抗性'        ,
                    `DamageUpgradeTimes` int(10) DEFAULT '0'        COMMENT '元素伤害提升能力'     ,   
                    `ResistanceUpgradeTimes` int(10) DEFAULT '0'    COMMENT '元素抗性提升能力',
                    PRIMARY KEY (`SerialNum`,`Stage`),             
		     KEY `SerialNum` (`SerialNum`),
		     KEY `Stage` (`Stage`),
		     KEY `AccountID` (`AccountID`),
		     KEY `RoleID` (`RoleID`)
                  ) ENGINE=MyISAM DEFAULT CHARSET=utf8  ;
                  
alter table `fabao` add column `EleInjuryType` tinyint(4) Default '0'	COMMENT '妖精元素伤害类型';
alter table `fabao` add column `EleInjury` int(10) Default '0'			COMMENT '妖精元素伤害值';
alter table `fabao` add column `EleResisFir` int(10) Default '0'		COMMENT '火元素伤害';
alter table `fabao` add column `EleResisWater` int(10) Default '0'		COMMENT '水元素伤害';
alter table `fabao` add column `EleResisEarth` int(10) Default '0'		COMMENT '土元素伤害';
alter table `fabao` add column `EleResisWind` int(10) Default '0'		COMMENT '风元素伤害';
alter table `fabao` add column `EleResisBright` int(10) Default '0'		COMMENT '光明元素伤害';
alter table `fabao` add column `EleResisDark` int(10) Default '0'		COMMENT '暗黑元素伤害';
alter table `fabao` add column `DamageUpgradeTimes` int(10) Default '0' COMMENT '妖精元素伤害提升次数';
alter table `fabao` add column `ResistanceUpgradeTimes` int(10) Default '0' COMMENT '妖精元素抗性提升次数';
alter table `fabao` add column `FairySkill1` int(10) unsigned Default '4294967295' COMMENT '妖精特技槽1';
alter table `fabao` add column `FairySkill2` int(10) unsigned Default '4294967295' COMMENT '妖精特技槽2';
alter table `fabao` add column `FairySkill3` int(10) unsigned Default '4294967295' COMMENT '妖精特技槽3';
alter table `fabao` add column `FairySkill4` int(10) unsigned Default '4294967295' COMMENT '妖精特技槽4';
alter table `fabao` add column `FairySkill5` int(10) unsigned Default '4294967295' COMMENT '妖精特技槽5';
alter table `fabao` add column `UnsureSkill` int(10) unsigned Default '4294967295' COMMENT '待确认特技';
                  
alter table role_online_time_each_month drop PRIMARY KEY;
alter table role_online_time_each_month add INDEX(`AccountID`,`RoleID`,`Year`);

############################## F-Plan 1.2.1 end############################################

############################## F-Plan 1.2.2 end############################################
alter table task add column ChuandaoCount int(10) unsigned Default '0' COMMENT '传道任务计数' after StartTime;
alter table `guild` add column `DailyRewardTakenTimes` tinyint(3) COMMENT '公会各职务领取在线奖励次数，用于战场每日奖励的职位限制';
alter table `roledata` add column `TakeGuildRewardTime` int(10) unsigned NOT NULL default '0' COMMENT '角色领取公会战场每日在线奖励时间';



# 武勋排行榜，                 
CREATE TABLE `wuxun_rankings` (                                           
                  `Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',         
                  `PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',  
                  `wuxun` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '武勋',      
                  `RoleName` varchar(32) NOT NULL DEFAULT 'N/A' COMMENT '角色名', 
                  `RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',        
                  PRIMARY KEY (`RoleName`)                                                
                ) ENGINE=MyISAM DEFAULT CHARSET=utf8       ;


# 武勋排行榜历史表，  
create table wuxun_rankings_pre like wuxun_rankings;

alter table `guild` change `DailyRewardTakenTimes` `DailyRewardTakenTimes` blob COMMENT '公会各职务领取在线奖励次数，用于战场每日奖励的职位限制';

alter table `fabao` add column `StoredExpOpen` tinyint(1) unsigned Default '0' COMMENT '经验封印是否开启,0:关闭，1:开启';
alter table `fabao` add column `CurStoredExp` int(10) unsigned Default '0' COMMENT '封印的经验值';
alter table `roledata` add column `ExpPilularUseTimes` int(10) unsigned NOT NULL default '0' COMMENT '角色使用经验丹次数';

alter table `role_online_time_each_month` add column `LastUpdateTime`  datetime DEFAULT NULL COMMENT '最后一次修改记录的时间';
alter table `role_online_time_each_month` drop key `AccountID`;
alter table `role_online_time_each_month` add primary key  (`AccountID`,`RoleID`,`Year`);
############################## F-Plan 1.2.2 end############################################

############################## F-Plan 1.3.0 begin############################################
# Jason 2010-11-30 
alter table `roledata` add column `PickupModeSetting`	int(10)	unsigned NOT NULL default '4294967295' COMMENT '拾取模式设置';

#神之竞标
create table `god_bid`(
				`RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',
				`Bidden` bigint(20) NOT NULL DEFAULT '0' COMMENT '竞标出价',
				`BidType` tinyint(1) unsigned NOT NULL Default '0' COMMENT '竞标类型,普通模式-0, 黑市金币-1, 黑市元宝-2',
				`BidID` int(10) unsigned NOT NULL Default '0' COMMENT '竞标ID',
				PRIMARY KEY (`RoleID`,`BidID`)
				)ENGINE=MyISAM DEFAULT CHARSET=utf8;

#活动表增加合服存储活动数据字典
alter table `activity` add column `ScriptData1` blob NULL;
alter table `activity` add column `ScriptData2` blob NULL;
alter table `activity` add column `ScriptData3` blob NULL;
alter table `activity` add column `ScriptData4` blob NULL;

# Jason 2010-12-6 妖精繁殖相关
alter table `fabao` add column `FabaoStatus` int(10) NOT NULL default '0' COMMENT '妖精状态';
alter table `fabao` add column `StatusChangeTime` int(10) unsigned NOT NULL default '0' COMMENT '妖精状态改变倒计时时间';
alter table `fabao` add column `BirthValue` int(10) NOT NULL default '0' COMMENT '妖精生育值';
alter table `fabao` add column `BirthTimes` int(10) NOT NULL default '0' COMMENT '妖精可配对次数';
alter table `fabao` add column `LevelBirth` tinyblob COMMENT '妖精配对次数升级记录';
alter table `fabao` add column `IsGivenBirthTimes`	tinyint NOT NULL default '0' COMMENT '是否生育过，补偿配对次数用';
############################## F-Plan 1.3.0 end############################################

############################## F-Plan 1.3.2 begin############################################
#砸金蛋
create table `egg_broker`(
				`RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',
				`CurLayer` tinyint(2) unsigned NOT NULL Default '0' COMMENT '当前可以砸的层',
				`ColourEggPos1` tinyint(2) NOT NULL Default '-1' COMMENT '彩蛋位置1',
				`ColourEggPos2` tinyint(2) NOT NULL Default '-1' COMMENT '彩蛋位置2',
				`BrokePos1` tinyint(2) NOT NULL Default '-1' COMMENT '1层砸中的位置',
				`ItemID1` int(10) unsigned NOT NULL Default '0' COMMENT '1层砸中的物品',
				`BrokePos2` tinyint(2) NOT NULL Default '-1' COMMENT '2层砸中的位置',
				`ItemID2` int(10) unsigned NOT NULL Default '0' COMMENT '2层砸中的物品',
				`BrokePos3` tinyint(2) NOT NULL Default '-1' COMMENT '3层砸中的位置',
				`ItemID3` int(10) unsigned NOT NULL Default '0' COMMENT '3层砸中的物品',
				`BrokePos4` tinyint(2) NOT NULL Default '-1' COMMENT '4层砸中的位置',
				`ItemID4` int(10) unsigned NOT NULL Default '0' COMMENT '4层砸中的物品',
				`BrokePos5` tinyint(2) NOT NULL Default '-1' COMMENT '5层砸中的位置',
				`ItemID5` int(10) unsigned NOT NULL Default '0' COMMENT '5层砸中的物品',
				`BrokePos6` tinyint(2) NOT NULL Default '-1' COMMENT '6层砸中的位置',
				`ItemID6` int(10) unsigned NOT NULL Default '0' COMMENT '6层砸中的物品',
				`BrokePos7` tinyint(2) NOT NULL Default '-1' COMMENT '7层砸中的位置',
				`ItemID7` int(10) unsigned NOT NULL Default '0' COMMENT '7层砸中的物品',
				PRIMARY KEY (`RoleID`)
				)ENGINE=MyISAM DEFAULT CHARSET=utf8;
############################## F-Plan 1.3.2 end############################################


############################## F-Plan 1.3.3 begin############################################
# Jason 2010-12-21 玩家回归修改
alter table `roledata` add column `PlayerBack` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否领取过玩家回归奖励';
alter table `roledata` add column `PlayerBackDays` smallint(6) NOT NULL DEFAULT '0' COMMENT '累计回归天数';
alter table `roledata` add column `VipMaxDays` int(10) unsigned NOT NULL Default '0' COMMENT 'Vip道具时限(单位为天)' after `FreeWorldTalkTimes`;
############################## F-Plan 1.3.3 end############################################

############################## F-Plan 1.3.6 begin############################################
# Jason 2011-1-11 台湾版本，增加是否同意20级关闭pk保护模式字段
alter table `roledata` add column `UseConstraintsMaxPKSafeGuardLevel` tinyint(1) Default '0' COMMENT '是否使用配置文件里面设置的最高pk保护等级';

# 衣橱系统
create table `wardrobe`(
				`RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',
				`LayerID` int(10) unsigned NOT NULL Default '0' COMMENT '衣橱栏位ID',
				`Level` tinyint(2) NOT NULL Default '1' COMMENT '衣橱栏等级',
				`State` tinyint(2) NOT NULL Default '0' COMMENT '衣橱栏状态,0-空，一件时装也没有;1-没有集齐，有几件时装;2-集齐，未使用;3-自己穿戴中;4-好友装扮中',
				`PlayActStartTime` int(10) unsigned NOT NULL Default '0' COMMENT '装扮开始时间',
				`PlayActFriendID` int(10) unsigned NOT NULL Default '0' COMMENT '装扮的好友ID',
				PRIMARY KEY (`RoleID`,`LayerID`)
				)ENGINE=MyISAM DEFAULT CHARSET=utf8;
				
# 被好友装扮的衣橱栏位ID
alter table `roledata` add column `BePlayActLayerID` int(10) unsigned NOT NULL Default '4294967295' COMMENT '被好友装扮的衣橱栏位ID';
alter table `roledata` add column `BePlayActLevel` int(10) unsigned NOT NULL Default '0' COMMENT '被好友装扮的衣橱栏位等级';

############################## F-Plan 1.3.6 end############################################

############################## F-Plan 1.3.8 begin############################################

# 当前经验值的最大值扩大
alter table roledata change `ExpCurLevel` `ExpCurLevel` bigint(20) unsigned NOT NULL default '0' COMMENT '当前经验';

############################## F-Plan 1.3.8 end############################################


############################## F-Plan 1.4.0 begin############################################
# Jason 2011-1-12 神系统属性
alter table `roledata` add column `GodHead` int(10) unsigned NOT NULL Default '0' COMMENT '神格';
alter table `roledata` add column `GodFaith` int(10) unsigned NOT NULL Default '0' COMMENT '神,信仰';
alter table `roledata` add column `GodCondenced` int(10) unsigned NOT NULL Default '0' COMMENT '神,凝聚度';

# 神职相关
alter table `roledata` add column `Clergy` int(10) unsigned NOT NULL Default '0' COMMENT '角色神职';
alter table `roledata` add column `PreClergy` int(10) unsigned NOT NULL Default '0' COMMENT '角色上次的神职';
alter table `roledata` add column `ClergyMarsPoint` int(10) unsigned NOT NULL Default '0' COMMENT '战神系分值';
alter table `roledata` add column `ClergyApolloPoint` int(10) unsigned NOT NULL Default '0' COMMENT '太阳系分值';
alter table `roledata` add column `ClergyRabbiPoint` int(10) unsigned NOT NULL Default '0' COMMENT '法神系分值';
alter table `roledata` add column `ClergyPeacePoint` int(10) unsigned NOT NULL Default '0' COMMENT '和平之神系分值';
alter table `roledata` add column `ClergyCandidateType` tinyint(2) NOT NULL Default '-1' COMMENT '神职竞选报名状态，0战神 1太阳神 2法师之神 3和平之神';

drop table if EXISTS Clergy_Mars;
create table `Clergy_Mars` (
				`Num` tinyint(2) unsigned NOT NULL COMMENT '名次',
				`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
				`Sex` tinyint(2) unsigned NOT NULL DEFAULT '3' COMMENT '性别',
				`Clergy` int(10) unsigned NOT NULL COMMENT '职位',
				`ClergyPoint` int(10) unsigned NOT NULL COMMENT '分值',
				`RecvOrNot` bool Default 0 COMMENT '职位是否被领取',
				KEY (`Num`)			
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS Clergy_Apollo;
create table `Clergy_Apollo` (
				`Num` tinyint(2) unsigned NOT NULL COMMENT '名次',
				`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
				`Sex` tinyint(2) unsigned NOT NULL DEFAULT '3' COMMENT '性别',
				`Clergy` int(10) unsigned NOT NULL COMMENT '职位',
				`ClergyPoint` int(10) unsigned NOT NULL COMMENT '分值',
				`RecvOrNot` bool Default 0 COMMENT '职位是否被领取',
				KEY (`Num`)			
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS Clergy_Rabbi;
create table `Clergy_Rabbi` (
				`Num` tinyint(2) unsigned NOT NULL COMMENT '名次',
				`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
				`Sex` tinyint(2) unsigned NOT NULL DEFAULT '3' COMMENT '性别', 
				`Clergy` int(10) unsigned NOT NULL COMMENT '职位',
				`ClergyPoint` int(10) unsigned NOT NULL COMMENT '分值',
				`RecvOrNot` bool Default 0 COMMENT '职位是否被领取',
				KEY (`Num`)			
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS Clergy_Peace;
create table `Clergy_Peace` (
				`Num` tinyint(2) unsigned NOT NULL COMMENT '名次',
				`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
				`Sex` tinyint(2) unsigned NOT NULL DEFAULT '3' COMMENT '性别', 
				`Clergy` int(10) unsigned NOT NULL COMMENT '职位',
				`ClergyPoint` int(10) unsigned NOT NULL COMMENT '分值',
				`RecvOrNot` bool Default 0 COMMENT '职位是否被领取',
				KEY (`Num`)			
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS area;
CREATE TABLE `area` (                                                        
          `AreaID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '领域ID',  
          `RoleID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',  
          `Activation` tinyint(3) unsigned DEFAULT '0' COMMENT '激活状态', 
	  `CDTime` int(10) unsigned DEFAULT '0' COMMENT 'cd时间',
          PRIMARY KEY (`AreaID`,`RoleID`)                              
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS rarearea;
CREATE TABLE `rarearea` (                                               
            `AreaID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '领域表ID',        
            `ItemID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '领域卡表ID',      
            `DataID` bigint(20) NOT NULL DEFAULT '0' COMMENT '领域卡唯一ID或角色ID',  
            `State` smallint(3) DEFAULT '0' COMMENT '状态',                           
            `LeaveTime` int(10) DEFAULT '0' COMMENT '剩余有效时间',                   
            `RecordTime` int(10) DEFAULT '0' COMMENT '下线时间',                    
            PRIMARY KEY (`AreaID`,`DataID`)                        
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

alter table roledata add column Clergy4SeniorOrJunior tinyint(2) default 0 after `ClergyCandidateType`;

alter table `Clergy_Mars` add column Clergy4SeniorOrJunior tinyint(2) default 0 after ClergyPoint;
alter table `Clergy_Apollo` add column Clergy4SeniorOrJunior tinyint(2) default 0 after ClergyPoint;
alter table `Clergy_Rabbi` add column Clergy4SeniorOrJunior tinyint(2) default 0 after ClergyPoint;
alter table `Clergy_Peace` add column Clergy4SeniorOrJunior tinyint(2) default 0 after ClergyPoint;
############################## F-Plan 1.4.0 end############################################

############################## F-Plan 1.4.1 end############################################

# Add by Zr
alter table roledata add column KeyCodeRewarded tinyint(3) not null default 0 COMMENT '是否使用key码领取过奖励';

# Add by Zr
alter table pet_data change `lifeadded` `lifeadded` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '附加寿命';
update pet_data a set a.lifeadded=a.lifeadded*3600;

# Add by Zr
alter table roledata change column `KeyCodeRewarded` `KeyCodeRewarded` int(10) unsigned not null default 0 COMMENT '使用key码领取过奖励';

############################## F-Plan 1.4.1 end############################################

############################## F-Plan 1.4.2 begin##########################################
#家族表
DROP TABLE IF EXISTS `family`;
CREATE TABLE `family` (
		`FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID',
		`FamilyName` varchar(32) NOT NULL  COMMENT '家族名字',
		`LeaderID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '族长ID',
		`FounderID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创始人ID',
		`Active`  int(10) NOT NULL DEFAULT '0' COMMENT '家族活跃度',
		`CreateTime` datetime  NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '家族创建时间',
		PRIMARY KEY(`FamilyID`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8;
#家族成员表
DROP TABLE IF EXISTS `family_member`;
CREATE TABLE `family_member` (
		`RoleID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID',  
		`FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID',
		`JoinTime` datetime  NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT '加入家族时间',
		`IsLeader` tinyint(2) NOT NULL DEFAULT '0' COMMENT '是否是族长,0:否， 1:是',
		PRIMARY KEY(`FamilyID`,`RoleID`)
		) ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS family_sprite;
CREATE TABLE `family_sprite` (                                                        
          `FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID',  
          `Level` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '守护妖精等级', 
	  `Exp` bigint(20) NOT NULL DEFAULT '0' COMMENT '守护妖精成长度',  
	  `Name` varchar(32) NOT NULL COMMENT '守护妖精名称',  

	  `HP` int(10) DEFAULT '0' COMMENT '体力能力',
	  `EXAttack` int(10) DEFAULT '0' COMMENT '物理攻击能力',
	  `InAttack` int(10) DEFAULT '0' COMMENT '法术攻击',
	  `EXDefense` int(10) DEFAULT '0' COMMENT '物理防御',
	  `InDefense` int(10) DEFAULT '0' COMMENT '法术防御',
	  `EXAttackDeeper` int(10) DEFAULT '0' COMMENT '物理伤害加深',
	  `InAttackDeeper` int(10) DEFAULT '0' COMMENT '法术伤害加深',
	  `EXAttackResistance` int(10) DEFAULT '0' COMMENT '物理伤害减免',
	  `InAttackResistance` int(10) DEFAULT '0' COMMENT '法术伤害减免',
	  `Toughness` int(10) DEFAULT '0' COMMENT '韧性',
	  `CritDes` int(10) DEFAULT '0' COMMENT '暴击抵消',
	  `ControleffectDeepen` int(10) DEFAULT '0' COMMENT '控制效果加深',
	  `ControleffectResistance` int(10) DEFAULT '0' COMMENT '控制效果抵抗',
	  `SlowingeffectDeepen` int(10) DEFAULT '0' COMMENT '减速效果强化',
	  `SlowingeffectResistance` int(10) DEFAULT '0' COMMENT '减速效果抵抗',
	  `FixedeffectDeepen` int(10) DEFAULT '0' COMMENT '固定伤害强化',
	  `FixedeffectResistance` int(10) DEFAULT '0' COMMENT '固定伤害抵抗',
	  `AgingeffectDeepen` int(10) DEFAULT '0' COMMENT '衰弱效果强化',
	  `AgingeffectResistance` int(10) DEFAULT '0' COMMENT '衰弱效果抵抗',
	  
          PRIMARY KEY (`FamilyID`)                              
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;


drop table if EXISTS family_sprite_requirement;
CREATE TABLE `family_sprite_requirement` (                                                        
          `FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID', 
	  `RequirementID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '修炼需求ID',
	  `Complete` tinyint(3) unsigned DEFAULT '0' COMMENT '完成状态',
	  `RoleName` varchar(32) DEFAULT '' COMMENT '完成者名称', 
	  `Result` int(10) DEFAULT '0' COMMENT '实际完成值',
	  
          PRIMARY KEY (`FamilyID`,`RequirementID`)                              
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

drop table if EXISTS family_role_sprite;
CREATE TABLE `family_role_sprite` (                                                        
          `FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID', 
	  `SpriteID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '妖精TypeID',
	  `RoleName` varchar(32) DEFAULT '' COMMENT '登记者名称', 
	  `Shili` float(10) DEFAULT '0' COMMENT '实力值',
	  `Gongming` float(10) DEFAULT '0' COMMENT '共鸣值',
	  `RoleID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '角色ID', 
	  `Level` tinyint(4) unsigned DEFAULT '0' COMMENT '登记妖精等级',  
	  `Quality` tinyint(1) unsigned DEFAULT '0' COMMENT '登记妖精品阶',
          PRIMARY KEY (`FamilyID`,`RoleID`)                              
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

# add by zr
drop table if EXISTS family_quest;
create table family_quest (
	`FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID',
	LastQuestProcess smallint(5) not null default 0 COMMENT '昨天的任务完成度',
	IssueFlag tinyint(3) not null default 0 COMMENT '0未发布 1已经发布',
	QuestID1 int(10) unsigned not null default 0 COMMENT '家族任务1',
	QuestNum1 int(10) unsigned not null default 0 COMMENT '家族任务1计数',
	QuestID2 int(10) unsigned not null default 0 COMMENT '家族任务2',
	QuestNum2 int(10) unsigned not null default 0 COMMENT '家族任务2计数', 
	QuestID3 int(10) unsigned not null default 0 COMMENT '家族任务3',
	QuestNum3 int(10) unsigned not null default 0 COMMENT '家族任务3计数', 
	QuestID4 int(10) unsigned not null default 0 COMMENT '家族任务4',
	QuestNum4 int(10) unsigned not null default 0 COMMENT '家族任务4计数', 	
	PRIMARY KEY (`FamilyID`)     
)ENGINE=MyISAM DEFAULT CHARSET=utf8;


alter table `family_member`  add column `BeRewarded` tinyint(2) NOT NULL DEFAULT '0' COMMENT '是否领取过奖励,0:否， 1:是';

alter table roledata add column `FamilyID` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族ID';

#混服运营
alter table `centralbilling` add column `CHANNELID` int(10) unsigned  DEFAULT '0' NOT NULL  COMMENT '运营商ID' after `USERNAME`;
alter table `centralbilling_log` add column `CHANNELID` int(10) unsigned  DEFAULT '0' NOT NULL  COMMENT '运营商ID' after `USERNAME`;
alter table `account_common` add column `ChannelID` int(10) unsigned  DEFAULT '0' NOT NULL  COMMENT '运营商ID' after `AccountID`;

alter table roledata change column `FamilyID` `FamilyID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '家族ID';
update roledata set FamilyID=4294967295 where FamilyID=0;


############################## F-Plan DB begin############################################
alter table `item` add column `del_time` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间';
alter table `item_del` add column `del_time` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间';
alter table `equip` add column `deltime` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间';
alter table `equip_del` add column `deltime` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间'; 
############################## F-Plan DB end############################################

#尝试解决主键相同的问题
alter table pet_data delay_key_write=0;
alter table vip_netbar delay_key_write=0;

# add by zr
drop table if EXISTS role_buy_mall_item_count;
create table role_buy_mall_item_count(
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID', 
	`MallItemID` int(10) unsigned NOT NULL comment '商城物品的ID',
	`Num` int(10) unsigned not null comment '购买次数',
	PRIMARY KEY (`RoleID`,`MallItemID`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

############################## F-Plan 1.4.2 end############################################
############################## F-Plan 1.4.4 begin############################################
alter table `roledata` add column `TrainDate` int(10) NOT NULL DEFAULT '0' COMMENT '家族妖精修炼物品计数日期';
alter table `roledata` add column `TrainNum` int(10) NOT NULL DEFAULT '0' COMMENT '家族妖精修炼当日修炼物品使用次数'; 

alter table `roledata` change `TrainDate` `TrainDate` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '家族妖精修炼物品计数日期';
alter table `rarearea` change `RecordTime` `RecordTime` int(10) unsigned DEFAULT '0' COMMENT '下线时间';

alter table `roledata` add column `award_point` int(10) NOT NULL DEFAULT '0' COMMENT '角色奖励点';
alter table `roledata` add column `award_flag` int(10) NOT NULL DEFAULT '0' COMMENT '礼包领取标志位';
############################## F-Plan 1.4.4 end############################################

#根据DBA要求增加key guildid 
alter table guild_member add key(guildid);

# gm工具使用的表
drop table if exists gm_server_data;      
create table gm_server_data
(
	`data_name` char(32) not null,
	`data_value` bigint(20) default 0,
	key(`data_name`) 
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
insert into gm_server_data value ('MinItemSerial',0);	

# by ZR
alter table item change column suffix suffix smallint(3) unsigned NOT NULL COMMENT '在容器中的位置';
alter table item_del change column suffix suffix smallint(3) unsigned NOT NULL COMMENT '在容器中的位置';

############################## F-Plan 1.4.8 begin############################################
# by zr
alter table fabao add column `MaxIntelligence` tinyint(3) default 0 comment '资质上限';

#增加角色结拜数据 by zs
alter table `roledata` add column `BrotherTeacherID` int(10) unsigned NOT NULL Default '0' COMMENT '角色结拜老师的ID';
alter table `roledata` add column `BrotherEndTime` int(10) unsigned NOT NULL Default '0' COMMENT '角色结拜结束时间';

############################## F-Plan 1.4.9 start############################################
# by zr
create table log_world_data
(
`Date` DATE not null,
data1	int(10) unsigned, 
data2	int(10) unsigned, 
data3	int(10) unsigned, 
data4	int(10) unsigned, 
data5	int(10) unsigned, 
data6	int(10) unsigned, 
data7	int(10) unsigned, 
data8	int(10) unsigned, 
data9	int(10) unsigned, 
data10	int(10) unsigned, 
data11	int(10) unsigned, 
data12	int(10) unsigned, 
data13	int(10) unsigned, 
data14	int(10) unsigned, 
data15	int(10) unsigned, 
data16	int(10) unsigned, 
data17	int(10) unsigned, 
data18	int(10) unsigned, 
data19	int(10) unsigned, 
data20	int(10) unsigned, 
data21	int(10) unsigned, 
data22	int(10) unsigned, 
data23	int(10) unsigned, 
data24	int(10) unsigned, 
data25	int(10) unsigned, 
data26	int(10) unsigned, 
data27	int(10) unsigned, 
data28	int(10) unsigned, 
data29	int(10) unsigned, 
data30	int(10) unsigned, 
data31	int(10) unsigned, 
data32	int(10) unsigned, 
data33	int(10) unsigned, 
data34	int(10) unsigned, 
data35	int(10) unsigned, 
data36	int(10) unsigned, 
data37	int(10) unsigned, 
data38	int(10) unsigned, 
data39	int(10) unsigned, 
data40	int(10) unsigned, 
data41	int(10) unsigned, 
data42	int(10) unsigned, 
data43	int(10) unsigned, 
data44	int(10) unsigned, 
data45	int(10) unsigned, 
data46	int(10) unsigned, 
data47	int(10) unsigned, 
data48	int(10) unsigned, 
data49	int(10) unsigned, 
data50	int(10) unsigned, 
data51	int(10) unsigned, 
data52	int(10) unsigned, 
data53	int(10) unsigned, 
data54	int(10) unsigned, 
data55	int(10) unsigned, 
data56	int(10) unsigned, 
data57	int(10) unsigned, 
data58	int(10) unsigned, 
data59	int(10) unsigned, 
data60	int(10) unsigned, 
data61	int(10) unsigned, 
data62	int(10) unsigned, 
data63	int(10) unsigned, 
data64	int(10) unsigned, 
data65	int(10) unsigned, 
data66	int(10) unsigned, 
data67	int(10) unsigned, 
data68	int(10) unsigned, 
data69	int(10) unsigned, 
data70	int(10) unsigned, 
data71	int(10) unsigned, 
data72	int(10) unsigned, 
data73	int(10) unsigned, 
data74	int(10) unsigned, 
data75	int(10) unsigned, 
data76	int(10) unsigned, 
data77	int(10) unsigned, 
data78	int(10) unsigned, 
data79	int(10) unsigned, 
data80	int(10) unsigned, 
data81	int(10) unsigned, 
data82	int(10) unsigned, 
data83	int(10) unsigned, 
data84	int(10) unsigned, 
data85	int(10) unsigned, 
data86	int(10) unsigned, 
data87	int(10) unsigned, 
data88	int(10) unsigned, 
data89	int(10) unsigned, 
data90	int(10) unsigned, 
data91	int(10) unsigned, 
data92	int(10) unsigned, 
data93	int(10) unsigned, 
data94	int(10) unsigned, 
data95	int(10) unsigned, 
data96	int(10) unsigned, 
data97	int(10) unsigned, 
data98	int(10) unsigned, 
data99	int(10) unsigned, 
data100	int(10) unsigned
)ENGINE=MyISAM DEFAULT CHARSET=utf8;
insert into log_world_data (`Date`) values(1);

# by ZR
create table log_role_data
(
RoleID	int(10) unsigned not null,
data1	int(10) unsigned, 
data2	int(10) unsigned, 
data3	int(10) unsigned, 
data4	int(10) unsigned, 
data5	int(10) unsigned, 
data6	int(10) unsigned, 
data7	int(10) unsigned, 
data8	int(10) unsigned, 
data9	int(10) unsigned, 
data10	int(10) unsigned, 
data11	int(10) unsigned, 
data12	int(10) unsigned, 
data13	int(10) unsigned, 
data14	int(10) unsigned, 
data15	int(10) unsigned, 
data16	int(10) unsigned, 
data17	int(10) unsigned, 
data18	int(10) unsigned, 
data19	int(10) unsigned, 
data20	int(10) unsigned
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

alter table fabao change column MaxIntelligence `MaxIntelligence` smallint(3) unsigned DEFAULT '0' COMMENT '资质上限';
alter table fabao change column `NativeIntelligence` `NativeIntelligence` smallint(3) NOT NULL COMMENT '法宝资质';

alter table log_world_data add unique index (`Date`);
alter table log_world_data change column `Date` `Date` int(10) not null;
alter table log_role_data drop column `Date`;

############################## F-Plan 1.4.11 begin############################################
#增加角色经脉 by zs
alter table `roledata` add column PulseRemainTime int(10) unsigned NOT NULL Default '0' COMMENT '经脉剩余学习次数';

# by zr
alter table account_common add column EMacc tinyint(2) default 0;

############################## F-Plan 1.5.0 begin############################################

alter table equip add column GrowID int(10) default 0; 
alter table equip add column GrowValue int(10) default 0;
alter table equip add column GrowLevel tinyint(2) default 0;
alter table equip add column GrowTimes int(10) default 0;
alter table equip add column GrowUseIMTimes int(10) default 0;
alter table equip add column GrowStars tinyblob;
alter table equip add column GrowAtt tinyblob;


alter table equip_del add column GrowID int(10) default 0; 
alter table equip_del add column GrowValue int(10) default 0;
alter table equip_del add column GrowLevel tinyint(2) default 0;
alter table equip_del add column GrowTimes int(10) default 0;
alter table equip_del add column GrowUseIMTimes int(10) default 0;
alter table equip_del add column GrowStars tinyblob;
alter table equip_del add column GrowAtt tinyblob;


alter table equip_baibao add column GrowID int(10) default 0; 
alter table equip_baibao add column GrowValue int(10) default 0;
alter table equip_baibao add column GrowLevel tinyint(2) default 0;
alter table equip_baibao add column GrowTimes int(10) default 0;
alter table equip_baibao add column GrowUseIMTimes int(10) default 0;
alter table equip_baibao add column GrowStars tinyblob;
alter table equip_baibao add column GrowAtt tinyblob;


# by zsu
insert into gm_server_data value ('TigerProfit',0);
alter table `roledata` add column TigerTime int(10) unsigned NOT NULL Default '0' COMMENT '老虎次数';

# by zr 1.5.0
create table SoulCrystal
(
	`SerialNum` bigint(20) NOT NULL COMMENT '64位id',
	`SoulLevel` tinyint(2) not null default 0,
	`SoulQlty` tinyint(3) unsigned not null,
	`SoulSkillID` int(10) unsigned not null,
	`SoulAttID1` int(10) unsigned default 0,
	`SoulAttID2` int(10) unsigned default 0,
	`SoulAttID3` int(10) unsigned default 0,
	`SoulAttID4` int(10) unsigned default 0,
	`SoulAttID5` int(10) unsigned default 0,
	`SoulAttID6` int(10) unsigned default 0,
	`SoulAttID7` int(10) unsigned default 0,
	`SoulAttID8` int(10) unsigned default 0,
	primary key(`SerialNum`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='魂晶';

create table Account_Reactive
(
	accountid int(10) unsigned NOT NULL COMMENT '账号ID',
	keycode int(10) unsigned not null,
	reward int(10) unsigned default 0,
	primary key(accountid),
	rewardlevel tinyint(2) unsigned default 0,
	key(keycode)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

create table Account_Reactiver
(
	accountid int(10) unsigned NOT NULL COMMENT '账号ID',
	keycode int(10) unsigned not null default 0,
	level tinyint(3) unsigned not null default 0,
	exp int(10) unsigned not null default 0,
	primary key(accountid)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

alter table `roledata` add column TigerTime int(10) unsigned NOT NULL Default '0' COMMENT '老虎次数';

# by ltt 1.5.0
alter table `roledata` add column `god_soul` int(10) NOT NULL Default '0' COMMENT '神魂';
alter table `roledata` add column `monster_soul` int(10) NOT NULL Default '0' COMMENT '魔魂';
alter table `roledata` add column `god_point` int(10) NOT NULL Default '0' COMMENT '神魂声望';
alter table `roledata` add column `monster_point` int(10) NOT NULL Default '0' COMMENT '魔魂声望';
alter table `skill` add column `active_time` int(10) unsigned DEFAULT '0' COMMENT '激活时间';


################################鲜花鸡蛋###bylib######################################	
# 创建鲜花排行记录表
CREATE TABLE `like_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`flowerNum` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '获得鲜花数量',
	`RoleName` varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '角色名字',
	`RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',
	KEY (`Num`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='等级排行';
	
# 创建鸡蛋排行记录表
CREATE TABLE `hate_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`EggNum` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '加成过的等级值，用于等级排名',
	`RoleName` varchar(32) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '角色名字',
	`RoleID` int(10) unsigned NOT NULL Default '0' COMMENT '角色ID',
	KEY (`Num`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='等级排行';

#鲜花鸡蛋排行
alter table roledata add column	flower_num bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '鲜花数' after monster_point;
alter table roledata add column egg_num bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '鸡蛋数' after flower_num;

#添加上一次鲜花鸡蛋排行记录
create table like_rankings_pre like like_rankings;
create table hate_rankings_pre like hate_rankings;

#添加宝石增幅相关内容
alter table equip add column DiamondLevel tinyint(2) default 0;

alter table equip_del add column DiamondLevel tinyint(2) default 0;

alter table equip_baibao add column DiamondLevel tinyint(2) default 0;

#1.4.4 补item_baibao和equip_baibao的删除时间字段
############################## F-Plan DB begin############################################
alter table `item_baibao` add column `del_time` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间' after `ItemSpecVal2`;
alter table `equip_baibao` add column `deltime` char(20) NOT NULL DEFAULT '2011-05-01 00:00:00' COMMENT '删除道具的时间' after `GodStrengthenTime`;
############################## F-Plan DB end############################################

############################## F-Plan 1.5.8 工会添加两个属性供策划脚本使用#################
alter table `guild` add column `GuildValue1` int(10) default 0 COMMENT '策划需求数值1' after `DailyRewardTakenTimes`;
alter table `guild` add column `GuildValue2` int(10) default 0 COMMENT '策划需求数值2' after `GuildValue1`;

alter table `fabao` add column `ElePierce` int(10) Default '0' COMMENT '元素伤害穿透值' after `EleInjury`;
alter table `fabao_extend_his` add column `ElePierce` int(10) Default '0' COMMENT '元素伤害穿透值' after `EleInjury`;
alter table `fabao` add column `EleInjPromoteTimes` int(10) Default '0' COMMENT '伤害提升次数' after `ElePierce`;
alter table `fabao_extend_his` add column `EleInjPromoteTimes` int(10) Default '0' COMMENT '伤害提升次数' after `ElePierce`;
alter table `fabao` add column `EleResPromoteTimes` int(10) Default '0' COMMENT '抗性提升次数' after `EleInjPromoteTimes`;
alter table `fabao_extend_his` add column `EleResPromoteTimes` int(10) Default '0' COMMENT '抗性提升次数' after `EleInjPromoteTimes`;

############################# F-Plan 1.5.12 新Key码相关###################################
CREATE TABLE `UseKeyCode_roleID` (                         
             `RoleID` int(10) unsigned NOT NULL COMMENT '角色id'  ,
	      primary key(`RoleID`)           
           ) 

############################## F-Plan 1.6.0 end############################################ 
##元神表
CREATE TABLE  `HolySoulInfo`(  
              `SoulID` int(10) unsigned NOT NULL  COMMENT '当前元神ID',
	      `RoleID`  int(10) unsigned NOT NULL  COMMENT '角色ID',
              `CurLevelExp` bigint(20) NOT NULL COMMENT '当前经验等级',
              `CurSoulValue` int(10) unsigned NOT NULL  COMMENT '当前元神值',
              `CurCovalue` int(10) unsigned NOT NULL  COMMENT '当前默契值',       
              `HolySoulState`   int(10)  NOT NULL  COMMENT '元神状态',
              `RebornLv`  int(10)  NOT NULL  COMMENT '转身等级',
              `SoulLevel`  int(10)  NOT NULL  COMMENT '元神等级',
              `Demage`  int(10)  NOT NULL  COMMENT '元神绝技类伤害',
              `Hit`  int(10)  NOT NULL  COMMENT '攻击命中',
              `Crit`  int(10)  NOT NULL  COMMENT '攻击致命',
              `CritAmount`  int(10) unsigned NOT NULL  COMMENT '攻击致命量',
	      `Exattack`  int(10) unsigned NOT NULL  COMMENT '外功攻击',
              `Inattack`  int(10) unsigned NOT NULL  COMMENT '内功攻击',
              `Accrate`  int(10) unsigned NOT NULL  COMMENT '精准',
              `Exdamage`  int(10) unsigned NOT NULL  COMMENT '额外伤害',
              `Toughness`  int(10) unsigned  NOT NULL  COMMENT '忽视韧性',
              `Morale`  int(10) unsigned  NOT NULL  COMMENT '士气',
               PRIMARY KEY (`RoleID`)                                
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;  

##元神装备强化属性
# 元神装备强化冲星属性    
alter table `equip`        add column `byStarEnhanceLvl`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '冲星等级';
alter table `equip_baibao` add column `byStarEnhanceLvl`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '冲星等级';
alter table `equip_del`    add column `byStarEnhanceLvl`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '冲星等级';

##元神装备凿孔属性   
alter table `equip`        add column `byHolyHoleNum`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '元神装备当前孔数';
alter table `equip_baibao` add column `byHolyHoleNum`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '元神装备当前孔数';
alter table `equip_del`    add column `byHolyHoleNum`	tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '元神装备当前孔数';

##圣灵表
CREATE TABLE `holyman` (                                          
          `SerialNum` bigint(20) NOT NULL COMMENT '装备序列号',   
	  `DevourNum` int(10) NOT NULL COMMENT '吸收妖精的当前次数', 
	  `EquipmentNumber` smallint(4) NOT NULL COMMENT '圣灵可装备道具数量',
	  `ToDayDevourNum` int(10) NOT NULL COMMENT '当天吸收妖精的次数',  
	  `CoValue` int(10) NOT NULL COMMENT '默契值', 	  
	  `HolyDmg` int(10) NOT NULL COMMENT '圣灵伤害值', 
	  `HolyDef` int(10) NOT NULL COMMENT '圣灵防御值', 
	  `Crit` int(10) NOT NULL COMMENT '致命', 
	  `HolyCritRate` int(10) NOT NULL COMMENT '致命量', 
	  `ExDamage` int(10) NOT NULL COMMENT '伤害加深', 
	  `AttackTec` int(10) NOT NULL COMMENT '精准', 
	  `NeglectToughness` int(10) NOT NULL COMMENT '灵巧', 
	  `HolyValue` int(10) NOT NULL COMMENT '灵能值', 	  
          PRIMARY KEY (`SerialNum`)                                     
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
alter table roledata add column holy_value int(10) unsigned NOT NULL DEFAULT '0' COMMENT '圣灵值' after egg_num;

##圣纹表
CREATE TABLE `holyequip` (                                          
          `SerialNum` bigint(20) NOT NULL COMMENT '装备序列号',   
	  `CostHoly` int(10) NOT NULL COMMENT '装备该圣纹要消耗的灵能', 	  
	  `HolyDmgChg` int(10) NOT NULL COMMENT '圣灵伤害影响值', 
	  `HolyDefChg` int(10) NOT NULL COMMENT '圣灵防御影响值', 
	  `CritChg` int(10) NOT NULL COMMENT '致命影响值', 
	  `HolyCritRateChg` int(10) NOT NULL COMMENT '致命量影响值', 
	  `ExDamageChg` int(10) NOT NULL COMMENT '伤害加深影响值', 
	  `AttackTecChg` int(10) NOT NULL COMMENT '精准影响值', 
	  `NeglectToughnessChg` int(10) NOT NULL COMMENT '灵巧影响值', 	   
          PRIMARY KEY (`SerialNum`)                                     
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

alter table holyman add column EquipSerialIDs tinyblob COMMENT '圣纹64位Id' after ToDayDevourNum;
alter table holyman add column CostHoly int(10) unsigned NOT NULL DEFAULT '0' COMMENT '装备圣纹所消耗的灵能' after EquipSerialIDs;
alter table holyequip add column EnhanceCount smallint(4) unsigned NOT NULL DEFAULT '0' COMMENT '圣纹的充能次数' after CostHoly;


############################## F-Plan 1.6.7 Start ########################################
alter table holyman add column MaxDevourNum int(10) unsigned NOT NULL DEFAULT '80' COMMENT '最大吃妖精次数' after HolyValue;
############################## F-Plan 1.6.7 end ##########################################

alter table roledata add column role_hit_add int(10) unsigned NOT NULL DEFAULT '0' COMMENT '命中' after holy_value;

############################# F-Plan 1.6.10 ###################################
CREATE TABLE `ReciveYuanBaoDaiBi` (                         
             `RoleID` int(10) unsigned NOT NULL COMMENT '角色id' ,
             `Num` int(10) unsigned NOT NULL COMMENT '可领取的数量' ,
	      primary key(`RoleID`)           
           ) 
           
############################## F-Plan 1.6.10 end ##########################################

alter table roledata add column role_eei_all int(10) unsigned NOT NULL DEFAULT '0' COMMENT '元素通用伤害' after role_hit_add;
