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
	void resLogin(CMsg& msg); // ��½����
	void resNormalChat(CMsg& msg); // ���췵��
	void resUserLogin(CMsg& msg); // �����û���½
	void resUserList(CMsg& msg); // ��ȡ�û��б�
	void resUserLogout(CMsg& msg); // �û��˳�
	void resWisper(CMsg& msg); // ˽��

public:
	void AddUser(CCUser& user); // �����û�
	void RemoveUser(int userSock); // ɾ���û�

public slots:
	void timeout(); // ��ʱ��


private:
	QHBoxLayout* m_pUiMainLayout;
	UILogin* m_pUiLogin;
	UIChat* m_pUiChat;
	QTimer* m_pTimer;
};
