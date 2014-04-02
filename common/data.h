#pragma once

/*
	struct for transformation

	2014-03-31 file created. base utility added.
*/

#include "constant.h"

struct CPReqLogin
{
	char name[CONST_MAX_NAME_LEN];
};

struct CPReqNormalChat
{
	char chat[CONST_MAX_CHAT_LEN];
};

struct CPResNormalChat
{
	char name[CONST_MAX_NAME_LEN];
	char chat[CONST_MAX_CHAT_LEN];
};

struct CPResUserList
{
	int userSize;
	char name[CONST_MAX_USER_NUM][CONST_MAX_NAME_LEN];
};

struct CPResUserLogin
{
	int sock;
	char name[CONST_MAX_NAME_LEN];
};

struct CPResUserLogout
{
	int sock;
};

