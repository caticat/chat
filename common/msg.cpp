#include <string>

#include "msg.h"

CMsg::CMsg() : m_length(0)
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
