#include "QtGui/QWidget"

class QListWidget;
class QTextEdit;
class QPushButton;
class CCUser;
struct CPResNormalChat;
struct CPResWisper;
class UIChat : public QWidget
{
	Q_OBJECT

public:
	UIChat(QWidget* pParent = 0);

public slots:
	void AddUser(CCUser& user);
	void RemoveUser(const char* pName);
	void AddMsg(CPResNormalChat& msg);
	void AddMsg(CPResWisper& msg);
	void onSendBtn();

private:
	QListWidget* m_pList;
	QTextEdit* m_pShowText;
	QTextEdit* m_pSpeakText;
	QPushButton* m_pSendBtn;
};
