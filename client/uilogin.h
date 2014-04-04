#include "QtGui/QWidget"

class QLineEdit;
class UILogin : public QWidget
{
	Q_OBJECT
public:
	UILogin(QWidget* pParent = 0);

signals:
	void onLoginClicked(QString name);

private slots:
	void onLogin();

private:
	QLineEdit* m_pLedit;
	
};
