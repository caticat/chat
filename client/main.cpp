#include <WinSock2.h>
#include <iostream>
#include <boost/thread.hpp>

#pragma comment(lib,"ws2_32.lib")

const char SERVER_IP[] = "127.0.0.1";
const int SERVER_PORT = 8889;
const int MAX_BUFF_SIZE = 4096;

SOCKET clientFd = 0;
bool g_clientOn = true;

void readThd()
{
	char buff[MAX_BUFF_SIZE];
	int ret = 0;
	
	while (g_clientOn)
	{
		memset(buff,0,MAX_BUFF_SIZE);
		ret = recv(clientFd,buff,MAX_BUFF_SIZE,0);
		if (ret == 0 || ((ret == SOCKET_ERROR) && (WSAGetLastError() == WSAECONNRESET)))
		{
			printf("socket close.\n");
			g_clientOn = false;
			closesocket(clientFd);
		}
		else if (ret < 0)
		{
			printf("socket close by client self.\n");
			break;
		}
		else
		{
			std::cout << WSAGetLastError() << std::endl;
			if (ret == MAX_BUFF_SIZE)
				--ret;
			buff[ret] = '\0';
			printf("msg from server:%s\n",buff);
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
			closesocket(clientFd);
			break;
		}
		send(clientFd,buff,MAX_BUFF_SIZE,0);
	}
}

int main()
{
	printf("client startup\n");

	WSADATA wsaData;
	WSAStartup(0x0202,&wsaData);

	clientFd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(clientFd,(sockaddr*)&serverAddr,sizeof(sockaddr_in)) != NOERROR)
	{
		printf("connect error:%d",WSAGetLastError());
		return 1;
	}

	boost::thread tRead(readThd);
	boost::thread tWrite(writeThd);

	tRead.join();
	tWrite.join();

	printf("client shutdown\n");

	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(2,2),&wsaData);

	//connectFd = socket(AF_INET,SOCK_STREAM,0);
	//printf("%d!\n",connectFd);
	//
	//sockaddr_in serverAddr;
	//memset(&serverAddr,0,sizeof(sockaddr_in));
	//serverAddr.sin_family = AF_INET;
	//serverAddr.sin_port = htons(SERVER_PORT);
	//serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	//int errNo = connect(connectFd,(sockaddr*)&serverAddr,sizeof(sockaddr));
	//if (errNo < 0)
	//{
	//	printf("connect failed!\n");
	//	return 0;
	//}

	//char buff[MAX_BUFF_SIZE];
	//fgets(buff,MAX_BUFF_SIZE,stdin);
	//send(connectFd,buff,strlen(buff),0);
	//
	//system("pause");
	return 0;
}