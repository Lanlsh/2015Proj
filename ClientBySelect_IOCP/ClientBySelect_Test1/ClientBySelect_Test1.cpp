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

/*
	������ �ͻ��˷�����Ϣͷ�������ṹ��
	sin_addr: IP��ַ
	sin_port: �˿ں�
*/
const int nMessageBufMaxSize = nMessageWordMaxSize - 22;	//������Ϣ��������Ϣ�����ֽ���
typedef struct ST_SendToIpInfo
{
	char sin_addr[16];
	char sin_port[6];
	char buf[nMessageBufMaxSize];
}SendToIpInfo;

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


//��:�ָ��ַ���
void SplitString(string str, SendToIpInfo& ipInfo)
{
	ZeroMemory(ipInfo.sin_addr, 16);
	ZeroMemory(ipInfo.sin_port, 6);
	ZeroMemory(ipInfo.buf, nMessageBufMaxSize);
	char buf[16];
	char strinfo[nMessageWordMaxSize];
	ZeroMemory(strinfo, nMessageWordMaxSize);
	memcpy(strinfo, str.c_str(), str.length());
	char* p = strtok_s(strinfo, ":", (char**)(&buf));

	int count = 0;
	while (p)
	{
		switch (count)
		{
		case 0:
			memcpy(ipInfo.sin_addr, p, 16);
			break;
		case 1:
			memcpy(ipInfo.sin_port, p, 6);
			break;
		case 2:
			memcpy(ipInfo.buf, p, nMessageBufMaxSize);
			break;
		}

		count++;
		p = strtok_s(NULL, ":", (char**)(&buf));
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

	////��Ϊ���͵�������ϢΪ���������ԣ�str.length()���ܳ���1024�ֽڣ�����
	//if (str.length() >= nMessageWordMaxSize)
	//{
	//	memcpy(cSendMessage, str.c_str(), nMessageWordMaxSize);
	//}
	//else
	//	memcpy(cSendMessage, str.c_str(), str.length());

	/*
		�ͻ��������ʽ��IP:�˿ں�:������Ϣ
	*/

	SendToIpInfo ipInfo;
	SplitString(str, ipInfo);
	memcpy(cSendMessage, &ipInfo, nMessageWordMaxSize);
	ULONG ulFlags = MSG_PARTIAL;
	DWORD dwNumBytesOfRecvd;
	WSABUF	m_wsaBuf;
	m_wsaBuf.buf = cSendMessage;
	m_wsaBuf.len = nMessageWordMaxSize;

	INT nRet = WSASend(
		sclient,
		&m_wsaBuf,
		1,
		&(m_wsaBuf.len),
		ulFlags,
		NULL,
		NULL);

	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
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
				return;
			}

			FD_SET(sclient, &fdRead);
		}

		bIsHaveConnect = false;
	}
	else
	{
		m_qeSendMessage.pop();
		cout<<ipInfo.sin_addr << ":" << ipInfo.sin_port << ":" << ipInfo.buf << "     �������ݳɹ�����������������" << endl;
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

				ULONG ulFlags = MSG_PARTIAL;
				DWORD dwNumBytesOfRecvd;
				WSABUF	m_wsaBuf;
				m_wsaBuf.buf = recvData;
				m_wsaBuf.len = nMessageWordMaxSize;

				UINT nRet = WSARecv(
					sclient,
					&(m_wsaBuf),
					1,
					&dwNumBytesOfRecvd,// ���յ��ֽ������첽�����ķ��ؽ��һ��Ϊ0��������յ����ֽ�������ɶ˿ڻ��
					&(ulFlags),
					NULL,
					NULL);

				if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
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
				}
				else
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

