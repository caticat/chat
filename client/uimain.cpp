#include "uimain.h"

#include "constant.h"
#include "uilogin.h"
#include "QtGui/QHBoxLayout"

UIMain::UIMain(QWidget* parent) : QWidget(parent)
{
	m_pUiLogin = new UILogin;

	//connect(pUiLogin,SIGNAL(onLoginClicked(QString)),this,SLOT(onLoginClicked(QString)));

	m_pUiMainLayout = new QHBoxLayout;
	m_pUiMainLayout->addWidget(m_pUiLogin);
	setLayout(m_pUiMainLayout);
}

// µÇÂ½·µ»Ø
void UIMain::resLogin()
{
	printf("login success\n");
	m_pUiLogin->hide();
	m_pUiMainLayout->removeWidget(m_pUiLogin);
	//m_pUiMainLayout->update();
}

//void UIMain::onLoginClicked(QString qstrname)
//{
//	char name[CONST_MAX_NAME_LEN] = {0};
//	strncpy(name,qstrname.toLocal8Bit(),CONST_MAX_NAME_LEN);
//	printf("name is :%s\n",name);
//}
