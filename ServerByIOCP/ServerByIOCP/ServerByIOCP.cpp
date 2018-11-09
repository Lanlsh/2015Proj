// ServerByIOCP.cpp : �������̨Ӧ�ó������ڵ㡣
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

//FD_SETSIZE����winsocket2.hͷ�ļ��ﶨ��ģ�����windowsĬ�����Ϊ64
//�ڰ���winsocket2.hͷ�ļ�ǰʹ�ú궨������޸����ֵ

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

void WorkerThread()
{

}

int main()
{
	//������ɶ˿�
	HANDLE hdIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //���һ��������������NumberOfConcurrentThreads��Ҳ����˵����Ӧ�ó���ͬʱִ�е��߳���������������Ϊ0������˵�ж��ٴ�����������ͬʱ���ٸ��߳����С�

	//����ϵͳ��CPU���ĵ������������ڵ�work�߳�
	//��ȡϵͳ������
	int nCPUCount = 0;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	nCPUCount = sysInfo.dwNumberOfProcessors;

	/*��������ǽ���CPU��������*2(Ҳ����˵�ǽ��� CPU���������� * 2 +2�����߳�)��ô����̣߳����������Գ������CPU��Դ����Ϊ��ɶ˿ڵĵ����Ƿǳ����ܵģ�
	  �������ǵ�Worker�߳��е�ʱ����ܻ���Sleep()����WaitForSingleObject()֮������������ͬһ��CPU�����ϵ���һ���߳̾Ϳ��Դ������Sleep���߳�ִ���ˣ�
	  ��Ϊ��ɶ˿ڵ�Ŀ����Ҫʹ��CPU�����ɵĹ�����
	  */
	int nThreadCount = 2 * nCPUCount;
	//��ȡ�̳߳ض���
	LanThreadPool* lanThreadPool = LanThreadPool::GetLanThreadPoolInstance();
	lanThreadPool->SetThreadPool(nThreadCount, nThreadCount);
	

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// ������Ҫ�ر�ע�⣬���Ҫʹ���ص�I/O�Ļ����������Ҫʹ��WSASocket����ʼ��Socket
	// ע�������и�WSA_FLAG_OVERLAPPED����
	SOCKET sockListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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

	int nBind = ::bind(sockListen, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//һ��Ҫ�ӡ�::��������bindΪ�������bind,����ֵ�Ͳ���int�����ˣ�����
	if (-1 == nBind)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind���ɹ�������" << endl;
		return 0;
	}

	int nListenOk = listen(sockListen, SOMAXCONN);	//����SOMAXCONN����΢����WinSock2.h�ж���ģ�ע��:Maximum queue length specifiable by listen
	if (nListenOk == -1)
	{
		cout << "m_sck: " << sockListen << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "nListenOk == -1" << endl;
		//assert(false);
		return 0;
	}

	//��IOCP�� CreateIoCompletionPort()�����������������API�ɲ�������������ɶ˿ڵģ��������ڽ�Socket����ǰ�������Ǹ���ɶ˿ڰ󶨵ģ���ҿ�Ҫ��׼�ˣ���Ҫ���Ի��ˣ���Ϊ���ǵĲ��������Բ�һ���ģ�ǰ���Ǹ��Ĳ�����һ��-1������0
	/*HANDLE WINAPI CreateIoCompletionPort(
		__in      HANDLE  FileHandle,             // ���ﵱȻ�����������׽��־����
		__in_opt  HANDLE  ExistingCompletionPort, // �������ǰ�洴�����Ǹ���ɶ˿�
		__in      ULONG_PTR CompletionKey,        // ������������������̲߳���һ������
												  // �󶨵�ʱ����Լ�����Ľṹ��ָ�봫��
												  // ��������Worker�߳��У�Ҳ����ʹ�����
												  // �ṹ��������ˣ��൱�ڲ����Ĵ���
		__in      DWORD NumberOfConcurrentThreads // ����ͬ����0
	);*/



    return 0;
}

