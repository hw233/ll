################################# 1.2.0 start ########################################

##2009-07-03
#修改login_log表格式
alter table `login_log` drop column `accountName`, drop column `id`;

#修改login_log表格式
alter table `login_log` add column `accountName` varchar(32) CHARSET utf8 COLLATE utf8_general_ci NOT NULL after `accountID`;

#修改game_guarder_log表格式
alter table `game_guarder_log` add column `name` varchar(32) CHARSET utf8 COLLATE utf8_general_ci NOT NULL after `account_id`;

##2009-07-11
#增加world状态表
CREATE TABLE `world_state` (
  `worldid` bigint(11) NOT NULL DEFAULT '0' COMMENT '世界ID',
  `rolenum` int(11) NOT NULL DEFAULT '0' COMMENT '当前在线人数',
  `worldstate` tinyint(3) NOT NULL DEFAULT '0' COMMENT '(当前人数/最高负载)*100',
  `time` char(20) DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`worldid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='世界状态 不停的update';


##2009-07-20
#增加world状态log表
CREATE TABLE `world_state_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `worldid` bigint(11) NOT NULL DEFAULT '0' COMMENT '世界ID',
  `rolenum` int(11) NOT NULL DEFAULT '0' COMMENT '当前在线人数',
  `worldstate` tinyint(3) NOT NULL DEFAULT '0' COMMENT '(当前人数/最高负载)*100',
  `time` char(20) DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`)  
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='世界状态 不停的insert';

##2009-07-23
#修改登入登出log的表结构
alter table `login_log` change `loginTime` `action` varchar(6) character set utf8 collate utf8_general_ci NOT NULL comment '动作', 
						change `logoutTime` `time` datetime NULL  comment '时间';

################################# 1.2.0 end ##########################################

################################# 1.2.1 start ########################################

##2009-08-28
#修改封停标示位						
alter table `account` change `forbid` `forbid_mask` tinyint(4) UNSIGNED NOT NULL DEFAULT 0 comment '对应目前各种封停类型，掩码。';

##2009-08-28
#增加gm封停对应游戏世界的表
CREATE TABLE `world_forbid` (
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '账号id',
  `worldname_crc` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏世界名的crc',
  PRIMARY KEY (`accountid`,`worldname_crc`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='记录gm工具封停账号';

##2009-09-07
#增加ip封停黑名单
CREATE TABLE `black_list` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) DEFAULT NULL COMMENT '被封的ip',
  PRIMARY KEY (`id`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='黑名单';

##2009-09-18
#添加防沉迷时间表
CREATE TABLE `fatigue_time` (                                           
                `account_id` int(11) NOT NULL COMMENT '账号id',                     
                `acc_online_time` int(11) DEFAULT '0' COMMENT '累计在线时间',   
                `acc_offline_time` int(11) DEFAULT '0' COMMENT '累计离线时间',  
                PRIMARY KEY (`account_id`)                                            
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
              
#防沉迷时间数据
INSERT INTO fatigue_time(account_id)  (SELECT id FROM account ) on DUPLICATE key update acc_offline_time = 0, acc_online_time=0; 

################################# 1.2.1 end ##########################################

################################# 1.2.2 start ########################################

##2009-10-09
#修改查询上次登录ip和time慢的bug
alter table `account` add column `ip` varchar(20) DEFAULT NULL COMMENT '上次登录ip' after `mibao`;
alter table `account` add column `time` datetime DEFAULT NULL COMMENT '上次登录time' after `ip`;

##2009-10-10
#添加防沉迷时间表
drop table `fatigue_time`;

##2009-10-12
#添加防沉迷时间表
CREATE TABLE `fatigue_time` (                                           
                `accountname_crc` int(11) NOT NULL COMMENT '帐号名crc',            
                `acc_online_time` int(11) DEFAULT '0' COMMENT '累计在线时间',   
                `acc_offline_time` int(11) DEFAULT '0' COMMENT '累计离线时间',  
                PRIMARY KEY (`accountname_crc`)                                       
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

##2009-10-14
#修改了accountname_crc的类型
alter table `fatigue_time` change `accountname_crc` `accountname_crc` int(11) UNSIGNED NOT NULL comment '帐号名crc';

################################# 1.2.2 end ##########################################
 
################################# 1.3.2 start ########################################

#台湾金牌网吧游戏世界配置
drop table if exists `vnb_world`;

################################# 1.3.2 end ##########################################

################################# 1.3.3 start ##########################################
##2010-5-11
#添加ActiveX控件登入的验证表
CREATE TABLE `zinaccount` (                                
              `idxno` int(11) NOT NULL AUTO_INCREMENT COMMENT '备用',  
              `name` char(36) NOT NULL COMMENT 'UserName',             
              `sitecode` char(10) NOT NULL,                            
              `secrandkey` char(32) NOT NULL,                          
              PRIMARY KEY (`name`),                                    
              UNIQUE KEY `idxno` (`idxno`)                             
            ) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=utf8 ; 
################################# 1.3.3 end ##########################################

#################################F计划1.0.0 start ##########################################
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
#################################F计划1.0.0 end ##########################################


#################################F计划1.0.0 start##########################################
##2010-11-18
#账号表建立索引
create index idx_crc_status on account(worldname_crc,login_status);
#################################F计划1.0.0 end ##########################################

#################################F计划1.2.2 start##########################################
# Jason 2010-11-24 1.2.2 台湾金牌网吧表
CREATE TABLE `inetcafes` (                                                        
             `InetCafesID` int(11) NOT NULL AUTO_INCREMENT,                                  
             `InetCafesName` varchar(32) DEFAULT NULL COMMENT 'internet cafes name',                                       
             `BuffID` int(10) unsigned DEFAULT '4294967295' COMMENT 'CB buff id',                                 
             `InstMapBuffID` int(10) unsigned DEFAULT '4294967295' COMMENT '副本地图要加的增益状态',                          
             `IPRangeMin1` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMax1` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMin2` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMax2` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMin3` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMax3` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMin4` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMax4` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMin5` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             `IpRangeMax5` varchar(20) DEFAULT NULL COMMENT 'dotted ip address',             
             PRIMARY KEY (`InetCafesID`)                                                     
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='台湾金牌网吧';
           
#记录游戏世界状态的表中增加排队人数字段
alter table world_state_log add column queuesize smallint(10) NOT NULL default 0 COMMENT '当前排队人数' after `rolenum`;

#################################F计划1.2.2 end ##########################################

#################################F计划1.4.2 end ##########################################
#全区家族名字统一
CREATE TABLE  `family_name` (                                                                                          
              `name` varchar(32) NOT NULL  COMMENT '家族名字',   
               PRIMARY KEY (`name`)                                      
            ) ENGINE=MyISAM DEFAULT CHARSET=utf8 ; 
#################################F计划1.4.2 end ##########################################

#################################MAC黑名单 begin ##########################################
alter table `login_log` add column `mac` varchar(64) DEFAULT NULL COMMENT 'MAC地址' after `ip`;
    DROP TABLE IF EXISTS `black_mac`;
    CREATE TABLE `black_mac` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `mac` varchar(40) DEFAULT NULL COMMENT '被封的mac',
      PRIMARY KEY (`id`),
      UNIQUE KEY `mac` (`mac`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='黑名单';
#################################MAC黑名单 end ##########################################

#################################F计划1.5.0 start##########################################
#韩国的二次密码
alter table `account` add column `SecondPsd` char(50) NOT NULL COMMENT '二次密码' after `mibao`;
#################################F计划1.5.0 end##########################################

alter table login_log add column worldid int(10) unsigned;


#################################F计划1.6.0 start##########################################
alter table `account` add column `savetelnum` tinyint(4) NOT NULL DEFAULT '1' COMMENT '是否需要玩家填写手机号' after `time`;

#增加保存玩家手机号
CREATE TABLE `account_tel` (
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '账号id',
  `account_name` char(36) NOT NULL COMMENT '帐号名',
  `tel_num` bigint(20) NOT NULL DEFAULT '0' COMMENT '手机号',
  `time` datetime DEFAULT NULL COMMENT '存手机号时间',
  PRIMARY KEY (`accountid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='记录玩家手机号';