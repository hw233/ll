#pragma once
#include ".\ChatSysDefine.h"

#include "EmotionFrame.h"
#include "PlayerNameTab.h"
#include "StaticNoInput.h"
#include "..\WorldDefine\role_info.h"
#include "..\WorldDefine\chat.h"
#include "..\WorldDefine\msg_show_item.h"
#include "ChatEvent.h"
#include "MultChatFrame.h"

//-----------------------------------------------------------------------------
//!class ChatFrame.h
//!brief 聊天框架
//-----------------------------------------------------------------------------
class NetSession;
struct tagPrivateChatEvent;
struct tagSocialInfoEvent;
struct tagDungeonEvent;
struct tagChannelSetEvent;
struct tagUpdataMiniChatEvent;
class RichEditBoxForChatInput;
class RichEditBoxForChatShow;
class ScrollRichEditBox;
class StaticForCast;
class HttpPicEx;
class GlintPushButton;
class GlintButton;

struct tagNPCEaseSpeechEvent;
struct tagNS_MonsterEnterCombat;
struct tagNS_RoleDead;
struct tagNS_Skill;
struct tagGroupChatButtonGlin;
struct tagNS_SendToPrisonEx;
struct tagNS_SomeoneViewYourEquip;

//------------------------密语相关-----------------------------------------
struct tagMiniMsg               //存储密语信息的结构
{
	DWORD                        RoleID;          //对方ID
	tstring                      Minimsg;         //所存的信息
	BOOL                         isRead;          //是否有新消息
};
static std::list<tagMiniMsg>           MiniChatMsg;   //定义tagMiniMsg类型结构用于存储10个人的ID、聊天内容、所有信息是否已读
static std::list<tagMiniMsg>::iterator MiniChatMsgit; //迭代器

static tstring                    m_NowChatWith;      //记录当前正在聊天的对象
static DWORD                      m_NowRoleID;        //当前聊天对象的ID
//----------------------------------------------------------------------------
class ChatFrame:
	public GameFrame
{
public:
	enum SendMsgState
	{
		SendMsgState_NULL = 0,
		SendMsgState_Chat,
		SendMsgState_Show,
	};
public:
	ChatFrame(void);
	~ChatFrame(void);
	static ChatFrame* Inst();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Render3D();
	virtual VOID Update();
	virtual BOOL EscCancel();
	//!切换中英文提示图标,isEnglish:是否是英文
	VOID SwitchLangueInfo(bool isEnglish=true);

	//!得输入框中输入的的文字,在发送聊天内容时使用
	VOID GetInputText(tstring& str);	

	//获得发言频道
	ESendChatChannel GetSendChannel(){return m_curSendChannel;}

	//获得显示频道
	DWORD GetRecvChannel(){return m_curRecvChannel;}

	//过滤格式自定义格式字符串
	VOID FilterMarkerString(tstring&  strText);

	//删除richEditBox为了进行排版在文字（strText）里自动加上的回车键
	VOID DeleteRETURNS(tstring& strText);

	//设置滚动文本，默认时间为40second(时间未定)
	VOID PushScrollText(LPCTSTR szText, float fTime = 20.0f);

	//设置大跑马灯的滚动文本，默认时间为40second(时间未定)
	VOID PushBigScrollText(LPCTSTR szText, DWORD dwPriority, float fTime = 20.0f,DWORD colour = 0xFFFF80FF);

	//设置大跑马灯让它播放完此次后不再继续播放
	VOID BigScrollTextCancelAfterThisTime();

	//立即显示40second
	VOID PushBigScrollTextImmediately(LPCTSTR szText, DWORD dwPriority,float fTime = 20.0f,DWORD colour = 0xFFFF80FF);

	//查看大跑马灯当前信息的优先级，如果没有返回0（优先级从1开始）
	DWORD BigRunHorseCurrentPriority(){return m_dwCurrentRunHorsePriority;};

	//设置传音，默认时间为1分钟(时间未定)
	VOID PushCastText(LPCTSTR szText, DWORD dwRoleID, float fTime = 30.0f);

	//添加从服务器接受的聊天数据到相应的显示频道
	VOID PushInfo(const TCHAR* strText, ESendChatChannel escc= ESCC_End);

	//!为RichEditBox注册图片（转义字符）
	//!这里由于使用的是固定路径，而且按序号排好图片所以只需要index
	void RegisterPicForRichEdit(const TCHAR *szName,int index);

	//获得输入框当前输入文本的大小
	int GetInputValidTextSize();

	//接收服务器发送的聊天信息
	DWORD NetRecvMsgNS_RoleChat( tagNS_RoleChat* pMsg, DWORD dwParam );

	void OnSendChatMsg(const tstring& strMsg, ESendChatChannel eChannel, DWORD dwDestID=GT_INVALID, BYTE byAutoReply=0);

	//接收win32消息
	void OnWin32Msg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);

	void SetPvtRoleName(const TCHAR* name);

	HKL GetEnglishHKL() { return m_HKLenglish; }
	
	void OnOpenMiniChat(tagOpenMiniChatEvent* e);
	DWORD OpenMiniChat( DWORD dwID, BOOL bDown );
	void UpdataNowchat();


private:
	DWORD EventHandler(tagGUIEvent* pEvent);
	BOOL GUIEventHandlerMenu(tagGUIEvent* pEvent);

	DWORD OnGameEventForChat(tagGameEventForChat* pEvent);

	//--得到角色名字、ID
	DWORD OnRecvNetRoleGetName( tagRoleGetNameEvent* pMsg);
	DWORD OnRecvNetRoleGetID( tagRoleGetIDEvent* pMsg);
	//--得到角色的贴图URL
	DWORD OnRecvHeadpicByID( tagGetHeadpicByID* pGameEvent );
	//目标窗口设置私聊
	DWORD OnPrivateChatEvent(tagPrivateChatEvent* pEvent);
	//接收好友上下线提示
	DWORD OnSocialInfoEvent(tagSocialInfoEvent* pEvent);
	//牢狱广播
	DWORD OntagDungeonEvent(tagDungeonEvent* pEvent);
	// NPC休闲动作说话
	DWORD OnNPCEaseSpeechEvent(tagNPCEaseSpeechEvent* pEvent);
	// 频道设置
	DWORD OnChannelSetEvent(tagChannelSetEvent* pGameEvent);
	// 留言
	DWORD OnLeaveMsg(tagGameEvent* pGameEvent);
	// 名字改变
	DWORD OnRoleGetNameEvent(tagRoleRenameEvent* pEvent);

	//初始化时设置聊天显示频道
	VOID ResetRecvChannel();

	//设置当前说话频道按钮显示
	VOID SetCurChannel(ESendChatChannel eChannel);

	//将发言频道转换为客户端显示频道
	DWORD TranIntEChat2EClient(ESendChatChannel eChannel);

	//输入框输入内容发生变化
	VOID OnInputContextChange();

	//处理命令行切换聊天频道
	ESendChatChannel TranCMD2SendChannel(int nChannel);

	//切换聊天显示频道，玩家通过发言频道listbox选择
	VOID OnSwitchPageShow(int index);

	//点击显示频道接钮时，自动切换发言频道的转换函数，用于界面操作
	ESendChatChannel TranRecv2SendChannel(int nRecvChannel);

	//将聊天信息添加到显示频道
	VOID AddStringToWnd(const tstring& strText, int nPate, ESendChatChannel escc);
	//将聊天信息添加到公告显示频道
	VOID AddStringToAffiche(const tstring& strText,ESendChatChannel escc);
	//向输入框添加表情
	void AddInputEmotion(const tstring &str);

	//处理私聊，传入有玩家名字的字符串，返回玩家名字
	tstring HandleStringForPvt(tstring& strText);

	//--发言
	void OnSendCastTalk();
	void OnSendWorld();
	void OnSendPvt();
	void OnSendGuild();
	void OnSendTroop();
	void OnSendCurrent();
	void OnSendAffiche();
	void OnSendScene();
	void OnSendFamily();


	//是否重复发言
	bool IsViladeSpeak(const tstring& str,ESendChatChannel escc);

	//世界频道发言是否间隔小于10秒
	bool IsIntervalSpeak(ESendChatChannel escc);

	//得到相应频道的字体颜色
	const TCHAR* GetChatChannelColor(ESendChatChannel escc);

	//隐藏窗口
	void HideWnd();
	//显示窗口
	void ShowWnd();

	//--展示装备或物品
	void OnRoleSendItemOrEquip(ShowEvent* pEvent);
	DWORD NetRecvMsgNS_RoleShowEquip(tagNS_RoleShowEquip* pMsg, DWORD dwParam);
	DWORD NetRecvMsgNS_RoleShowItem(tagNS_RoleShowItem* pMsg, DWORD dwParam);
	DWORD OnNS_TemmatePickUpBroadCast( tagNS_RoleShowDroptItemInTeam* pMsg, DWORD dwParam );
	// 查看装备
	DWORD OnNS_SomeoneViewYourEquip( tagNS_SomeoneViewYourEquip* pMsg, DWORD dwParam );
	void OnRoleShowItem(DWORD data, const TCHAR* szSrcRole, const TCHAR* szDstRole, ESendChatChannel escc);
	void OnRoleShowEquip(const Equipment& data, const TCHAR* szSrcRole, const TCHAR* szDstRole, ESendChatChannel escc);

	//--个性动作
	BOOL JudePersonalityAct();

	//重新设置焦点
	void SetActive();

	//自动回复
	void AutoReplyPvt(DWORD dwRoleID, LPCTSTR strText);

	void ForceResizeMenu( GUIListBox* pListBox, const INT nNumRow );
	void ForceMoveMenu(const tagPoint& pos);

	//NPC或怪说话
	DWORD OnNS_MonsterEnterCombat(tagNS_MonsterEnterCombat* pMsg, DWORD dwParam);
	DWORD OnNS_RoleDead(tagNS_RoleDead* pMsg, DWORD dwParam);
	DWORD OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);

	//系统发布的跑马灯
	void OnNS_AutoNotice(tagNS_AutoNotice* pMsg, DWORD dwParam);
	tstring AutoNoticeColor(INT nType);
	//
	void NetRecvMessage_SendToPrisonEx(tagNS_SendToPrisonEx* pMsg, DWORD pParam);

	//频道设置
	void ChannelSet();
	void SaveChannelSet();

	//显示密语窗口
	void ShowMiniChat();

	//快捷键设置
	DWORD OnKey(DWORD dwID, BOOL bDown);
	DWORD OnKeyChange(DWORD dwID, DWORD key1, DWORD key2);
	DWORD OnHideOtherPlayers( DWORD dwID, BOOL bDown );
	
	//判断是否有世界喊话工具，
	//工具typeid写死在客户端
	BOOL HaveWorldSpeakTool();

	void OnGroupChatButtonGlin(tagGroupChatButtonGlin* e);

	BOOL CheckSystemPic();	//检查是否是公告，系统图片

	void UpDateFangChenMi();	// 韩国防沉迷

private:

	TSFPTrunk<ChatFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameInputMap>		m_pKeyMap;

	GUIWnd*						m_pWnd;
	GUIStatic*					m_pWndCaption;
	GUIWnd*						m_pSclFatherWnd;			//跑马灯你控件
	GUIButton*					m_pBtnHide;					//隐藏按钮
	GUIButton*					m_pBtnChannelSet;			//频道设置按钮
	//GlintButton*				m_pBtnLeave;				//留言
	GlintButton*                m_pBtnMiniChat;             //密语聊天按钮
	GUIButton*					m_pBtnCurChannel;			//当前发言频道按钮
	GUIButton*					m_pBtnEmotion;				//表情按钮
	GUIStatic*					m_pStaticLangue[2];			//中英文提示
	RichEditBoxForChatInput*	m_pRebInput;				//聊天输入框
	GUIStatic*					m_pInpuBack;				//输入框背景
	GUIListBox*					m_pChannelSel;				//发言频道选择
	GUIWnd*						m_pWndChannel;				//listbox父窗体
	GUIPatch*					m_pMidBack;					//
	ScrollRichEditBox*			m_pScrollRichText;			//跑马灯
	GUIButton*                m_pBn_StyleAction;            //个性动作按钮
	GUIWnd*						m_pRunHorseWnd;				//跑马2控件
	ScrollRichEditBox*			m_pRunHorseScrollText;		//跑马灯2
	DWORD						m_dwCurrentRunHorsePriority;		//当前大跑马灯优先级

	GUIFrameMutex*				m_pMutex;					//接收频道的互斥控件
	//GUIPushButton*				m_pPage[NUM_CHAT_CHANNEL];	//对就显示频道的pushbutton
	GlintPushButton*				m_pPage[NUM_CHAT_CHANNEL];

	RichEditBoxForChatShow*		m_pChatContext[NUM_CHAT_CHANNEL];				//聊天的显示(接收)频道
	GUIScrollBar*				m_pScl[NUM_CHAT_CHANNEL];	//对应的8个滚动条
	GUIButton*					m_pBtnUp[NUM_CHAT_CHANNEL];	//对应滚动条的向上按钮
	GUIButton*					m_pBtnDp[NUM_CHAT_CHANNEL];	//对应滚动条的向下按钮

	GUIWnd*						m_pWndHeadPic;				//传音头像
	GUIWnd*						m_pWndImName;				//传音姓名
	StaticForCast*				m_pStcName[NUM_CHAT_CAST_WND];//传音玩家的姓名
	HttpPicEx*					m_pStcHeadPic[NUM_CHAT_CAST_WND];//传音减低
	GUIWnd*						m_pStcCast;
	ScrollRichEditBox*			m_pCastRichText[NUM_CHAT_CAST_WND];	//传音
	GUIButton*					m_pBtnZoomUp;					//缩放按钮向上
	GUIButton*					m_pBtnZoomDn;					//缩放按钮向下
	GUIPushButton*				m_pBtnHideOthers;				// 隐藏其他玩家
	
	GUIWnd*						m_pWndWorld;
	GUIButton*					m_pBtnWorldOK;
	GUIButton*					m_pBtnWorldCancel;
	GUIPushButton*				m_pPBtnWorld;
	GUIStatic*					m_pStcExWold;

	GUIWnd*						m_pWndMenuEx;					//扩展菜单
	GUIListBox*					m_pListMenuEx;					//菜单选项

	INT							m_nZoomMax;

	BOOL						m_bChatContext;
	BOOL						m_bHide;

	
	ESendChatChannel			m_curSendChannel;			//记录当前发言频道
	int							m_curRecvChannel;			//当前显示频道
	DWORD						m_dwRecvChalShow[NUM_CHAT_CHANNEL];			//当前显示频道过滤	
	
	LONG						m_lnSlcFatherWndX;			// 跑马灯父控件的client坐标
	LONG						m_lnSlcFatherWndY;			// 跑马灯父控件的client坐标
	LONG						m_lnCastWndX;				//传音父控件的client坐标
	LONG						m_lnCastWndY;

	EmotionFrame*				m_pEmotionFrame;			//表情框架

	
	vector<tagRecvSpeak>		m_vecRecv;					//接受的聊天数据缓存，保存到正确显示出来为止
	vector<tagSendSpeak>		m_vecSpeak;					//已经发出的发言，保留一定时间
	vector<tagSendSpeak>		m_vecPvtSpeak;				//私聊说的话
	vector<tagScroText>			m_vecSclText;				//从服务器接收跑马灯消息缓存
	vector<tagScroText>			m_vecBigSclText;				//从服务器接收跑BIG马灯消息缓存
	vector<tagScroText>			m_vecCast;					//保存的传音
	vector<tagScroText>			m_vecCastNoName;			//保存的传音,没有得到角色名
	vector<tstring>				m_vecSomeoneViewYouEquip;	//保存别人查看你装备的信息...
	DWORD						m_dwLocalID;				//本地玩家ID
	tstring						m_strLocalName;				//本地玩家姓名
	tstring						m_strLastName;				//上一次私聊玩家姓名
	tstring						m_strMenuName;				//扩展菜单点击时的玩家姓名

	vector<tagChatShowEquip>	m_vecRecvEquip;				//接收的展示装备缓存
	vector<tagChatShowItem>		m_vecRecvItem;				//接收的展示物品缓存

	BOOL						m_bWorldOK;					//世界频道发言扣钱确认
	SendMsgState				m_eSendMsgState;
	ShowEvent					m_ShowEvent;
	HKL							m_HKL;
	HKL							m_HKLenglish;

	DWORD						m_dwViewEquipTime;

	//-------------------------密语的-----------------------------------------------------------------
	GUIWnd*                         m_pMiniChatWnd;
	GUIButton*                      m_pBtn_Exit;        //退出
	GUIButton*                      m_pBtn_Send;        //发送
	GUIButton*                      m_pBtn_SetObj;      //设置对象名字的按钮
	GUIListBox*                     m_pLB_ChatObj;      //聊天对象ListBox
	RichEditBoxForChatShow*         m_pWndText;         //聊天信息
	RichEditBoxForChatInput*	    m_pEB_SendMsg;      //发送信息框
	GUIEditBox*                     m_pEB_SetObj;       //设置聊天对象名字
	GUIPushButton*                  m_pPB_CloseTag;     //控制发送信息后是否关闭聊天窗口的按钮
	GUIStaticEx*                    m_pSE_ShowObjName;  //显示正在和谁聊天

	GUIScrollBar*                   m_pSB_MiniChat;     //聊天滚动条
	GUIButton*                      m_pBtn_up;          //向上滚屏
	GUIButton*                      m_pBtn_down;        //向下滚屏
	BOOL                            m_bisfromChat;      //表示从聊天窗口传进来的私聊信息(切勿更改)
	DWORD                           m_destID;           //向我发送消息的人的ID



	//---------------------------------密语函数声明---------------------------------------------
	public:
		virtual BOOL MiniChatReloadUI();
//		virtual VOID OnEvent(tagGameEvent* pEvent);
//		virtual VOID Update();
//		virtual BOOL EscCancel();
//		void OnSendChatMsg(const tstring& strMsg, ESendChatChannel eChannel, DWORD dwDestID, BYTE byAutoReply);
		void OnReceiveMsg(tstring name,tstring strMsg);                        //当接收到密语信息时做相应操作
// 		VOID DeleteRETURNS(tstring& strText);
// 		VOID Show();
//		void OnUpdataMiniChat(tagUpdataMiniChatEvent *pEvent);
		void UpdataMiniChat(DWORD roleID);
		void UpdataListBox();
		void setNowChatWith(tstring name,BOOL isresetname);                                     //设置当前正在跟谁聊天

//	private:
		//控件事件处理函数
//		DWORD MiniChatEventHandler(tagGUIEvent* pEvent);

		//---------------------------------群聊函数声明---------------------------------------------
	public:
		GlintButton*                   m_pBtnGroupChat;          //群聊按钮
		MultChatFrame*					 m_pMultChatFrame;
		//----------------------------隐藏按钮----------------------------
		GUIWnd						*m_pWndHide;			    // 隐藏菜单
		GlintButton*                 m_pBn_Hide;              //隐藏
		//----------------------------公告显示区----------------------------
		GUIStatic*					 m_pStcAffiche;			//公告牌
		RichEditBoxForChatShow*		 m_pShowAffiche;			//公告显示

	private:
		tagDWORDTime		m_stStartTime;
		int							m_nOnlineHour;
		
};
