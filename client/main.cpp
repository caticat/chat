#include <WinSock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 8888;
const int MAX_BUFF_SIZE = 4096;

int connectFd = 0;

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	connectFd = socket(AF_INET,SOCK_STREAM,0);
	printf("%d!\n",connectFd);
	
	sockaddr_in serverAddr;
	memset(&serverAddr,0,sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int errNo = connect(connectFd,(sockaddr*)&serverAddr,sizeof(sockaddr));
	if (errNo < 0)
	{
		printf("connect failed!\n");
		return 0;
	}

	char* buf = "aaaaaaa";
	send(connectFd,buf,strlen(buf),0);

	int msgLen = 0;
	char msg[MAX_BUFF_SIZE];
	while (1)
	{
		msgLen = recv(connectFd,msg,MAX_BUFF_SIZE,0);
		if (msgLen > 0)
		{
			msg[msgLen] = '\0';
			printf("%s\n",msg);
			Sleep(1000);
		}
	}
	
	return 0;
}