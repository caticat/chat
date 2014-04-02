#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>

#include "msg.h"
#include "protocol.h"
#include "data.h"
#include "define.h"
#include "user.h"

#pragma comment(lib,"ws2_32.lib")

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 8889;
const int MAX_BUFF_SIZE = 4096;
const int WORK_INTERVAL = 500; // 工作间隔，500毫秒

CUser client;
bool g_clientOn = true;

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
			memcpy(&msg,buff,ret);
			//client.PushMsg(msg);

			if (msg.GetTitle() == EPReq_NormalChat)
			{
				CPReqNormalChat nchat;
				INIT_CP(nchat);
				msg.GetData(nchat);
				printf("msg from server is:%s\n",nchat.chat);
			}

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
		strncpy(nchat.chat,buff,strlen(buff));
		client.Send(EPReq_NormalChat,nchat);
	}
}

void workThd()
{
	printf("workThd start.\n");

	int i = 0;
	while (g_clientOn)
	{
		Sleep(WORK_INTERVAL); // 每0.5秒遍历一次
		if (!client.IsEmpty())
		{
			CMsg msg = client.PopMsg();
			if (msg.GetTitle() == EPReq_NormalChat)
			{
				CPReqNormalChat nchat;
				INIT_CP(nchat);
				msg.GetData(nchat);
				printf("msg from server is:%s;\n",nchat.chat);
			}
		}
	}

	printf("workThd end.\n");
}

int main()
{
	printf("client startup\n");

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
	//boost::thread tWork(workThd);

	tRead.join();
	tWrite.join();
	//tWork.join();

	printf("client shutdown\n");
	return 0;
}