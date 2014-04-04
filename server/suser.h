#pragma once

#include "constant.h"
#include "user.h"

class CSUser : public CUser
{
public:
	CSUser() { CUser(); memset(m_name,0,CONST_MAX_NAME_LEN); }

public:
	char m_name[CONST_MAX_NAME_LEN];
};
