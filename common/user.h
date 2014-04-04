#pragma once

/*
	user for net data store to use

	2014-04-02 file created
*/

#include "define.h"
#include "msg.h"
#include <queue>
#include <WinSock2.h>

class DLL_API CUser
{
public:
	CUser() { m_pMsgDeque = new msgDeque_t; }
	CUser(const CUser& user);
	~CUser() { delete m_pMsgDeque; }

	CUser& operator=(const CUser& user);
	
	int GetSize() const { return m_pMsgDeque->size(); }
	bool IsEmpty() const { return m_pMsgDeque->empty(); }

	void SetSock(int sock) { m_sock = sock; }
	int GetSock() const { return m_sock; }

	void PushMsg(CMsg& msg);
	CMsg PopMsg();

	template <typename T>
	void Send(int title,const T& data)
	{
		CMsg msg;
		msg.SetTitle(title);
		msg.SetData(data);
		send(m_sock,(char*)&msg,msg.GetLength(),0);
	}

private:
	typedef std::deque<CMsg> msgDeque_t;
	typedef std::deque<CMsg>::const_iterator cItMsgDeque_t;

private:
	int m_sock;
	msgDeque_t* m_pMsgDeque;
};
