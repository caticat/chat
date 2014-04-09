#include "uilogin.h"

#include "QtGui/QFont"
#include "QtGui/QPushButton"
#include "QtGui/QLineEdit"
#include "QtGui/QHBoxLayout"

#include "constant.h"
#include "user.h"
#include "data.h"
#include "protocol.h"

extern char g_name[CONST_MAX_NAME_LEN];
extern CUser client;

UILogin::UILogin(QWidget* pParent) : QWidget(pParent)
{
	m_pLedit = new QLineEdit;

	QPushButton* pBtnLogin = new QPushButton(tr("Login"));
	pBtnLogin->setFont(QFont("Times",18,QFont::Bold));
	connect(pBtnLogin,SIGNAL(clicked()),this,SLOT(onLogin()));

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(m_pLedit);
	layout->addWidget(pBtnLogin);
	setLayout(layout);

	setFocusProxy(m_pLedit);
}

void UILogin::onLogin()
{
	QString qstrName = m_pLedit->text();
	strcpy_s(g_name,CONST_MAX_NAME_LEN,qstrName.toLocal8Bit());
	CPReqLogin rlogin;
	INIT_CP(rlogin);
	strcpy_s(rlogin.name,CONST_MAX_NAME_LEN,g_name);
	client.Send(EPReq_Login,rlogin);
	//emit onLoginClicked(qstrName);
}
