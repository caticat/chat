#pragma once

#include "QtGui/QWidget"

class QHBoxLayout;
class QTimer;
class UILogin;
class UIChat;
class CCUser;
struct CPResNormalChat;
class CMsg;
class UIMain : public QWidget
{
	Q_OBJECT
public:
	UIMain(QWidget* parent = 0);

public:
	void resLogin(CMsg& msg); // 登陆返回
	void resNormalChat(CMsg& msg); // 聊天返回
	void resUserLogin(CMsg& msg); // 其他用户登陆
	void resUserList(CMsg& msg); // 获取用户列表
	void resUserLogout(CMsg& msg); // 用户退出
	void resWisper(CMsg& msg); // 私聊

public:
	void AddUser(CCUser& user); // 增加用户
	void RemoveUser(int userSock); // 删除用户

public slots:
	void timeout(); // 定时器


private:
	QHBoxLayout* m_pUiMainLayout;
	UILogin* m_pUiLogin;
	UIChat* m_pUiChat;
	QTimer* m_pTimer;
};
