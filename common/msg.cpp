#include <string>

#include "msg.h"
#include "data.h"

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

void CMsg::GetData(CData* pData, int dataLen)
{
	if (pData == NULL)
		return;
	memcpy(pData,m_buff+MSG_TITLE,dataLen);
}

void CMsg::SetData(CData* pData, int dataLen)
{
	if (pData == NULL)
		return;
	memcpy(m_buff+MSG_TITLE,pData,dataLen);
}
