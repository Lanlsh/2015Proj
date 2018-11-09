// ServerByIOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <mutex>
#include "LanThreadPool.h"
using namespace std;

#pragma comment(lib,"ws2_32.lib")

//FD_SETSIZE是在winsocket2.h头文件里定义的，这里windows默认最大为64
//在包含winsocket2.h头文件前使用宏定义可以修改这个值

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

void WorkerThread()
{

}

int main()
{
	//创建完成端口
	HANDLE hdIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //最后一个参数，它代表NumberOfConcurrentThreads，也就是说允许应用程序同时执行的线程数量。我们设置为0，就是说有多少处理器就允许同时多少个线程运行。

	//根据系统中CPU核心的数量建立对于的work线程
	//获取系统核心数
	int nCPUCount = 0;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	nCPUCount = sysInfo.dwNumberOfProcessors;

	/*我们最好是建立CPU核心数量*2(也有人说是建立 CPU“核心数量 * 2 +2”个线程)那么多的线程，这样更可以充分利用CPU资源，因为完成端口的调度是非常智能的，
	  比如我们的Worker线程有的时候可能会有Sleep()或者WaitForSingleObject()之类的情况，这样同一个CPU核心上的另一个线程就可以代替这个Sleep的线程执行了；
	  因为完成端口的目标是要使得CPU满负荷的工作。
	  */
	int nThreadCount = 2 * nCPUCount;
	//获取线程池对象
	LanThreadPool* lanThreadPool = LanThreadPool::GetLanThreadPoolInstance();
	lanThreadPool->SetThreadPool(nThreadCount, nThreadCount);
	

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 这里需要特别注意，如果要使用重叠I/O的话，这里必须要使用WSASocket来初始化Socket
	// 注意里面有个WSA_FLAG_OVERLAPPED参数
	SOCKET sockListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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

	int nBind = ::bind(sockListen, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//一定要加“::”，否则bind为其它库的bind,返回值就不是int类型了！！！
	if (-1 == nBind)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind不成功！！！" << endl;
		return 0;
	}

	int nListenOk = listen(sockListen, SOMAXCONN);	//常量SOMAXCONN是在微软在WinSock2.h中定义的，注释:Maximum queue length specifiable by listen
	if (nListenOk == -1)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "nListenOk == -1" << endl;
		//assert(false);
		return 0;
	}

	//绑定IOCP用 CreateIoCompletionPort()函数，但是这里这个API可不是用来建立完成端口的，而是用于将Socket和以前创建的那个完成端口绑定的，大家可要看准了，不要被迷惑了，因为他们的参数是明显不一样的，前面那个的参数是一个-1，三个0
	/*HANDLE WINAPI CreateIoCompletionPort(
		__in      HANDLE  FileHandle,             // 这里当然是连入的这个套接字句柄了
		__in_opt  HANDLE  ExistingCompletionPort, // 这个就是前面创建的那个完成端口
		__in      ULONG_PTR CompletionKey,        // 这个参数就是类似于线程参数一样，在
												  // 绑定的时候把自己定义的结构体指针传递
												  // 这样到了Worker线程中，也可以使用这个
												  // 结构体的数据了，相当于参数的传递
		__in      DWORD NumberOfConcurrentThreads // 这里同样置0
	);*/



    return 0;
}

