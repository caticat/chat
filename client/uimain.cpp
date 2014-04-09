#include "uimain.h"

#include "protocol.h"
#include "constant.h"
#include "uilogin.h"
#include "uichat.h"
#include "user.h"
#include "cuser.h"
#include "data.h"
#include "QtGui/QHBoxLayout"
#include "QtCore/QTimer"
#include "QtCore/QTextCodec"
#include <boost/thread.hpp>

extern CUser client;
extern std::list<CCUser> g_userList;
extern boost::mutex g_userListMutex; // 角色列表 互斥量

UIMain::UIMain(QWidget* parent) : QWidget(parent)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	m_pUiLogin = new UILogin(parent);

	//connect(pUiLogin,SIGNAL(onLoginClicked(QString)),this,SLOT(onLoginClicked(QString)));

	m_pTimer = new QTimer(this);
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(timeout()));

	m_pUiChat = new UIChat(parent);
	m_pUiChat->hide();

	m_pUiMainLayout = new QHBoxLayout;
	m_pUiMainLayout->addWidget(m_pUiLogin);
	setLayout(m_pUiMainLayout);

	m_pTimer->start(5);
}

// 登陆返回
void UIMain::resLogin(CMsg& msg)
{
	printf("login success\n");
	m_pUiLogin->hide();
	m_pUiMainLayout->removeWidget(m_pUiLogin);
	m_pUiChat->show();
	m_pUiMainLayout->addWidget(m_pUiChat);
}

void UIMain::resNormalChat(CMsg& msg)
{
	CPResNormalChat nchat;
	INIT_CP(nchat);
	msg.GetData(nchat);
	printf("msg from server is:%s\n",nchat.chat);
	m_pUiChat->AddMsg(nchat);
}

void UIMain::resUserLogin(CMsg& msg)
{
	CPResUserLogin userLogin;
	INIT_CP(userLogin);
	msg.GetData(userLogin);
	printf("othersock:%d,name:%s\n",userLogin.sock,userLogin.name);
	CCUser user;
	user.m_sock = userLogin.sock;
	strcpy_s(user.m_name,CONST_MAX_NAME_LEN,userLogin.name);
	{
		boost::mutex::scoped_lock lock(g_userListMutex);
		g_userList.push_back(user);
	}
	AddUser(user);
}

// 获取用户列表
void UIMain::resUserList(CMsg& msg)
{
	CPResUserList uList;
	INIT_CP(uList);
	msg.GetData(uList);
	printf("get user list.");
	CCUser user;
	boost::mutex::scoped_lock lock(g_userListMutex);
	for (int i = 0; i < uList.userSize; ++i)
	{
		if (uList.user[i].sock != client.GetSock())
		{
			user.m_sock = uList.user[i].sock;
			strcpy_s(user.m_name,CONST_MAX_NAME_LEN,uList.user[i].name);
			g_userList.push_back(user);
			AddUser(user);
		}
	}
}

// 用户退出
void UIMain::resUserLogout(CMsg& msg)
{
	CPResUserLogout uLogout;
	INIT_CP(uLogout);
	msg.GetData(uLogout);
	RemoveUser(uLogout.sock);
}

// 私聊
void UIMain::resWisper(CMsg& msg)
{
	CPResWisper sWisper;
	INIT_CP(sWisper);
	msg.GetData(sWisper);
	m_pUiChat->AddMsg(sWisper);
}

// 增加用户
void UIMain::AddUser(CCUser& user)
{
	m_pUiChat->AddUser(user);
}

// 删除用户
void UIMain::RemoveUser(int userSock)
{
	boost::mutex::scoped_lock lock(g_userListMutex);
	std::list<CCUser>::iterator it = g_userList.begin();
	for (; it != g_userList.end(); ++it)
	{
		if (it->m_sock == userSock)
			break;
	}
	m_pUiChat->RemoveUser(it->m_name);
	g_userList.erase(it);
}

// 定时器
void UIMain::timeout()
{
	if (client.IsEmpty())
		return;
	CMsg msg = client.PopMsg();
	CTITLE_HUB(EPRes_Login,resLogin,msg);
	CTITLE_HUB(EPRes_NormalChat,resNormalChat,msg);
	CTITLE_HUB(EPRes_UserLogin,resUserLogin,msg);
	CTITLE_HUB(EPRes_UserList,resUserList,msg);
	CTITLE_HUB(EPRes_UserLogout,resUserLogout,msg);
	CTITLE_HUB(EPRes_Wisper,resWisper,msg);
}
