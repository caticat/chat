#pragma once

/*
	msg for net data transfer

	2014-03-28 file created. base utility added.
	2014-03-31 change setdata to template. use title to adjust T type.
*/

#include "define.h"

class CData;
class DLL_API CMsg
{
public:
	CMsg();

	int GetTitle();
	void SetTitle(int title);

	template <typename T>
	void GetData(T& data)
	{
		memcpy(&data,m_buff+MSG_TITLE,sizeof(data));
	}

	template <typename T>
	void SetData(T& data)
	{
		memcpy(m_buff+MSG_TITLE,&data,sizeof(data));
	}

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
