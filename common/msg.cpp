#include <string>

#include "msg.h"

CMsg::CMsg() : m_pos(0)
{
	memset(m_buff,0,MSG_MAX_BUFF_SIZE);
}

int CMsg::GetTitle()
{
	int title = 0;
	memcpy(&title,m_buff,sizeof(int));
	return title;
}

void CMsg::SetTitle(int title)
{
	memcpy(m_buff,&title,sizeof(int));
}

template <typename T>
void CMsg::GetData(T& data)
{
	memcpy(data,m_buff+MSG_TITLE,sizeof(data));
}

template <typename T>
void CMsg::SetData(T& data)
{
	memcpy(m_buff+MSG_TITLE,data,sizeof(data));
}
