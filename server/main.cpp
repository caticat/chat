#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>

#include "msg.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"common.lib")


const int SERVER_PORT = 8889;
const int MAX_BUFF_SIZE = 4096;
const int MAX_USER = FD_SETSIZE;
const int MAX_BACKLOG_SIZE = 3; // 等待连接队列的最大长度

int g_userNum = 0;
SOCKET g_users[MAX_USER];
bool g_serveiceOn = true; // 程序服务是否开启


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
		g_users[g_userNum++] = clientFD;
	}

	printf("acceptThd end.\n");
}

// 工作线程
void workThd()
{
	printf("workThd start.\n");

	fd_set fdRead;
	fd_set fdWrite;
	int i, ret;
	timeval tv = {1,0};
	char buff[MAX_BUFF_SIZE];
	char toBuff[MAX_BUFF_SIZE];
	SOCKET toSock = 0;

	while (g_serveiceOn)
	{
		if (g_userNum == 0)
		{
			printf("service wait.\n");
			Sleep(5000);
			continue;
		}

		printf("read & write sock data\n");
		
		FD_ZERO(&fdRead);
		for (i = 0; i < g_userNum; ++i)
			FD_SET(g_users[i],&fdRead);
		memset(toBuff,0,MAX_BUFF_SIZE);
		toSock = 0;

		//ret = select(0,&fdRead,&fdWrite,NULL,&tv); // 同时读写会导致一直读取，写入数据，返回的值判定不清楚
		ret = select(0,&fdRead,NULL,NULL,&tv);
		if (ret != 0)
		{
			// 接受数据
			for (i = 0; i < g_userNum; ++i)
			{
				if (FD_ISSET(g_users[i],&fdRead))
				{
					ret = recv(g_users[i],buff,MAX_BUFF_SIZE,0);
					if ((ret == 0) || ((ret == SOCKET_ERROR) && (WSAGetLastError() == WSAECONNRESET)))
					{
						printf("client sock:%d closed\n",g_users[i]);
						closesocket(g_users[i]);
						if (i < g_userNum-1)
							g_users[i--] = g_users[--g_userNum];
						else
							--g_userNum;
					}
					else
					{
						if (ret == MAX_BUFF_SIZE)
							--ret;
						buff[ret] = '\0';
						strncpy(toBuff,buff,MAX_BUFF_SIZE);
						toSock = g_users[i];
						printf("msg from sock:%d is:%s\n",g_users[i],buff);
					}
				}
			}
		}

		// 发送数据
		if (strlen(toBuff) != 0) // 有信息则发送给客户端
		{
			FD_ZERO(&fdWrite);
			for (i = 0; i < g_userNum; ++i)
				FD_SET(g_users[i],&fdWrite);
			ret = select(0,&fdRead,&fdWrite,NULL,&tv);
			if (ret != 0)
			{
				for (i = 0; i < g_userNum; ++i)
				{
					if (FD_ISSET(g_users[i],&fdWrite) && (g_users[i] != toSock))
						send(g_users[i],toBuff,strlen(toBuff),0);
				}
			}
		}
	}

	printf("workThd end.\n");
}

int main()
{
	printf("server start.\n");

	boost::thread tAccept(acceptThd);
	boost::thread tWork(workThd);

	tAccept.join();
	tWork.join();

	printf("server end.\n");
	/*
	printf("server start\n");

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	int listenFD,connectFD;
	listenFD = socket(AF_INET,SOCK_STREAM,0);

	sockaddr_in serverAddr,clientAddr;
	int addrLen = sizeof(sockaddr_in);
	memset(&serverAddr,0,sizeof(sockaddr_in));
	memset(&clientAddr,0,sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(ADDR_ANY);

	bind(listenFD,(sockaddr*)&serverAddr,sizeof(sockaddr));

	if (listen(listenFD,MAX_USER) == -1)
	{
		printf("listen failed!\n");
		return 0;
	}

	int userIdx = 0;
	memset(users,0,sizeof(users));
	char buf[MAX_BUFF_SIZE];
	int msgLen = 0;
	while(userIdx < MAX_USER)
	{
		if ((connectFD = accept(listenFD,(sockaddr*)&clientAddr,&addrLen)) == -1)
		{
			printf("wait connect\n");
			continue;
		}
		users[userIdx++] = connectFD;
		printf("receive msg\n");
		msgLen = recv(connectFD,buf,MAX_BUFF_SIZE,0);
		users[msgLen] = '\0';
		for (int i = 0; i < userIdx; ++i)
		{
			send(users[i],buf,msgLen,0);
		}
		printf("send msg complete\n");
	}

	printf("server end\n");
	*/
	return 0;
}
