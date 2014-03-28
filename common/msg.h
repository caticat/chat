#pragma once

/*
	msg for net data transfer

	2014-03-28 file created. base utility added.
*/

#include "define.h"

class CData;
class DLL_API CMsg
{
public:
	CMsg();

	int GetTitle();
	void SetTitle(int title);

	void GetData(CData* pData, int dataLen);
	void SetData(CData* pData, int dataLen);

private:
	enum MSG_CONST {
		MSG_TITLE = sizeof(int),
		MSG_CONTENT = 1024,
		MSG_MAX_BUFF_SIZE = MSG_TITLE+MSG_CONTENT,
	};

public:
	int m_pos;
	char m_buff[MSG_MAX_BUFF_SIZE];
};
