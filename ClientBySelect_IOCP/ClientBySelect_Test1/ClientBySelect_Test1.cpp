// ClientBySelect_Test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cstring>
#include <thread>
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <WS2tcpip.h>
#include <queue>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int nMessageWordMaxSize = 4096;	//发送消息的文字信息最大的字节数

//TCP消息封装结构
struct TCPMessage
{
	bool bIsHeartBeat;	//是否为心跳报文
	char chMessage[nMessageWordMaxSize];	//发送的文字信息(这里为定长的大小)  //如果不要设置定长，则需要使用流模式去传递！！！	

	TCPMessage()
	{
		bIsHeartBeat = true;
		memset(chMessage, 0, nMessageWordMaxSize);
	}
};

const int nSize = sizeof(char) * sizeof(TCPMessage);

#define SERVER_PORT 8888
#define SERVER_IP "192.168.15.112"
bool bIsHaveConnect = false;

//发送消息给服务器的消息队列
std::queue<string> m_qeSendMessage;
void ReceiveInput(VOID)
{
	char input[nMessageWordMaxSize] = { 0 };
	while (true)
	{	
		cin.getline(input, nMessageWordMaxSize);	//字符串的“空格”也会获取！！
		if (input[0] != '\0')
		{
			m_qeSendMessage.push(input);
			memset(input, 0, nMessageWordMaxSize);
		}

		//Sleep(1000);
	}
}

//发送消息给服务器
void SendMsgToServer(fd_set& fdTemp, fd_set& fdRead, SOCKET& sclient)
{
	//取需要发送给server的信息
	string str = m_qeSendMessage.front();

	//对发送的报文信息进行封装
	char* cSendMessage = (char*)malloc(nMessageWordMaxSize);
	memset(cSendMessage, 0, nMessageWordMaxSize);

	//因为发送的文字信息为定长，所以，str.length()不能超过1024字节！！！
	if (str.length() >= nMessageWordMaxSize)
	{
		memcpy(cSendMessage, str.c_str(), nMessageWordMaxSize);
	}
	else
		memcpy(cSendMessage, str.c_str(), str.length());

	int sendSuccessSize = 0;
	while (sendSuccessSize != nMessageWordMaxSize)
	{
		int curSuccessSize = 0;
		curSuccessSize = send(sclient, cSendMessage, nMessageWordMaxSize, 0);
		if (curSuccessSize <= 0 || (curSuccessSize == SOCKET_ERROR))
		{
			cout << SERVER_IP << "closed" << endl;

			//如果关闭相关的socket，则必须clear掉相关socket
			FD_CLR(sclient, &fdTemp);
			FD_CLR(sclient, &fdRead);

			if (sclient != INVALID_SOCKET)
			{
				closesocket(sclient);
				sclient = INVALID_SOCKET;

				sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (sclient == INVALID_SOCKET)
				{
					cout << "创建socket失败！！！！" << endl;
					return ;
				}

				FD_SET(sclient, &fdRead);
			}

			bIsHaveConnect = false;
			break;
		}

		sendSuccessSize += curSuccessSize;
	}

	if (sendSuccessSize == nSize)
	{
		m_qeSendMessage.pop();
	}
	
	free(cSendMessage);
	cSendMessage = NULL;
}

int main()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		cout << "WSAStartup(sockVersion, &data) == 0!!!!!" << endl;
		return 0;
	}

	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		cout << "创建socket失败！！！！" << endl;
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(SERVER_PORT);
	//serAddr.sin_addr.S_un.S_addr = inet_addr(m_cIp);
	struct in_addr addr;
	inet_pton(AF_INET, SERVER_IP, (void *)&addr);
	serAddr.sin_addr.s_addr = addr.s_addr;

	fd_set fdRead;
	fd_set fdTemp;
	FD_ZERO(&fdRead);
	FD_SET(sclient, &fdRead);

	TIMEVAL tv;
	int nRet = 0;

	int nSize = sizeof(char) * (sizeof(TCPMessage));
	char* recvData = (char*)malloc(nSize);

	//创建一个线程，接收玩家输入信息
	std::thread thrdInput(&ReceiveInput);
	thrdInput.detach();
	
	while (true)
	{
		if (!bIsHaveConnect)
		{
			if (connect(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				cout << "connect失败！！！！" << GetLastError() << endl;
				bIsHaveConnect = false;
				Sleep(1000);
				continue;
			}
			else
			{
				cout << "连接" << SERVER_IP << ":" << SERVER_PORT << " " << "connect成功！！！！" << endl;
				bIsHaveConnect = true;
				continue;
			}
		}

		//每次调用完select后，必须tv会被重置，所以，必须重新赋值！！
		tv.tv_sec = 0;
		tv.tv_usec = 100;
		fdTemp = fdRead;

		//只处理read事件，不过后面还是会有读写消息发送的
		//select成功时返回0， 失败时返回-1 
		nRet = select(1, &fdTemp, NULL, NULL, &tv);	//windows下的select函数的第一个参数是没意义的，linux下为socket描述符的总数（linux下socket描述符也是文件描述符）

		if (nRet == -1)
		{
			cout << "select() error: " << GetLastError() << endl;
			break;
		}
		//else if (nRet == 0)
		//{
		//	//没有连接或者没有读事件
		//	Sleep(100);
		//	continue;
		//}

		if (FD_ISSET(sclient, &fdTemp))
		{
			if (bIsHaveConnect)
			{
				memset(recvData, 0, nSize);
				//接收来自客户端的数据
				int nRecvLength = 0;
				while (nRecvLength != nMessageWordMaxSize)
				{
					int curLength = 0;
					curLength = recv(sclient, recvData, nMessageWordMaxSize, 0);
					if (curLength <= 0 || (curLength == SOCKET_ERROR))
					{
						cout << "server " << SERVER_IP << "closed" << endl;
						//如果关闭相关的socket，则必须clear掉相关socket
						FD_CLR(sclient, &fdTemp);
						FD_CLR(sclient, &fdRead);

						//windows套接字不能被关闭多次!!!!!!
						if (sclient != INVALID_SOCKET)
						{
							closesocket(sclient);
							sclient = INVALID_SOCKET;

							sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
							if (sclient == INVALID_SOCKET)
							{
								cout << "创建socket失败！！！！" << endl;
								return 0;
							}

							FD_SET(sclient, &fdRead);
						}

						bIsHaveConnect = false;

						break;
					}

					nRecvLength += curLength;

				}

				if (nRecvLength == nMessageWordMaxSize)
				{
					cout << "server " << SERVER_IP << ": " << recvData << endl;
				}
			}

		}

		if (bIsHaveConnect)
		{
			if (!m_qeSendMessage.empty())
			{
				SendMsgToServer(fdTemp, fdRead, sclient);
			}
		}

		//Sleep(100);

	}

	if (sclient != INVALID_SOCKET)
	{
		closesocket(sclient);
		sclient = INVALID_SOCKET;
	}

	WSACleanup();

    return 0;
}

