#include "stdafx.h"
#include "MyClient.h"
#include <cstring>
#include <thread>

using namespace std;

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

MyClient::MyClient()
{
	memset(m_cIp, 0, 16 * sizeof(char));
	m_nPort = -1;
	m_bIsServerDisConnect = false;
	m_bIsLooping = true;
}


MyClient::~MyClient()
{
}

void MyClient::CreateClient(char * connectIp, int connectPort)
{
	if (connectIp == NULL)
	{
		cout << "连接的服务器的IP设置错误！！！！" << endl;
		return;
	}

	if ((connectPort< 0) || (connectPort>65535))
	{
		cout << "连接的服务器的端口设置错误！！！！" << endl;
		return;
	}

	memcpy(m_cIp, connectIp, 15 * sizeof(char));
	m_nPort = connectPort;
}

void MyClient::StartConnection()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		cout << "WSAStartup(sockVersion, &data) == 0!!!!!" << endl;
		return ;
	}

	SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		cout << "创建socket失败！！！！" << endl;
		return;
	}

	//创建一个线程，接收玩家输入信息
	std::thread thrdInput(&MyClient::ReceiveInput, this);
	thrdInput.detach();

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(m_nPort);
	//serAddr.sin_addr.S_un.S_addr = inet_addr(m_cIp);
	struct in_addr addr;
	inet_pton(AF_INET, m_cIp, (void *)&addr);
	serAddr.sin_addr.s_addr = addr.s_addr;

	ConnectToServer(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in));

	//socket连接时，如果远端服务器断开连接，这时客户端只能重现创建套接字才能重新连接到服务器！！！！

	cout << "sclient: " << sclient << endl;
	bool bIsServerDissconnect = false;
	while (m_bIsLooping) {
		//char recData[256];
		//int ret = recv(sclient, recData, 255, 0);
		//if (ret>0) {
		//	//cout << "server" << ": ";
		//	recData[ret] = 0x00;
		//	//printf(recData);
		//	//cout << endl;
		//}
		
		//cin.getline(input, 256);	//字符串的“空格”也会获取！！
		//send()用来将数据由指定的socket传给对方主机
		//int send(int s, const void * msg, int len, unsigned int flags)
		//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
		//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 

		if (!m_qeSendMessage.empty())
		{
			//取需要发送给server的信息
			string str = m_qeSendMessage.front();

			//对发送的报文信息进行封装
			int nSize = sizeof(char) * (sizeof(TCPMessage));
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
			
			int sendSuccessSize = send(sclient, cSendMessage, nSize, 0);
			free(cSendMessage);
			cSendMessage = NULL;

			if (sendSuccessSize < 0)
			{
				//cout << "client: 发送消息给server失败！！！！！！！！！" << endl;

				//重新连接到Server(这时，服务器断开，则客户端需要重新创建socket才能连接上server！！！！)
				bIsServerDissconnect = true;
				break;
				//ConnectToServer(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in));

			}
			else
			{
				m_qeSendMessage.pop();
				cout << "client自己发送内容: " << str.c_str() << endl;
			}
		}
		else
		{
			//发送心跳报文
			//对发送的报文信息进行封装
			int nSize = sizeof(char) * sizeof(TCPMessage);
			char* cSendMessage = (char*)malloc(nSize);
			memset(cSendMessage, 0, nSize);
			TCPMessage* tcpMessage = (TCPMessage*)cSendMessage;
			tcpMessage->bIsHeartBeat = true;
			tcpMessage->chMessage[0] = '1';

			int sendSuccessSize = send(sclient, cSendMessage, nSize, 0);
			free(cSendMessage);
			cSendMessage = NULL;

			if (sendSuccessSize < 0)
			{
				cout << "client: 发送消息给server失败！！！！！！！！！" << endl;

				//重新连接到Server(这时，服务器断开，则客户端需要重新创建socket才能连接上server！！！！)
				bIsServerDissconnect = true;
				break;
				//ConnectToServer(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in));

			}
		}


		//client 接收服务器的信息
		int nSize = sizeof(char) * (sizeof(TCPMessage));
		char* recvData = (char*)malloc(nSize);
		memset(recvData, 0, nSize);

		int recvClientMessageSize = recv(sclient, recvData, nSize, 0);		//如果没有收到客户端发的消息，就会一直阻塞！！
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
				cout << "server：" << tcpMessage->chMessage << endl;
			}

			free(recvData);
			recvData = NULL;
		}
		
		Sleep(1000);
	}

	closesocket(sclient);

	WSACleanup();

	if (bIsServerDissconnect)
	{
		StartConnection();
	}

}

void MyClient::ConnectToServer(SOCKET sclient, sockaddr * sckAddr, int nNameLength)
{
	while (true)
	{
		if (connect(sclient, sckAddr, nNameLength) == SOCKET_ERROR)
		{  
			cout << "connect失败！！！！" << GetLastError() << endl;
			Sleep(1000);
		}
		else
		{
			cout << "connect成功！！！！" << endl;
			break;
		}
	}
}

void MyClient::ReceiveInput(VOID)
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

		Sleep(1000);
	}
}
