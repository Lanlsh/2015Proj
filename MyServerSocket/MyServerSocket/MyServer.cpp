#include "stdafx.h"
#include "MyServer.h"
#include "Common.h"

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

MyServer::MyServer()
{
	m_sck = -1;	//socket�ļ�������
	memset(m_cIp, 0, 16*sizeof(char));
	m_nPort = 0;	//�˿ں�
	m_bIsStartUplisten = false;
	m_mapNametoIp.clear(); //���������ߵ����ֺ���IP��ַ
	m_mapNametoMessageQueue.clear();
	m_mapNameToSocket.clear();
}


MyServer::~MyServer()
{
	if (m_sck != -1)
	{
		//�ر�socket
		closesocket(m_sck);
	}

	for (map<string, SOCKET>::iterator iter = m_mapNameToSocket.begin(); iter!=m_mapNameToSocket.end(); iter++)
	{
		//�ر�socket
		closesocket(iter->second);
	}

	//int WSACleanup(void);
	//Ӧ�ó�������ɶ������Socket���ʹ�ú�Ҫ����WSACleanup�����������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��
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

	//int WSAStartup ( WORD wVersionRequested, LPWSADATA lpWSAData );   �ɹ�����0����
	//ʹ��Socket�ĳ�����ʹ��Socket֮ǰ"�������WSAStartup����"���Ժ�Ӧ�ó���Ϳ��Ե����������Socket���е�����Socket������!!!!!!!!!
	//��������һ������Ҫʹ��2.1�汾��Socket, ��ô�����������
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
	af��һ����ַ������Ŀǰ��֧��AF_INET��ʽ��Ҳ����˵ARPA Internet��ַ��ʽ��
	type�����׽ӿڵ�����������
	protocol���׽ӿ����õ�Э�顣������߲���ָ��������0ָ������ʾȱʡ��
	*/
	//����һ���׽��֣����󷵻�-1
	m_sck = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sck == -1)
	{
		cout << "m_sck == -1" << endl;
		return;
	}

	//��IP�Ͷ˿ںŵ�׼������
	struct sockaddr_in my_addr;	//sin_port�� sin_addr �����������ֽ�˳��(Network Byte Order)��
	my_addr.sin_family = AF_INET; //* host byte order */ �����ֽ�����   
	//����Ҳ��Ҫע��ļ������顣my_addr.sin_port �������ֽ�˳�� my_addr.sin_addr.s_addr Ҳ�ǵ�
	my_addr.sin_port = htons(m_nPort); /* short, network byte order */  //���������ֽ�����ת���ɡ������ֽ������short���ͣ�2�ֽڣ���
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY��ʾ����IP��ַ���˴����ñ������У����ʹ��ָ����IP����Ϊmy_addr.sin_addr.s_addr = inet_addr(��ָ��IP��);��

	memset(&(my_addr.sin_zero), 0, sizeof(8)) ; /* zero the rest of the struct */

	//�����׽��ְ�IP�Ͷ˿ں�
	//������� bind() ��ʱ����ҪС�ĵ���һ�������ǣ���Ҫ����С�� 1024�Ķ˿ںš�����С��1024�Ķ˿ںŶ���ϵͳ�����������ѡ���1024 ��65535�Ķ˿�(�������û�б���ĳ���ʹ�õĻ�)�� 
	//��Ҫע�������һ��С���ǣ���ʱ�����������Ҫ�������������ʹ �� connect() ����Զ�̻�������ͨѶ���㲻��Ҫ������ı��ض˿ں�(���� ����ʹ�� telnet ��ʱ��)����ֻҪ�򵥵ĵ��� connect() �Ϳ����ˣ������ ���׽����Ƿ�󶨶˿ڣ����û�У������Լ���һ��û��ʹ�õı��ض� �ڡ�
	int nBind = ::bind(m_sck, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));	//һ��Ҫ�ӡ�::��������bindΪ�������bind,����ֵ�Ͳ���int�����ˣ�����
	if (-1 == nBind)
	{
		cout << "m_sck: " << m_sck << endl;
		cout << "GetLastError(): " << GetLastError() << endl;
		cout << "bind���ɹ�������" << endl;
		return;
	}


	//int WSACleanup(void);
	//Ӧ�ó�������ɶ������Socket���ʹ�ú�Ҫ����WSACleanup�����������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��
	//WSACleanup();

	//����һ���̣߳��������������Ϣ
	std::thread thrdInput(&MyServer::ReceiveInput, this);
	thrdInput.detach();
}

void MyServer::StartServerByName(string strName)
{
	if (m_sck == -1)
	{
		return;
	}

	//���ý��ܺ�����������������ʱ���������ӽ����뵽�ȴ����� (accept()) �Ķ��� �С������ accept() ���������п��е����ӡ���������һ���µ��׽����� ������������������������׽����ˣ�ԭ����һ��������������Ǹ��˿ڣ� �µ���׼������ (send()) �ͽ��� ( recv()) ���ݡ������������̣�
	/*
	int accept(int sockfd, void *addr, int *addrlen);
	sockfd �൱�򵥣��Ǻ� listen() ��һ�����׽�����������addr �Ǹ�ָ ��ֲ������ݽṹ sockaddr_in ��ָ�롣����Ҫ��������Ϣ��Ҫȥ�ĵ� ��������Բⶨ�Ǹ���ַ���Ǹ��˿ں����㣩�������ĵ�ַ���ݸ� accept ֮ ǰ��addrlen �Ǹ��ֲ������α���������Ϊ sizeof(struct sockaddr_in)�� accept �����Ὣ������ֽڸ� addr�������������Щ����ô����ͨ���� �� addrlen ��ֵ��ӳ������
	ͬ�����ڴ���ʱ����-1��������ȫ�ִ������ errno��
	*/
	struct sockaddr_in their_addr; /* connector's address information */	//sin_port�� sin_addr �����������ֽ�˳��(Network Byte Order)��
	their_addr.sin_family = AF_INET;
	char theirAddr[16] = { 0 };
	memcpy(theirAddr, m_mapNametoIp[strName].c_str(), 15 * sizeof(char));

	//��Ϊinet_addr()���������ã�������Ҫ����ת��
	struct in_addr addr;
	inet_pton(AF_INET, theirAddr, (void *)&addr);

	their_addr.sin_addr.s_addr = addr.s_addr;

	//inet_addr�������ص��Ѿ��������ֽڸ�ʽ������
	//�����Խ�һ��in_addr�ṹ������ɵ�����ʽ�������Ļ������Ҫ�õ����� inet_ntoa()("ntoa"�ĺ�����"network to ascii")������������
	//printf("%s", inet_ntoa(ina.sin_addr));
	//�������IP��ַ����Ҫע�����inet_ntoa()���ṹ��in - addr��Ϊһ�����������ǳ����Ρ�ͬ����Ҫע����������ص���һ��ָ��һ���ַ��� ָ�롣����һ����inet_ntoa()���Ƶľ�̬�Ĺ̶���ָ�룬����ÿ�ε��� inet_ntoa()�����ͽ������ϴε���ʱ���õ�IP��ַ��

	memset(&(their_addr.sin_zero), 0, sizeof(8)); /* zero the rest of the struct */

	char IPdotdec[20] = { 0 };
	inet_ntop(AF_INET, &their_addr.sin_addr, IPdotdec, 15);

	//linux inet_ntoa(their_addr.sin_addr)
	//server ������Ϣ�� Ŀ�Ŀͻ���
	//std::thread thrd(&Common::ServerSayHelloToClient, sck_cliend);
	//thrd.detach();	//ִ�����̣߳�����ִ��������߳��Զ��ͷ��ڴ�
	
	SOCKET sck_cliend = -1;

	AddServerToConnectQueueByName(strName);
	bool bAcceptIsSuccess = false;
	bool bIsNeedCreateAcceptThread = true;
	while (true)
	{
		//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), (sockaddr*)(&their_addr), IPdotdec, std::ref(bAcceptIsSuccess)); //C++�е�thread�Ĳ�������ֱ��Ϊ����
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
	//sockfd �ǵ��� socket() ���ص��׽����ļ���������backlog ���ڽ��� �����������������Ŀ��ʲô��˼��? ������������ڶ�����һֱ�ȴ�ֱ ������� (accept() �뿴���������)���ӡ����ǵ���Ŀ�����ڶ��е����� �����ϵͳ��������Ŀ��20����Ҳ��������Ϊ5��10��
	//�ͱ�ĺ���һ�����ڷ��������ʱ�򷵻� - 1��������ȫ�ִ������ errno��

	//��ʼ����
	int nListenOk = listen(m_sck, 10);	//10Ҳ���ǡ����ٵȴ����ӿ��ơ�����˼
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
	m_mapNametoIp[strName] = strIp;  //���ַ�ʽ���룬�������key֮ǰ���Ѵ��ڣ���ֱ���滻value
	//m_mapNametoIp.insert(pair<string, string>(strName, strIp));	//�����滻���������֮ǰ��key�Ѵ��ڣ���˾䲻��ִ�У�������
}

void MyServer::AddServerToConnectQueueByName(string strName)
{
	queue<string> qeMessage;
	m_mapNametoMessageQueue.insert( pair<string, queue<string> >(strName, qeMessage) );	//�����滻���������֮ǰ��key�Ѵ��ڣ���˾䲻��ִ�У�������
}

void MyServer::AcceptFromClient(SOCKET & socket, sockaddr * sckAddr, char* IPdotdec, bool& bIsAcceptSuccess, bool& bIsNeedCreateAcceptThread)
{
	//cout << "��" << IPdotdec << "accept�У�����" << endl;
	if (bIsAcceptSuccess)
	{
		return;
	}

	if (bIsNeedCreateAcceptThread)
	{
		//һ���ͻ��ˣ�ֻ����һ������accpt�̣߳�����
		Common::CreateListenAccepThread(m_sck, sckAddr, socket, IPdotdec, bIsAcceptSuccess);
		bIsNeedCreateAcceptThread = false;
	}
	
	//int sin_size = sizeof(struct sockaddr_in);
	//socket = accept(m_sck, sckAddr, &sin_size);	//accept  �������ģ������Ӧ��IPû�������������ͻ�����һֱ��ס�����벻��ס��������һ���߳�ר������������������
																		//��ϵͳ���� send() �� recv() ����Ӧ��ʹ���µ��׽��������� new_fd�������ֻ����һ�����ӽ�������ô�����ʹ�� close() ȥ�ر�ԭ �����ļ������� m_sck ������ͬһ���˿ڸ��������

}

void MyServer::DoRecvAndSendMessage(SOCKET& sck_cliend, sockaddr* their_addr, bool bAcceptIsSuccess, string strName, char* IPdotdec)
{
	//��ס�� recv��accept���������ģ�����������������������������������������
	int sin_size = sizeof(struct sockaddr_in);
	while (bAcceptIsSuccess)
	{
		if (!m_mapNametoMessageQueue[strName].empty())
		{
			//ȡ��Ҫ���͸�server����Ϣ
			string str = m_mapNametoMessageQueue[strName].front();

			//�Է��͵ı�����Ϣ���з�װ
			int nSize = sizeof(char) * (sizeof(TCPMessage) + nMessageWordMaxSize);
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

			int sendSuccessSize = send(sck_cliend, cSendMessage, nSize, 0);
			free(cSendMessage);
			cSendMessage = NULL;

			if (sendSuccessSize < 0)
			{
				cout << "server: ������Ϣ���ͻ���" << strName.c_str() << "ʧ�ܣ�����������������" << endl;
				bAcceptIsSuccess = false;
				bool bIsNeedCreateAcceptThread = true;
				while (!bAcceptIsSuccess)
				{
					//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), their_addr, IPdotdec, std::ref(bAcceptIsSuccess)); //C++�е�thread�Ĳ�������ֱ��Ϊ����
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
							m_mapNameToSocket[IPdotdec] = sck_cliend;//���ַ�ʽ���룬�������key֮ǰ���Ѵ��ڣ���ֱ���滻value
						}
					}
					
				}

				continue;

			}
			else
			{
				m_mapNametoMessageQueue[strName].pop();
				cout << "�������ݸ�" << strName.c_str() << ": " << str.c_str() << endl;
			}
		}
		else
		{
			//������������
			//�Է��͵ı�����Ϣ���з�װ
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
				cout << "server: ������Ϣ���ͻ���" << strName.c_str() << "ʧ�ܣ�����������������" << endl;
				bAcceptIsSuccess = false;
				bool bIsNeedCreateAcceptThread = true;
				while (!bAcceptIsSuccess)
				{
					//std::thread thrdAccesThread(&MyServer::AcceptFromClient, this, std::ref(sck_cliend), their_addr, IPdotdec, std::ref(bAcceptIsSuccess)); //C++�е�thread�Ĳ�������ֱ��Ϊ����
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
							m_mapNameToSocket[IPdotdec] = sck_cliend;//���ַ�ʽ���룬�������key֮ǰ���Ѵ��ڣ���ֱ���滻value
						}
					}
					
				}

				continue;
			}

		}

		//server ����Ŀ�Ŀͻ��˵���Ϣ
		int nSize = sizeof(char) * (sizeof(TCPMessage));
		char* recvData = (char*)malloc(nSize);
		memset(recvData, 0, nSize);

		int recvClientMessageSize = recv(sck_cliend, recvData, nSize, 0);		//���û���յ��ͻ��˷�����Ϣ���ͻ�һֱ��������
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
				cout << "client��" << tcpMessage->chMessage << endl;
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
		cin.getline(input, nMessageWordMaxSize);	//�ַ����ġ��ո�Ҳ���ȡ����
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
