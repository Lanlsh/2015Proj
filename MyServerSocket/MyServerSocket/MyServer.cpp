#include "stdafx.h"
#include "MyServer.h"
#include "Common.h"

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

MyServer::MyServer()
{
	m_sck = -1;	//socket文件描述符
	memset(m_cIp, 0, 16*sizeof(char));
	m_nPort = 0;	//端口号
	m_bIsStartUplisten = false;
	m_mapNametoIp.clear(); //保存连接者的名字和其IP地址
	m_mapNametoMessageQueue.clear();
	m_mapNameToSocket.clear();
}


MyServer::~MyServer()
{
	if (m_sck != -1)
	{
		//关闭socket
		closesocket(m_sck);
	}

	for (map<string, SOCKET>::iterator iter = m_mapNameToSocket.begin(); iter!=m_mapNameToSocket.end(); iter++)
	{
		//关闭socket
		closesocket(iter->second);
	}

	//int WSACleanup(void);
	//应用程序在完成对请求的Socket库的使用后，要调用WSACleanup函数来解除与Socket库的绑定并且释放Socket库所占用的系统资源。
	WSACleanup();
}

void MyServer::CreatMyServer(char * ip, int port)
{
	if ((ip == NULL) || (port < 0) || (port > 65535))
	{
		assert(ip);
		assert((port >= 0) && (port <= 65535));
		return;
	}

	//int WSAStartup ( WORD wVersionRequested, LPWSADATA lpWSAData );   成功返回0！！
	//使用Socket的程序在使用Socket之前"必须调用WSAStartup函数"。以后应用程序就可以调用所请求的Socket库中的其它Socket函数了!!!!!!!!!
	//例：假如一个程序要使用2.1版本的Socket, 那么程序代码如下
	//	wVersionRequested = MAKEWORD(2, 1);
	//err = WSAStartup(wVersionRequested, &wsaData);
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		assert(false);
		return;
	}

	memcpy(m_cIp, ip, 15*sizeof(char));
	m_nPort = port;

	/*
	SOCKET PASCAL FAR socket( int af, int type, int protocol);
	af：一个地址描述。目前仅支持AF_INET格式，也就是说ARPA Internet地址格式。
	type：新套接口的类型描述。
	protocol：套接口所用的协议。如调用者不想指定，可用0指定，表示缺省。
	*/
	//创建一个套接字，错误返回-1
	m_sck = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sck == -1)
	{
		cout << "m_sck == -1" << endl;
		return;
	}

	//绑定IP和端口号的准备工作
	struct sockaddr_in my_addr;	//sin_port和 sin_addr 必须是网络字节顺序(Network Byte Order)！
	my_addr.sin_family = AF_INET; //* host byte order */ 本机字节排序   
	//这里也有要注意的几件事情。my_addr.sin_port 是网络字节顺序， my_addr.sin_addr.s_addr 也是的
	my_addr.sin_port = htons(m_nPort); /* short, network byte order */  //将“本机字节排序”转换成“网络字节排序的short类型（2字节）”
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY表示本机IP地址（此处先用本机进行，如果使用指定的IP，则为my_addr.sin_addr.s_addr = inet_addr(“指定IP”);）

	memset(&(my_addr.sin_zero), 0, sizeof(8)) ; /* zero the rest of the struct */

	//将此套接字绑定IP和端口号
	//在你调用 bind() 的时候，你要小心的另一件事情是：不要采用小于 1024的端口号。所有小于1024的端口号都被系统保留！你可以选择从1024 到65535的端口(如果它们没有被别的程序使用的话)。 
	//你要注意的另外一件小事是：有时候你根本不需要调用它。如果你使 用 connect() 来和远程机器进行通讯，你不需要关心你的本地端口号(就象 你在使用 telnet 的时候)，你只要简单的调用 connect() 就可以了，它会检 查套接字是否绑定端口，如果没有，它会自己绑定一个没有使用的本地端 口。
	int nBind = ::bind(m_sck, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//一定要加“::”，否则bind为其它库的bind,返回值就不是int类型了！！！
	if (-1 == nBind)
	{
		cout << "m_sck: " << m_sck << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind不成功！！！" << endl;
		return;
	}


	//int WSACleanup(void);
	//应用程序在完成对请求的Socket库的使用后，要调用WSACleanup函数来解除与Socket库的绑定并且释放Socket库所占用的系统资源。
	//WSACleanup();

	//创建一个线程，接收玩家输入信息
	std::thread thrdInput(&MyServer::ReceiveInput, this);
	thrdInput.detach();
}

void MyServer::StartServerByName(string strName)
{
	if (m_sck == -1)
	{
		return;
	}

	//设置接受函数，即有人连接上时，它的连接将加入到等待接受 (accept()) 的队列 中。你调用 accept() 告诉它你有空闲的连接。它将返回一个新的套接字文 件描述符！这样你就有两个套接字了，原来的一个还在侦听你的那个端口， 新的在准备发送 (send()) 和接收 ( recv()) 数据。这就是这个过程！
	/*
	int accept(int sockfd, void *addr, int *addrlen);
	sockfd 相当简单，是和 listen() 中一样的套接字描述符。addr 是个指 向局部的数据结构 sockaddr_in 的指针。这是要求接入的信息所要去的地 方（你可以测定那个地址在那个端口呼叫你）。在它的地址传递给 accept 之 前，addrlen 是个局部的整形变量，设置为 sizeof(struct sockaddr_in)。 accept 将不会将多余的字节给 addr。如果你放入的少些，那么它会通过改 变 addrlen 的值反映出来。
	同样，在错误时返回-1，并设置全局错误变量 errno。
	*/
	struct sockaddr_in their_addr; /* connector's address information */	//sin_port和 sin_addr 必须是网络字节顺序(Network Byte Order)！
	their_addr.sin_family = AF_INET;
	char theirAddr[16] = { 0 };
	memcpy(theirAddr, m_mapNametoIp[strName].c_str(), 15 * sizeof(char));

	//因为inet_addr()函数不能用，所以需要如下转换
	struct in_addr addr;
	inet_pton(AF_INET, theirAddr, (void *)&addr);

	their_addr.sin_addr.s_addr = addr.s_addr;

	//inet_addr函数返回的已经是网络字节格式！！！
	//它可以将一个in_addr结构体输出成点数格式？这样的话，你就要用到函数 inet_ntoa()("ntoa"的含义是"network to ascii")，就像这样：
	//printf("%s", inet_ntoa(ina.sin_addr));
	//它将输出IP地址。需要注意的是inet_ntoa()将结构体in - addr作为一个参数，不是长整形。同样需要注意的是它返回的是一个指向一个字符的 指针。它是一个由inet_ntoa()控制的静态的固定的指针，所以每次调用 inet_ntoa()，它就将覆盖上次调用时所得的IP地址。

	memset(&(their_addr.sin_zero), 0, sizeof(8)); /* zero the rest of the struct */

	char IPdotdec[20] = { 0 };
	inet_ntop(AF_INET, &their_addr.sin_addr, IPdotdec, 15);

	//linux inet_ntoa(their_addr.sin_addr)
	//server 发送信息到 目的客户端
	//std::thread thrd(&Common::ServerSayHelloToClient, sck_cliend);
	//thrd.detach();	//执行子线程，程序执行完后，子线程自动释放内存
	
	SOCKET sck_cliend = -1;

	AddServerToConnectQueueByName(strName);
	bool bAcceptIsSuccess = false;
	bool bIsNeedCreateAcceptThread = true;
	while (true)
	{
		//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), (sockaddr*)(&their_addr), IPdotdec, std::ref(bAcceptIsSuccess)); //C++中的thread的参数不能直接为引用
		//thrdAccesThread.join();
		AcceptFromClient(sck_cliend, (sockaddr*)(&their_addr), IPdotdec, bAcceptIsSuccess, bIsNeedCreateAcceptThread);
		DoRecvAndSendMessage(sck_cliend, (sockaddr*)(&their_addr), bAcceptIsSuccess, strName, IPdotdec);
		Sleep(1000);
	}

	m_mapNameToSocket[strName] = sck_cliend;

}

void MyServer::StartServerAllIp()
{
	StartListen();

	for (map<string, string>::const_iterator iter=m_mapNametoIp.begin(); iter!=m_mapNametoIp.end(); iter++)
	{
		std::thread thrd(&MyServer::StartServerByName, this, iter->first);
		thrd.join();
		
		//StartServerByName(iter->first);
	}
}

void MyServer::StartListen()
{
	if (m_bIsStartUplisten)
	{
		return;
	}
	//int listen(int sockfd, int backlog);
	//sockfd 是调用 socket() 返回的套接字文件描述符。backlog 是在进入 队列中允许的连接数目。什么意思呢? 进入的连接是在队列中一直等待直 到你接受 (accept() 请看下面的文章)连接。它们的数目限制于队列的允许。 大多数系统的允许数目是20，你也可以设置为5到10。
	//和别的函数一样，在发生错误的时候返回 - 1，并设置全局错误变量 errno。

	//开始监听
	int nListenOk = listen(m_sck, 10);	//10也就是“多少等待连接控制”的意思
	if (nListenOk == -1)
	{
		cout << "m_sck: " << m_sck << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "nListenOk == -1" << endl;
		//assert(false);
		return;
	}

	m_bIsStartUplisten = true;
}

void MyServer::AddConnectNameAndIp(string strName, string strIp)
{
	m_mapNametoIp[strName] = strIp;  //此种方式插入，如果发现key之前就已存在，则直接替换value
	//m_mapNametoIp.insert(pair<string, string>(strName, strIp));	//此种替换，如果发现之前的key已存在，则此句不会执行！！！！
}

void MyServer::AddServerToConnectQueueByName(string strName)
{
	queue<string> qeMessage;
	m_mapNametoMessageQueue.insert( pair<string, queue<string> >(strName, qeMessage) );	//此种替换，如果发现之前的key已存在，则此句不会执行！！！！
}

void MyServer::AcceptFromClient(SOCKET & socket, sockaddr * sckAddr, char* IPdotdec, bool& bIsAcceptSuccess, bool& bIsNeedCreateAcceptThread)
{
	//cout << "跟" << IPdotdec << "accept中！！！" << endl;
	if (bIsAcceptSuccess)
	{
		return;
	}

	if (bIsNeedCreateAcceptThread)
	{
		//一个客户端，只创建一个监听accpt线程！！！
		Common::CreateListenAccepThread(m_sck, sckAddr, socket, IPdotdec, bIsAcceptSuccess);
		bIsNeedCreateAcceptThread = false;
	}
	
	//int sin_size = sizeof(struct sockaddr_in);
	//socket = accept(m_sck, sckAddr, &sin_size);	//accept  是阻塞的，如果对应的IP没有连接上来，就会在这一直卡住！！想不卡住，则启动一个线程专门用来监听！！！！
																		//在系统调用 send() 和 recv() 中你应该使用新的套接字描述符 new_fd。如果你只想让一个连接进来，那么你可以使用 close() 去关闭原 来的文件描述符 m_sck 来避免同一个端口更多的连接

}

void MyServer::DoRecvAndSendMessage(SOCKET& sck_cliend, sockaddr* their_addr, bool bAcceptIsSuccess, string strName, char* IPdotdec)
{
	//记住： recv和accept都是阻塞的！！！！！！！！！！！！！！！！！！！！！
	int sin_size = sizeof(struct sockaddr_in);
	while (bAcceptIsSuccess)
	{
		if (!m_mapNametoMessageQueue[strName].empty())
		{
			//取需要发送给server的信息
			string str = m_mapNametoMessageQueue[strName].front();

			//对发送的报文信息进行封装
			int nSize = sizeof(char) * (sizeof(TCPMessage) + nMessageWordMaxSize);
			char* cSendMessage = (char*)malloc(nSize);
			memset(cSendMessage, 0, nSize);
			TCPMessage* tcpMessage = (TCPMessage*)cSendMessage;
			tcpMessage->bIsHeartBeat = false;

			//因为发送的文字信息为定长，所以，str.length()不能超过1024字节！！！
			if (str.length() >= nMessageWordMaxSize)
			{
				memcpy(tcpMessage->chMessage, str.c_str(), nMessageWordMaxSize);
			}
			else
				memcpy(tcpMessage->chMessage, str.c_str(), str.length());

			int sendSuccessSize = send(sck_cliend, cSendMessage, nSize, 0);
			free(cSendMessage);
			cSendMessage = NULL;

			if (sendSuccessSize < 0)
			{
				cout << "server: 发送消息给客户端" << strName.c_str() << "失败！！！！！！！！！" << endl;
				bAcceptIsSuccess = false;
				bool bIsNeedCreateAcceptThread = true;
				while (!bAcceptIsSuccess)
				{
					//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), their_addr, IPdotdec, std::ref(bAcceptIsSuccess)); //C++中的thread的参数不能直接为引用
					//thrdAccesThread.detach();
					AcceptFromClient(sck_cliend, their_addr, IPdotdec, bAcceptIsSuccess, bIsNeedCreateAcceptThread);
					if (!bAcceptIsSuccess)
					{
						Sleep(1000);
					}
					else
					{
						if (sck_cliend != -1)
						{
							m_mapNameToSocket[IPdotdec] = sck_cliend;//此种方式插入，如果发现key之前就已存在，则直接替换value
						}
					}
					
				}

				continue;

			}
			else
			{
				m_mapNametoMessageQueue[strName].pop();
				cout << "发送内容给" << strName.c_str() << ": " << str.c_str() << endl;
			}
		}
		else
		{
			//发送心跳报文
			//对发送的报文信息进行封装
			int nSize = sizeof(char) * (sizeof(TCPMessage));
			char* cSendMessage = (char*)malloc(nSize);
			memset(cSendMessage, 0, nSize);
			TCPMessage* tcpMessage = (TCPMessage*)cSendMessage;
			tcpMessage->bIsHeartBeat = true;
			//char chTemp[10] = "66666";
			//memcpy(tcpMessage->chMessage, chTemp, 10);
			tcpMessage->chMessage[0] = '1';

			int sendSuccessSize = send(sck_cliend, cSendMessage, nSize, 0);
			free(cSendMessage);
			cSendMessage = NULL;

			if (sendSuccessSize < 0)
			{
				cout << "server: 发送消息给客户端" << strName.c_str() << "失败！！！！！！！！！" << endl;
				bAcceptIsSuccess = false;
				bool bIsNeedCreateAcceptThread = true;
				while (!bAcceptIsSuccess)
				{
					//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), their_addr, IPdotdec, std::ref(bAcceptIsSuccess)); //C++中的thread的参数不能直接为引用
					//thrdAccesThread.detach();
					AcceptFromClient(sck_cliend, their_addr, IPdotdec, bAcceptIsSuccess, bIsNeedCreateAcceptThread);
					if (!bAcceptIsSuccess)
					{
						Sleep(1000);
					}
					else
					{
						if (sck_cliend != -1)
						{
							m_mapNameToSocket[IPdotdec] = sck_cliend;//此种方式插入，如果发现key之前就已存在，则直接替换value
						}
					}
					
				}

				continue;
			}

		}

		//server 接收目的客户端的信息
		int nSize = sizeof(char) * (sizeof(TCPMessage));
		char* recvData = (char*)malloc(nSize);
		memset(recvData, 0, nSize);

		int recvClientMessageSize = recv(sck_cliend, recvData, nSize, 0);		//如果没有收到客户端发的消息，就会一直阻塞！！
		if (recvClientMessageSize > 0)
		{
			//解析客户端发送的报文
			TCPMessage* tcpMessage = (TCPMessage*)recvData;
			if (tcpMessage->bIsHeartBeat)
			{
				//cout << "client：心跳报文-------------------" << endl;
			}
			else
			{
				cout << "client：" << tcpMessage->chMessage << endl;
			}
		}
		else
		{
			cout << "recv(sck_cliend, recvData, 255, 0) == -1   !!!!!!!!!!!!!!!!!!!!!!" << endl << recvData << endl;
		}

		free(recvData);
		recvData = NULL;

		Sleep(1000);
	}
}

void MyServer::ReceiveInput(VOID)
{
	char input[nMessageWordMaxSize] = { 0 };
	while (true)
	{
		cin.getline(input, nMessageWordMaxSize);	//字符串的“空格”也会获取！！
		if (input[0] != '\0')
		{
			for (map<string, queue<string> >::iterator iter= m_mapNametoMessageQueue.begin(); iter!=m_mapNametoMessageQueue.end(); iter++)
			{
				iter->second.push(input);
			}
			
			memset(input, 0, nMessageWordMaxSize);
		}

		Sleep(1000);
	}
}
