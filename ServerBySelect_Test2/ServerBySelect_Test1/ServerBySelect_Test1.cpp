// ServerBySelect_Test1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <mutex>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

//FD_SETSIZE����winsocket2.hͷ�ļ��ﶨ��ģ�����windowsĬ�����Ϊ64
//�ڰ���winsocket2.hͷ�ļ�ǰʹ�ú궨������޸����ֵ

const int nMessageWordMaxSize = 1023;	//������Ϣ��������Ϣ�����ֽ���
//TCP��Ϣ��װ�ṹ
struct TCPMessage
{
	bool bIsHeartBeat;	//�Ƿ�Ϊ��������
	char chMessage[nMessageWordMaxSize];	//���͵�������Ϣ(����Ϊ�����Ĵ�С)  //�����Ҫ���ö���������Ҫʹ����ģʽȥ���ݣ�����	

	TCPMessage()
	{
		bIsHeartBeat = true;
		memset(chMessage, 0, nMessageWordMaxSize);
	}
};

const int nSize = sizeof(char) * sizeof(TCPMessage);

#define PORT 8888
#define SRV_IP "192.168.15.112"

int g_nSockConn = 0;//�������ӵ���Ŀ

struct ClientInfo
{
	SOCKET sockClient;
	SOCKADDR_IN addrClient;
};

ClientInfo g_Client[FD_SETSIZE];

//������Ϣ���ͻ��˵���Ϣ����
std::queue<string> m_qeSendMessage;
std::mutex mutex_sendMsg;

void ReceiveInput(VOID)
{
	char input[nMessageWordMaxSize] = { 0 };
	while (true)
	{
		cin.getline(input, nMessageWordMaxSize);	//�ַ����ġ��ո�Ҳ���ȡ����
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

//����������
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
			//����ر���ص�socket�������clear�����socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//��ʧЧ��sockClient�޳�������������һ������ȥ
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

//�����Լ�����Ϣ���ͻ���
void SendMsgToClient(char* recvData, int i, fd_set& fdTemp, fd_set& fdRead, string strMsg)
{
	//���Լ�����Ϣ
	TCPMessage* tcpMessage = (TCPMessage*)recvData;
	memset(tcpMessage->chMessage, 0, nMessageWordMaxSize);
	tcpMessage->bIsHeartBeat = false;

	//��Ϊ���͵�������ϢΪ���������ԣ�str.length()���ܳ���1024�ֽڣ�����
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
			//����ر���ص�socket�������clear�����socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//��ʧЧ��sockClient�޳�������������һ������ȥ
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
			//����ر���ص�socket�������clear�����socket
			FD_CLR(g_Client[i].sockClient, &fdTemp);
			FD_CLR(g_Client[i].sockClient, &fdRead);
			closesocket(g_Client[i].sockClient);

			if (i < g_nSockConn - 1)
			{
				//��ʧЧ��sockClient�޳�������������һ������ȥ
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
	//��������Ͳ������ˣ������̻�����ƪ�����ｲ�ĺ���ϸ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/*
	SOCKET PASCAL FAR socket( int af, int type, int protocol);
	af��һ����ַ������Ŀǰ��֧��AF_INET��ʽ��Ҳ����˵ARPA Internet��ַ��ʽ��
	type�����׽ӿڵ�����������
	protocol���׽ӿ����õ�Э�顣������߲���ָ��������0ָ������ʾȱʡ��
	*/

	//����һ���׽��֣����󷵻�-1
	SOCKET sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sockListen == SOCKET_ERROR)
	{
		cout << "sockListen == -1" << endl;
		return 0;
	}
	else
	{
		cout << "�������Լ��ļ���socket�� " << sockListen << endl;
	}

	//���÷������Ķ˿�����������������ʹ�ö�����ռ��
	bool option = true;
	int optlen = sizeof(option);
	setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, (char*)&option, optlen);


	//��IP�Ͷ˿ںŵ�׼������
	struct sockaddr_in my_addr;	//sin_port�� sin_addr �����������ֽ�˳��(Network Byte Order)��
	my_addr.sin_family = AF_INET; //* host byte order */ �����ֽ�����   
	//����Ҳ��Ҫע��ļ������顣my_addr.sin_port �������ֽ�˳�� my_addr.sin_addr.s_addr Ҳ�ǵ�
	my_addr.sin_port = htons(PORT); /* short, network byte order */  //���������ֽ�����ת���ɡ������ֽ������short���ͣ�2�ֽڣ���
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY��ʾ����IP��ַ���˴����ñ������У����ʹ��ָ����IP����Ϊmy_addr.sin_addr.s_addr = inet_addr(��ָ��IP��);��
	//my_addr.sin_addr.s_addr = inet_addr(SRV_IP); 
	memset(&(my_addr.sin_zero), 0, sizeof(8)); /* zero the rest of the struct */

	//�����׽��ְ�IP�Ͷ˿ں�
	//������� bind() ��ʱ����ҪС�ĵ���һ�������ǣ���Ҫ����С�� 1024�Ķ˿ںš�����С��1024�Ķ˿ںŶ���ϵͳ�����������ѡ���1024 ��65535�Ķ˿�(�������û�б���ĳ���ʹ�õĻ�)�� 
	//��Ҫע�������һ��С���ǣ���ʱ�����������Ҫ�������������ʹ �� connect() ����Զ�̻�������ͨѶ���㲻��Ҫ������ı��ض˿ں�(���� ����ʹ�� telnet ��ʱ��)����ֻҪ�򵥵ĵ��� connect() �Ϳ����ˣ������ ���׽����Ƿ�󶨶˿ڣ����û�У������Լ���һ��û��ʹ�õı��ض� �ڡ�
	int nBind = ::bind(sockListen, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//һ��Ҫ�ӡ�::��������bindΪ�������bind,����ֵ�Ͳ���int�����ˣ�����
	if (-1 == nBind)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind���ɹ�������" << endl;
		return 0;
	}

	//int listen(int sockfd, int backlog);
	//sockfd �ǵ��� socket() ���ص��׽����ļ���������backlog ���ڽ��� �����������������Ŀ��ʲô��˼��? ������������ڶ�����һֱ�ȴ�ֱ ������� (accept() �뿴���������)���ӡ����ǵ���Ŀ�����ڶ��е����� �����ϵͳ��������Ŀ��20����Ҳ��������Ϊ5��10��
	//�ͱ�ĺ���һ�����ڷ��������ʱ�򷵻� - 1��������ȫ�ִ������ errno��
	int nListenOk = listen(sockListen, 64);	//64Ҳ���ǡ����ٵȴ����ӿ��ơ�����˼
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

	//����һ���̣߳��������������Ϣ
	std::thread thrdInput(&ReceiveInput);
	thrdInput.detach();

	while (true)
	{
		TIMEVAL tv;//���ó�ʱ�ȴ�ʱ��
		//ÿ�ε�����select�󣬱���tv�ᱻ���ã����ԣ��������¸�ֵ����
		tv.tv_sec = 0;
		tv.tv_usec = 200;
		fdTemp = fdRead;

		//ֻ����read�¼����������滹�ǻ��ж�д��Ϣ���͵�
		//select�ɹ�ʱ����0�� ʧ��ʱ����-1 
		nRet = select(1, &fdTemp, NULL, NULL, &tv);	//windows�µ�select�����ĵ�һ��������û����ģ�linux��Ϊsocket��������������linux��socket������Ҳ���ļ���������

		if (nRet == -1)
		{
			cout << "select() error: " << GetLastError() << endl;
			break;
		}
		//else if (nRet == 0)
		//{
		//	//û�����ӻ���û�ж��¼�
		//	Sleep(100);
		//	continue;
		//}

		if (FD_ISSET(sockListen, &fdTemp))
		{
			//��������
			sockClient = accept(sockListen, (SOCKADDR*)&addrClient, &nLenAddrClient);

			if (sockClient != INVALID_SOCKET)
			{
				g_Client[g_nSockConn].addrClient = addrClient;//�������Ӷ˵�ַ��Ϣ
				g_Client[g_nSockConn].sockClient = sockClient;//���������߶���
				FD_SET(g_Client[g_nSockConn].sockClient, &fdRead);
				g_nSockConn++;
				//��������ߵĵ�ַ��Ϣ
				cout << inet_ntoa(addrClient.sin_addr) << ":" << ntohs(addrClient.sin_port) <<":" << sockClient << "���ӳɹ� !" << endl;
			}
		}
		//else
		//{
		for (int i = 0; i < g_nSockConn; i++)
		{
			if (FD_ISSET(g_Client[i].sockClient, &fdTemp))
			{
				memset(recvData, 0, nSize);
				//�������Կͻ��˵�����
				bool bRecvOk = false;
				bRecvOk = RecvClientMsg(recvData, i, fdTemp, fdRead);

				if (bRecvOk)
				{
					//��ʾ�����Կͻ��˵���Ϣ
					TCPMessage* recvMsg = (TCPMessage*)recvData;
					if (recvMsg->bIsHeartBeat == false)
					{
						cout << "Client " << inet_ntoa(g_Client[i].addrClient.sin_addr) << ": " << recvMsg->chMessage << endl;
					}

					//������Ϣ���ͻ���
					int nSendLength = 0;
					if (m_qeSendMessage.empty())
					{
						//����������
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
			//������������IP���Լ����������
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

		//Ϊ�����Լ���������������Լӵ�queue��
		//Sleep(100);

	}

	closesocket(sockListen);
	WSACleanup();

	return 0;
}

