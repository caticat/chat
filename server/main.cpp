#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>

#include "msg.h"
#include "suser.h"
#include "protocol.h"
#include "data.h"
#include "define.h"
#include <queue>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"common.lib")


const int SERVER_PORT = 8889;
const int MAX_BUFF_SIZE = 4096;
const int MAX_USER = FD_SETSIZE;
const int MAX_BACKLOG_SIZE = 3; // 等待连接队列的最大长度
const int WORK_INTERVAL = 200; // 工作间隔，200毫秒

int g_userNum = 0;
CSUser g_users[MAX_USER];
bool g_serveiceOn = true; // 程序服务是否开启

std::deque<CMsg> g_msg; // 对所有成员的广播消息队列


int CALLBACK acceptCondition(LPWSABUF lpCallerId,LPWSABUF lpCallerData, LPQOS lpSQOS,LPQOS lpGQOS,LPWSABUF lpCalleeId, LPWSABUF lpCalleeData,GROUP FAR * g,DWORD dwCallbackData)
{
	if (g_userNum < MAX_USER)
		return CF_ACCEPT;
	return CF_REJECT;
}

// accept线程
void acceptThd()
{
	printf("acceptThd start.\n");

	WSADATA wsaData;
	if (WSAStartup(0x0202,&wsaData) != NOERROR)
	{
		printf("WSAStartup error.\n");
		abort();
	}
	printf("WSAStartup complete\n");

	SOCKET listenFD;
	listenFD = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	printf("socket init complete\n");

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(SERVER_PORT);
	int addrSize = sizeof(sockaddr_in);
	printf("addr init complete\n");

	if (bind(listenFD,(sockaddr*)&localAddr,addrSize) != NOERROR)
	{
		printf("bind error:%d\n",WSAGetLastError());
		abort();
	}
	printf("bind complete\n");

	if (listen(listenFD,MAX_BACKLOG_SIZE) != NOERROR)
	{
		printf("listen error:%d\n",WSAGetLastError());
		abort();
	}
	printf("listen complete\n");

	SOCKET clientFD;
	sockaddr_in clientAddr;
	while (g_serveiceOn)
	{
		clientFD = WSAAccept(listenFD,(sockaddr*)&clientAddr,&addrSize,acceptCondition,0);
		printf("accept client:%s:%d:%d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),g_userNum);
		g_users[g_userNum++].SetSock(clientFD);
	}

	printf("acceptThd end.\n");
}

// 工作线程
void readThd()
{
	printf("readThd start.\n");

	fd_set fdRead;
	fd_set fdWrite;
	int i, ret;
	timeval tv = {1,0};
	char buff[MAX_BUFF_SIZE];
	CMsg msg;

	while (g_serveiceOn)
	{
		if (g_userNum == 0)
		{
			printf("service wait.\n");
			Sleep(5000);
			continue;
		}

		//printf("read & write sock data\n");
		
		FD_ZERO(&fdRead);
		for (i = 0; i < g_userNum; ++i)
			FD_SET(g_users[i].GetSock(),&fdRead);

		//ret = select(0,&fdRead,&fdWrite,NULL,&tv); // 同时读写会导致一直读取，写入数据，返回的值判定不清楚
		ret = select(0,&fdRead,NULL,NULL,&tv);
		if (ret != 0)
		{
			// 接受数据
			for (i = 0; i < g_userNum; ++i)
			{
				if (FD_ISSET(g_users[i].GetSock(),&fdRead))
				{
					ret = recv(g_users[i].GetSock(),buff,MAX_BUFF_SIZE,0);
					if ((ret == 0) || ((ret == SOCKET_ERROR) && (WSAGetLastError() == WSAECONNRESET)))
					{
						printf("client sock:%d closed\n",g_users[i].GetSock());
						closesocket(g_users[i].GetSock());
						if (i < g_userNum-1)
							g_users[i--] = g_users[--g_userNum];
						else
							--g_userNum;
					}
					else
					{
						if (ret == MAX_BUFF_SIZE)
							--ret;
						//buff[ret] = '\0';
						memcpy(&msg,buff,ret);
						g_users[i].PushMsg(msg);
						//printf("msg from sock:%d is:%s\n",g_users[i],buff);
					}
				}
			}
		}

		// 发送数据
		if (!g_msg.empty()) // 有信息则发送给客户端
		{
			msg = g_msg.front();
			g_msg.pop_front();
			FD_ZERO(&fdWrite);
			for (i = 0; i < g_userNum; ++i)
				FD_SET(g_users[i].GetSock(),&fdWrite);
			ret = select(0,NULL,&fdWrite,NULL,&tv);
			if (ret != 0)
			{
				for (i = 0; i < g_userNum; ++i)
				{
					if (FD_ISSET(g_users[i].GetSock(),&fdWrite))
						send(g_users[i].GetSock(),(char*)&msg,msg.GetLength(),0);
					printf("msglen:%d\n",msg.GetLength());
				}
			}
		}
	}

	printf("readThd end.\n");
}

void reqLogin(CSUser& user, CMsg& msg)
{
	CPReqLogin login;
	INIT_CP(login);
	msg.GetData(login);
	strncpy(user.m_name,login.name,CONST_MAX_NAME_LEN);
	user.Send(EPRes_Login,NULL);

	CPResUserLogin userLogin;
	INIT_CP(userLogin);
	userLogin.sock = user.GetSock();
	strncpy(userLogin.name,login.name,CONST_MAX_NAME_LEN);
	CMsg loginMsg;
	loginMsg.SetTitle(EPRes_UserLogin);
	loginMsg.SetData(userLogin);
	g_msg.push_back(loginMsg);
}

void reqNormalChat(CSUser& user, CMsg& msg)
{
	CPReqNormalChat nchat;
	INIT_CP(nchat);
	msg.GetData(nchat);
	g_msg.push_back(msg);
	printf("msg from %d is:%s\n",user.GetSock(),nchat.chat);
}

void wordThd()
{
	printf("workThd start.\n");

	int i = 0;
	while (g_serveiceOn)
	{
		Sleep(WORK_INTERVAL);
		for (i = 0; i < g_userNum; ++i)
		{
			while (!g_users[i].IsEmpty())
			{
				CMsg msg = g_users[i].PopMsg();
				STITLE_HUB(EPReq_Login,reqLogin,g_users[i],msg);
				STITLE_HUB(EPReq_NormalChat,reqNormalChat,g_users[i],msg);
			}
		}
	}

	printf("workThd end.\n");
};

int main()
{
	printf("server start.\n");

	boost::thread tAccept(acceptThd);
	boost::thread tRead(readThd);
	boost::thread tWork(wordThd);

	tAccept.join();
	tRead.join();
	tWork.join();

	printf("server end.\n");
	return 0;
}
