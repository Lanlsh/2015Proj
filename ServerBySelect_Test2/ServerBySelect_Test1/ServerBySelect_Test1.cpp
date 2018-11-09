// ServerBySelect_Test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <mutex>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

//FD_SETSIZE是在winsocket2.h头文件里定义的，这里windows默认最大为64
//在包含winsocket2.h头文件前使用宏定义可以修改这个值

const int nMessageWordMaxSize = 1023;	//发送消息的文字信息最大的字节数
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

#define PORT 8888
#define SRV_IP "192.168.15.112"

int g_nSockConn = 0;//请求连接的数目

struct ClientInfo
{
	SOCKET sockClient;
	SOCKADDR_IN addrClient;
};

ClientInfo g_Client[FD_SETSIZE];

//发送消息给客户端的消息队列
std::queue<string> m_qeSendMessage;
std::mutex mutex_sendMsg;

void ReceiveInput(VOID)
{
	char input[nMessageWordMaxSize] = { 0 };
	while (true)
	{
		cin.getline(input, nMessageWordMaxSize);	//字符串的“空格”也会获取！！
		if (input[0] != '\0')
		{
			//mutex_sendMsg.lock();
			m_qeSendMessage.push(input);
			//mutex_sendMsg.unlock();
			memset(input, 0, nMessageWordMaxSize);
		}

		//Sleep(1000);
	}
}

//发心跳报文
void SendHeartMsg(char* recvData, int i, fd_set& fdTemp, fd_set& fdRead)
{
	TCPMessage* tcpMessage = (TCPMessage*)recvData;
	memset(tcpMessage->chMessage, 0, nMessageWordMaxSize);
	tcpMessage->bIsHeartBeat = true;
	strcpy_s(tcpMessage->chMessage, 7, "Hello!");

	int nSendLength = 0;
	while (nSendLength != nSize)
	{
		int curLength = 0;
		curLength = send(g_Client[i].sockClient, recvData, nSize, 0);
		if (curLength <= 0 || (curLength == SOCKET_ERROR))
		{
			cout << "Client " << inet_ntoa(g_Client[i].addrClient.sin_addr) << "closed" << endl;
			//如果关闭相关的socket，则必须clear掉相关socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//将失效的sockClient剔除，用数组的最后一个补上去
				g_Client[i--].sockClient = g_Client[--g_nSockConn].sockClient;
			}
			else
			{
				g_Client[i].sockClient = 0;
				g_nSockConn--;
			}

			break;
		}

		nSendLength += curLength;
	}
}

//发送自己的消息给客户端
void SendMsgToClient(char* recvData, int i, fd_set& fdTemp, fd_set& fdRead, string strMsg)
{
	//发自己的消息
	TCPMessage* tcpMessage = (TCPMessage*)recvData;
	memset(tcpMessage->chMessage, 0, nMessageWordMaxSize);
	tcpMessage->bIsHeartBeat = false;

	//因为发送的文字信息为定长，所以，str.length()不能超过1024字节！！！
	if (strMsg.length() >= nMessageWordMaxSize)
	{
		memcpy(tcpMessage->chMessage, strMsg.c_str(), nMessageWordMaxSize);
	}
	else
		memcpy(tcpMessage->chMessage, strMsg.c_str(), strMsg.length());

	int nSendLength = 0;
	while (nSendLength != nSize)
	{
		int curLength = 0;
		curLength = send(g_Client[i].sockClient, recvData, nSize, 0);
		if (curLength <= 0 || (curLength == SOCKET_ERROR))
		{
			cout << "Client " << inet_ntoa(g_Client[i].addrClient.sin_addr) << "closed" << endl;
			//如果关闭相关的socket，则必须clear掉相关socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//将失效的sockClient剔除，用数组的最后一个补上去
				g_Client[i--].sockClient = g_Client[--g_nSockConn].sockClient;
			}
			else
			{
				g_Client[i].sockClient = 0;
				g_nSockConn--;
			}

			break;
		}

		nSendLength += curLength;
	}

	if (nSendLength == sizeof(TCPMessage))
	{
		cout << "Server: sendto " << inet_ntoa(g_Client[i].addrClient.sin_addr) << ": " << tcpMessage->chMessage << endl;
	}

}


bool RecvClientMsg(char* recvData, int i, fd_set& fdTemp, fd_set& fdRead)
{
	int nRecvLength = 0;
	while (nRecvLength != nSize)
	{
		int curLength = 0;
		curLength = recv(g_Client[i].sockClient, recvData, nSize, 0);
		if (curLength <= 0 || (curLength == SOCKET_ERROR))
		{
			cout << "Client " << inet_ntoa(g_Client[i].addrClient.sin_addr) << "closed" << endl;
			//如果关闭相关的socket，则必须clear掉相关socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//将失效的sockClient剔除，用数组的最后一个补上去
				g_Client[i--].sockClient = g_Client[--g_nSockConn].sockClient;
			}
			else
			{
				g_Client[i].sockClient = 0;
				g_nSockConn--;
			}

			return false;
		}

		nRecvLength += curLength;
	}

	if (nRecvLength == nSize)
	{
		return true;
	}

	return false;
}

int main()
{
	//基本步骤就不解释了，网络编程基础那篇博客里讲的很详细了
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/*
	SOCKET PASCAL FAR socket( int af, int type, int protocol);
	af：一个地址描述。目前仅支持AF_INET格式，也就是说ARPA Internet地址格式。
	type：新套接口的类型描述。
	protocol：套接口所用的协议。如调用者不想指定，可用0指定，表示缺省。
	*/

	//创建一个套接字，错误返回-1
	SOCKET sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sockListen == SOCKET_ERROR)
	{
		cout << "sockListen == -1" << endl;
		return 0;
	}
	else
	{
		cout << "服务器自己的监听socket： " << sockListen << endl;
	}

	//设置服务器的端口重启后能立马正常使用而不被占用
	bool option = true;
	int optlen = sizeof(option);
	setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, (char*)&option, optlen);


	//绑定IP和端口号的准备工作
	struct sockaddr_in my_addr;	//sin_port和 sin_addr 必须是网络字节顺序(Network Byte Order)！
	my_addr.sin_family = AF_INET; //* host byte order */ 本机字节排序   
	//这里也有要注意的几件事情。my_addr.sin_port 是网络字节顺序， my_addr.sin_addr.s_addr 也是的
	my_addr.sin_port = htons(PORT); /* short, network byte order */  //将“本机字节排序”转换成“网络字节排序的short类型（2字节）”
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY表示本机IP地址（此处先用本机进行，如果使用指定的IP，则为my_addr.sin_addr.s_addr = inet_addr(“指定IP”);）
	//my_addr.sin_addr.s_addr = inet_addr(SRV_IP); 
	memset(&(my_addr.sin_zero), 0, sizeof(8)); /* zero the rest of the struct */

	//将此套接字绑定IP和端口号
	//在你调用 bind() 的时候，你要小心的另一件事情是：不要采用小于 1024的端口号。所有小于1024的端口号都被系统保留！你可以选择从1024 到65535的端口(如果它们没有被别的程序使用的话)。 
	//你要注意的另外一件小事是：有时候你根本不需要调用它。如果你使 用 connect() 来和远程机器进行通讯，你不需要关心你的本地端口号(就象 你在使用 telnet 的时候)，你只要简单的调用 connect() 就可以了，它会检 查套接字是否绑定端口，如果没有，它会自己绑定一个没有使用的本地端 口。
	int nBind = ::bind(sockListen, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//一定要加“::”，否则bind为其它库的bind,返回值就不是int类型了！！！
	if (-1 == nBind)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind不成功！！！" << endl;
		return 0;
	}

	//int listen(int sockfd, int backlog);
	//sockfd 是调用 socket() 返回的套接字文件描述符。backlog 是在进入 队列中允许的连接数目。什么意思呢? 进入的连接是在队列中一直等待直 到你接受 (accept() 请看下面的文章)连接。它们的数目限制于队列的允许。 大多数系统的允许数目是20，你也可以设置为5到10。
	//和别的函数一样，在发生错误的时候返回 - 1，并设置全局错误变量 errno。
	int nListenOk = listen(sockListen, 64);	//64也就是“多少等待连接控制”的意思
	if (nListenOk == -1)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "nListenOk == -1" << endl;
		//assert(false);
		return 0;
	}

	SOCKET sockClient;
	SOCKADDR_IN addrClient;
	int nLenAddrClient = sizeof(SOCKADDR);

	fd_set fdRead;
	fd_set fdTemp;
	FD_ZERO(&fdRead);
	FD_SET(sockListen, &fdRead);

	int nRet = 0;
	char* recvData = (char*)malloc(nSize);

	//创建一个线程，接收玩家输入信息
	std::thread thrdInput(&ReceiveInput);
	thrdInput.detach();

	while (true)
	{
		TIMEVAL tv;//设置超时等待时间
		//每次调用完select后，必须tv会被重置，所以，必须重新赋值！！
		tv.tv_sec = 0;
		tv.tv_usec = 200;
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

		if (FD_ISSET(sockListen, &fdTemp))
		{
			//接收连接
			sockClient = accept(sockListen, (SOCKADDR*)&addrClient, &nLenAddrClient);

			if (sockClient != INVALID_SOCKET)
			{
				g_Client[g_nSockConn].addrClient = addrClient;//保存连接端地址信息
				g_Client[g_nSockConn].sockClient = sockClient;//加入连接者队列
				FD_SET(g_Client[g_nSockConn].sockClient, &fdRead);
				g_nSockConn++;
				//输出连接者的地址信息
				cout << inet_ntoa(addrClient.sin_addr) << ":" << ntohs(addrClient.sin_port) <<":" << sockClient << "连接成功 !" << endl;
			}
		}
		//else
		//{
		for (int i = 0; i < g_nSockConn; i++)
		{
			if (FD_ISSET(g_Client[i].sockClient, &fdTemp))
			{
				memset(recvData, 0, nSize);
				//接收来自客户端的数据
				bool bRecvOk = false;
				bRecvOk = RecvClientMsg(recvData, i, fdTemp, fdRead);

				if (bRecvOk)
				{
					//显示接收自客户端的消息
					TCPMessage* recvMsg = (TCPMessage*)recvData;
					if (recvMsg->bIsHeartBeat == false)
					{
						cout << "Client " << inet_ntoa(g_Client[i].addrClient.sin_addr) << ": " << recvMsg->chMessage << endl;
					}

					//发送信息给客户端
					int nSendLength = 0;
					if (m_qeSendMessage.empty())
					{
						//发心跳报文
						//SendHeartMsg(recvData, i, fdTemp, fdRead);
					}
					else
					{
						//SendMsgToClient(recvData, i, fdTemp, fdRead);
					}
				}

			}

		}

		//}

		if (!m_qeSendMessage.empty())
		{
			//给所有已连接IP发自己输入的内容
			//mutex_sendMsg.lock();
			string strMsg = m_qeSendMessage.front();
			//mutex_sendMsg.unlock();
			for (int i=0; i<g_nSockConn; i++)
			{
				SendMsgToClient(recvData, i, fdTemp, fdRead, strMsg);
			}

			//mutex_sendMsg.lock();
			m_qeSendMessage.pop();
			//mutex_sendMsg.unlock();
		}

		//为了让自己服务器端输入可以加到queue里
		//Sleep(100);

	}

	closesocket(sockListen);
	WSACleanup();

	return 0;
}

