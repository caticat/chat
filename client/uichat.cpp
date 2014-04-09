#include "uichat.h"
#include "cuser.h"
#include "user.h"
#include "data.h"
#include "protocol.h"
#include "QtGui/QGridLayout"
#include "QtGui/QListWidget"
#include "QtGui/QTextEdit"
#include "QtGui/QPushButton"

extern CUser client;

extern void GetNameBySock(int sock, char* name);
extern int GetSockByName(char* name);

QString TO_ALL_NAME = "To All";

UIChat::UIChat(QWidget* pParent) : QWidget(pParent)
{
	m_pList = new QListWidget;
	m_pShowText = new QTextEdit;
	m_pSpeakText = new QTextEdit;
	m_pSendBtn = new QPushButton(tr("Send"));
	connect(m_pSendBtn,SIGNAL(clicked()),this,SLOT(onSendBtn()));

	QGridLayout* pLayout = new QGridLayout;
	pLayout->addWidget(m_pShowText,0,0);
	pLayout->addWidget(m_pList,0,1);
	pLayout->addWidget(m_pSpeakText,1,0);
	pLayout->addWidget(m_pSendBtn,1,1);
	pLayout->setColumnStretch(0,1);

	CCUser user;
	user.m_sock = 0;
	strcpy_s(user.m_name,CONST_MAX_NAME_LEN,TO_ALL_NAME.toLocal8Bit());
	AddUser(user);
	m_pList->setItemSelected(m_pList->item(0),true);

	setLayout(pLayout);
}

void UIChat::AddUser(CCUser& user)
{
	printf("adduser:%d,name:%s\n",user.m_sock,user.m_name);
	m_pList->addItem(tr(user.m_name));
}

void UIChat::RemoveUser(const char* pName)
{
	QList<QListWidgetItem*> itemList = m_pList->findItems(pName,Qt::MatchExactly);
	for (QList<QListWidgetItem*>::iterator it = itemList.begin(); it != itemList.end(); ++it)
		m_pList->takeItem(m_pList->row(*it));
	printf("remove user:%s complete.",pName);
}

void UIChat::AddMsg(CPResNormalChat& msg)
{
	printf("chat msg: %s\n",msg.chat);
	char name[CONST_MAX_NAME_LEN];
	GetNameBySock(msg.sock,name);
	char allMsg[CONST_MAX_NAME_LEN+CONST_MAX_CHAT_LEN+3] = {0};
	sprintf_s(allMsg,sizeof(allMsg),"%s : %s",name,msg.chat);
	m_pShowText->append(tr(allMsg));
}

void UIChat::AddMsg(CPResWisper& msg)
{
	char name[CONST_MAX_NAME_LEN];
	GetNameBySock(msg.sock,name);
	char allMsg[CONST_MAX_NAME_LEN+CONST_MAX_CHAT_LEN+3] = {0};
	sprintf_s(allMsg,sizeof(allMsg),"%s wispered: %s",name,msg.chat);
	m_pShowText->append(tr(allMsg));
}

void UIChat::onSendBtn()
{
	QString text = m_pSpeakText->toPlainText();
	if (text.isEmpty())
		return;

	QList<QListWidgetItem*> list = m_pList->selectedItems();
	if (list.isEmpty())
		return;
	QString talkToQStr = list.first()->text();
	if (talkToQStr == TO_ALL_NAME)
	{
		CPReqNormalChat chat;
		INIT_CP(chat);
		strcpy_s(chat.chat,CONST_MAX_CHAT_LEN,text.toLocal8Bit());
		client.Send(EPReq_NormalChat,chat);
		m_pSpeakText->clear();
	}
	else
	{
		printf("send msg to %s\n",talkToQStr.toLocal8Bit());
		char toName[CONST_MAX_NAME_LEN];
		strcpy_s(toName,CONST_MAX_NAME_LEN,talkToQStr.toLocal8Bit());
		int toSock = GetSockByName(toName);
		if (toSock == 0)
			return;
		CPReqWisper qWisper;
		INIT_CP(qWisper);
		qWisper.sock = toSock;
		strcpy_s(qWisper.chat,CONST_MAX_CHAT_LEN,text.toLocal8Bit());
		client.Send(EPReq_Wisper,qWisper);
		m_pSpeakText->clear();

		char allMsg[CONST_MAX_NAME_LEN+CONST_MAX_CHAT_LEN+3] = {0};
		sprintf_s(allMsg,sizeof(allMsg),"wisper to %s : %s",toName,qWisper.chat);
		m_pShowText->append(tr(allMsg));
	}
}
