#pragma once

#include "constant.h"
#include <string>

class CCUser
{
public:
	CCUser() : m_sock(0) { memset(m_name,0,CONST_MAX_NAME_LEN); }

public:
	int m_sock;
	char m_name[CONST_MAX_NAME_LEN];
};
