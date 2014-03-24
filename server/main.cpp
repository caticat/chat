#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>

#pragma comment(lib,"ws2_32.lib")

const int SERVER_PORT = 8888;
const int MAX_BUFF_SIZE = 4096;
const int MAX_USER = 10;

int users[MAX_USER];

int main()
{
	boost::thread tAccept(acceptThd);
	boost::thread tWork(workThd);

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
