#include "stdafx.h"
#include "MyClient.h"
#include <cstring>
#include <thread>

using namespace std;

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
		cout << "���ӵķ�������IP���ô��󣡣�����" << endl;
		return;
	}

	if ((connectPort< 0) || (connectPort>65535))
	{
		cout << "���ӵķ������Ķ˿����ô��󣡣�����" << endl;
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
		cout << "����socketʧ�ܣ�������" << endl;
		return;
	}

	//����һ���̣߳��������������Ϣ
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

	//socket����ʱ�����Զ�˷������Ͽ����ӣ���ʱ�ͻ���ֻ�����ִ����׽��ֲ����������ӵ���������������

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
		
		//cin.getline(input, 256);	//�ַ����ġ��ո�Ҳ���ȡ����
		//send()������������ָ����socket�����Է�����
		//int send(int s, const void * msg, int len, unsigned int flags)
		//sΪ�ѽ��������ӵ�socket��msgָ���������ݣ�len��Ϊ���ݳ��ȣ�����flagsһ����0
		//�ɹ��򷵻�ʵ�ʴ��ͳ�ȥ���ַ�����ʧ�ܷ���-1������ԭ�����error 

		if (!m_qeSendMessage.empty())
		{
			//ȡ��Ҫ���͸�server����Ϣ
			string str = m_qeSendMessage.front();

			//�Է��͵ı�����Ϣ���з�װ
			int nSize = sizeof(char) * (sizeof(TCPMessage));
			char* cSendMessage = (char*)malloc(nSize);
			memset(cSendMessage, 0, nSize);
			TCPMessage* tcpMessage = (TCPMessage*)cSendMessage;
			tcpMessage->bIsHeartBeat = false;

			//��Ϊ���͵�������ϢΪ���������ԣ�str.length()���ܳ���1024�ֽڣ�����
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
				//cout << "client: ������Ϣ��serverʧ�ܣ�����������������" << endl;

				//�������ӵ�Server(��ʱ���������Ͽ�����ͻ�����Ҫ���´���socket����������server��������)
				bIsServerDissconnect = true;
				break;
				//ConnectToServer(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in));

			}
			else
			{
				m_qeSendMessage.pop();
				cout << "client�Լ���������: " << str.c_str() << endl;
			}
		}
		else
		{
			//������������
			//�Է��͵ı�����Ϣ���з�װ
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
				cout << "client: ������Ϣ��serverʧ�ܣ�����������������" << endl;

				//�������ӵ�Server(��ʱ���������Ͽ�����ͻ�����Ҫ���´���socket����������server��������)
				bIsServerDissconnect = true;
				break;
				//ConnectToServer(sclient, (sockaddr *)(&serAddr), sizeof(sockaddr_in));

			}
		}


		//client ���շ���������Ϣ
		int nSize = sizeof(char) * (sizeof(TCPMessage));
		char* recvData = (char*)malloc(nSize);
		memset(recvData, 0, nSize);

		int recvClientMessageSize = recv(sclient, recvData, nSize, 0);		//���û���յ��ͻ��˷�����Ϣ���ͻ�һֱ��������
		if (recvClientMessageSize > 0)
		{
			//�����ͻ��˷��͵ı���
			TCPMessage* tcpMessage = (TCPMessage*)recvData;
			if (tcpMessage->bIsHeartBeat)
			{
				//cout << "client����������-------------------" << endl;
			}
			else
			{
				cout << "server��" << tcpMessage->chMessage << endl;
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
			cout << "connectʧ�ܣ�������" << GetLastError() << endl;
			Sleep(1000);
		}
		else
		{
			cout << "connect�ɹ���������" << endl;
			break;
		}
	}
}

void MyClient::ReceiveInput(VOID)
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

		Sleep(1000);
	}
}
