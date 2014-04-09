#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>
#include "QtGui/QApplication"
#include "uimain.h"

#include "msg.h"
#include "protocol.h"
#include "data.h"
#include "define.h"
#include "user.h"
#include "cuser.h"
#include <list>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"QtCore4.lib")
#pragma comment(lib,"QtGui4.lib")

static char SERVER_IP[12]; // = "127.0.0.1";
static int SERVER_PORT; // = 8889;
const int MAX_BUFF_SIZE = 4096;
const int WORK_INTERVAL = 500; // 工作间隔，500毫秒

CUser client;
char g_name[CONST_MAX_NAME_LEN];
bool g_clientOn = true;
UIMain* g_pUiMain; // 主界面
std::list<CCUser> g_userList; // 角色列表
boost::mutex g_userListMutex; // 角色列表 互斥量

// 获取玩家名字
void GetNameBySock(int sock, char* name)
{
	if (name == NULL)
		return;
	boost::mutex::scoped_lock lock(g_userListMutex);
	for (std::list<CCUser>::iterator it = g_userList.begin(); it != g_userList.end(); ++it)
	{
		if (it->m_sock == sock)
		{
			strcpy_s(name,CONST_MAX_NAME_LEN,it->m_name);
			return;
		}
	}
}

// 获取玩家Sock
int GetSockByName(char* name)
{
	if (name == NULL)
		return 0;
	boost::mutex::scoped_lock lock(g_userListMutex);
	for (std::list<CCUser>::iterator it = g_userList.begin(); it != g_userList.end(); ++it)
	{
		if (strcmp(it->m_name,name) == 0)
			return it->m_sock;
	}
	return 0;
}

void resLogin(CMsg& msg)
{
	//g_pUiMain->resLogin();
}

void resNormalChat(CMsg& msg)
{
	CPReqNormalChat nchat;
	INIT_CP(nchat);
	msg.GetData(nchat);
	printf("msg from server is:%s\n",nchat.chat);
}

void userLogin(CMsg& msg)
{
	CPResUserLogin userLogin;
	INIT_CP(userLogin);
	msg.GetData(userLogin);
	printf("othersock:%d,name:%s\n",userLogin.sock,userLogin.name);
	CCUser user;
	user.m_sock = userLogin.sock;
	strcpy_s(user.m_name,CONST_MAX_NAME_LEN,userLogin.name);
	{
		boost::mutex::scoped_lock lock(g_userListMutex);
		g_userList.push_back(user);
	}
	g_pUiMain->AddUser(user);
}

void readThd()
{
	char buff[MAX_BUFF_SIZE];
	int ret = 0;
	CMsg msg;
	
	while (g_clientOn)
	{
		memset(buff,0,MAX_BUFF_SIZE);
		ret = recv(client.GetSock(),buff,MAX_BUFF_SIZE,0);
		if (ret == 0 || ((ret == SOCKET_ERROR) && (WSAGetLastError() == WSAECONNRESET)))
		{
			printf("socket close.\n");
			g_clientOn = false;
			closesocket(client.GetSock());
		}
		else if (ret < 0)
		{
			printf("socket close by client self.\n");
			break;
		}
		else
		{
			//std::cout << WSAGetLastError() << std::endl;
			if (ret == MAX_BUFF_SIZE)
				--ret;
			//buff[ret] = '\0';
			printf("msglen:%d\n",ret);
			memcpy(&msg,buff,ret);
			client.PushMsg(msg);

			//CTITLE_HUB(EPRes_Login,resLogin,msg);
			//CTITLE_HUB(EPReq_NormalChat,resNormalChat,msg);
			//CTITLE_HUB(EPRes_UserLogin,userLogin,msg);
			//printf("msg from server:%s\n",buff);
		}
	}
	
}

void writeThd()
{
	char buff[MAX_BUFF_SIZE];
	while (g_clientOn)
	{
		memset(buff,0,MAX_BUFF_SIZE);
		fgets(buff,MAX_BUFF_SIZE,stdin);
		if (strlen(buff) > 0)
			buff[strlen(buff)-1] = '\0';
		if (strcmp(buff,"exit") == 0)
		{
			g_clientOn = false;
			closesocket(client.GetSock());
			break;
		}
		//send(clientFd,buff,MAX_BUFF_SIZE,0);
		CPReqNormalChat nchat;
		INIT_CP(nchat);
		strcpy_s(nchat.chat,strlen(buff),buff);
		client.Send(EPReq_NormalChat,nchat);
	}
}

void uiThd(int argc, char** argv)
{
	QApplication app(argc,argv);

	g_pUiMain = new UIMain;
	g_pUiMain->show(); // 逻辑不能放在这里面，需要单独写一个函数

	app.exec();
	g_clientOn = false;
}

int main(int argc, char** argv)
{
	printf("client startup\n");

	// 加载配置文件
	std::ifstream inFile("config");
	if (inFile.fail())
	{
		printf("open config failed.\n");
		return 0;
	}
	char config[128];
	std::string tmpStr;
	while (inFile.getline(config,sizeof(config)))
	{
		if (strstr(config,"port") != NULL)
		{
			tmpStr = config;
			tmpStr = tmpStr.substr(5);
			SERVER_PORT = atoi(tmpStr.c_str());
			//std::cout << "line:" << SERVER_PORT << std::endl;
		}
		else if (strstr(config,"host") != NULL)
		{
			tmpStr = config;
			tmpStr = tmpStr.substr(5);
			strcpy_s(SERVER_IP,sizeof(SERVER_IP),tmpStr.c_str());
			//std::cout << "line:" << SERVER_PORT << std::endl;
		}
	}

	WSADATA wsaData;
	WSAStartup(0x0202,&wsaData);

	client.SetSock(socket(AF_INET,SOCK_STREAM,IPPROTO_TCP));
	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(client.GetSock(),(sockaddr*)&serverAddr,sizeof(sockaddr_in)) != NOERROR)
	{
		printf("connect error:%d",WSAGetLastError());
		return 1;
	}

	boost::thread tRead(readThd);
	boost::thread tWrite(writeThd);
	boost::thread tUi(boost::bind(&uiThd,argc,argv));


	// join
	tUi.join();

	if (g_clientOn)
		client.Send(EPReq_Logout,NULL);

	printf("client shutdown\n");
	return 0;
}