#pragma once

#include "QtGui/QWidget"

class QHBoxLayout;
class UILogin;
class UIMain : public QWidget
{
	Q_OBJECT
public:
	UIMain(QWidget* parent = 0);

public:
	void resLogin(); // ��½����

public slots:
	//void onLoginClicked(QString qstrname);

private:
	QHBoxLayout* m_pUiMainLayout;
	UILogin* m_pUiLogin;
};
