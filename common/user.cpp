#include "user.h"

CUser::CUser(const CUser& user)
{
	m_sock = user.m_sock;
	m_pMsgDeque = new msgDeque_t;
	for (cItMsgDeque_t cit = user.m_pMsgDeque->begin();cit != user.m_pMsgDeque->end(); ++cit)
		m_pMsgDeque->push_back(*cit);
}

CUser& CUser::operator=(const CUser& user)
{
	if (this == &user)
		return *this;
	m_sock = user.m_sock;
	m_pMsgDeque->clear();
	for (cItMsgDeque_t cit = user.m_pMsgDeque->begin();cit != user.m_pMsgDeque->end(); ++cit)
		m_pMsgDeque->push_back(*cit);
	return *this;
}

void CUser::PushMsg(CMsg& msg)
{
	m_pMsgDeque->push_back(msg);
}

CMsg CUser::PopMsg()
{
	CMsg msg;
	if (m_pMsgDeque->empty())
		return msg;
	msg = m_pMsgDeque->front();
	m_pMsgDeque->pop_front();
	return msg;
}
