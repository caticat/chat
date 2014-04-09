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
	int sock;
	char chat[CONST_MAX_CHAT_LEN];
};

struct CPResUserLogin
{
	int sock;
	char name[CONST_MAX_NAME_LEN];
};

struct CPResUserList
{
	int userSize;
	CPResUserLogin user[CONST_MAX_USER_NUM];
};

struct CPResUserLogout
{
	int sock;
};

struct CPReqWisper
{
	int sock;
	char chat[CONST_MAX_CHAT_LEN];
};

struct CPResWisper
{
	int sock;
	char chat[CONST_MAX_CHAT_LEN];
};

