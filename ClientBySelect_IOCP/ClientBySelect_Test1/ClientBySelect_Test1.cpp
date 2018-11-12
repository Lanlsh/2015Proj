// ClientBySelect_Test1.cpp : �������̨Ӧ�ó������ڵ㡣
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

const int nMessageWordMaxSize = 4096;	//������Ϣ��������Ϣ�����ֽ���

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

#define SERVER_PORT 8888
#define SERVER_IP "192.168.15.112"
bool bIsHaveConnect = false;

//������Ϣ������������Ϣ����
std::queue<string> m_qeSendMessage;
void ReceiveInput(VOID)
{
	char input[nMessageWordMaxSize] = { 0 };
	while (true)
	{	
		cin.getline(input, nMessageWordMaxSize);	//�ַ����ġ��ո�Ҳ���ȡ����
		if (input[0] != '\0')
		{
			m_qeSendMessage.push(input);
			memset(input, 0, nMessageWordMaxSize);
		}

		//Sleep(1000);
	}
}

//������Ϣ��������
void SendMsgToServer(fd_set& fdTemp, fd_set& fdRead, SOCKET& sclient)
{
	//ȡ��Ҫ���͸�server����Ϣ
	string str = m_qeSendMessage.front();

	//�Է��͵ı�����Ϣ���з�װ
	char* cSendMessage = (char*)malloc(nMessageWordMaxSize);
	memset(cSendMessage, 0, nMessageWordMaxSize);

	//��Ϊ���͵�������ϢΪ���������ԣ�str.length()���ܳ���1024�ֽڣ�����
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

			//����ر���ص�socket�������clear�����socket
			FD_CLR(sclient, &fdTemp);
			FD_CLR(sclient, &fdRead);

			if (sclient != INVALID_SOCKET)
			{
				closesocket(sclient);
				sclient = INVALID_SOCKET;

				sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (sclient == INVALID_SOCKET)
				{
					cout << "����socketʧ�ܣ�������" << endl;
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
		cout << "����socketʧ�ܣ�������" << endl;
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

	//����һ���̣߳��������������Ϣ
	std::thread thrdInput(&ReceiveInput);
	thrdInput.detach();
	
	while (true)
	{
		if (!bIsHaveConnect)
		{
			if (connect(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				cout << "connectʧ�ܣ�������" << GetLastError() << endl;
				bIsHaveConnect = false;
				Sleep(1000);
				continue;
			}
			else
			{
				cout << "����" << SERVER_IP << ":" << SERVER_PORT << " " << "connect�ɹ���������" << endl;
				bIsHaveConnect = true;
				continue;
			}
		}

		//ÿ�ε�����select�󣬱���tv�ᱻ���ã����ԣ��������¸�ֵ����
		tv.tv_sec = 0;
		tv.tv_usec = 100;
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

		if (FD_ISSET(sclient, &fdTemp))
		{
			if (bIsHaveConnect)
			{
				memset(recvData, 0, nSize);
				//�������Կͻ��˵�����
				int nRecvLength = 0;
				while (nRecvLength != nMessageWordMaxSize)
				{
					int curLength = 0;
					curLength = recv(sclient, recvData, nMessageWordMaxSize, 0);
					if (curLength <= 0 || (curLength == SOCKET_ERROR))
					{
						cout << "server " << SERVER_IP << "closed" << endl;
						//����ر���ص�socket�������clear�����socket
						FD_CLR(sclient, &fdTemp);
						FD_CLR(sclient, &fdRead);

						//windows�׽��ֲ��ܱ��رն��!!!!!!
						if (sclient != INVALID_SOCKET)
						{
							closesocket(sclient);
							sclient = INVALID_SOCKET;

							sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
							if (sclient == INVALID_SOCKET)
							{
								cout << "����socketʧ�ܣ�������" << endl;
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

