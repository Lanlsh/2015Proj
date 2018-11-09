#pragma once
#include "stdafx.h"
#include <map>
#include <thread>
#include<queue>

using namespace std;

class MyServer
{
public:
	MyServer();
	~MyServer();

	//����Server,����IP�Ͷ˿�
	void CreatMyServer(char* ip, int port);

	//����Server��strName��ip�ļ����ͷ����Լ����ܵ�����
	void StartServerByName(string strName);

	//����Server�����е��Ѵ��ڵ�IP���м����ͷ����Լ����ܵ�����
	void StartServerAllIp();

	//��������
	void StartListen();

	//���������ŵ����ֺ���IP��ַ �������ڣ��򸲸�IP��
	void AddConnectNameAndIp(string strName, string strIp);

	//���ӷ��������͸������ߵ���Ϣ���У������ڣ����滻��
	void AddServerToConnectQueueByName(string strName);

	//���ܿͻ�������
	void AcceptFromClient(SOCKET& socket, sockaddr* sckAddr, char* IPdotdec, bool& bIsAcceptSuccess, bool& bIsNeedCreateAcceptThread);

	//������ͻ��˵���Ϣ���պͷ���
	void DoRecvAndSendMessage(SOCKET& sck_cliend, sockaddr* their_addr, bool bAcceptIsSuccess, string strName, char* IPdotdec);

	void ReceiveInput(VOID);

private:
	SOCKET m_sck;	//socket�ļ�������
	char m_cIp[16];	//	ip	

	//������� bind() ��ʱ����ҪС�ĵ���һ�������ǣ���Ҫ����С�� 1024�Ķ˿ںš�����С��1024�Ķ˿ںŶ���ϵͳ�����������ѡ���1024 ��65535�Ķ˿�(�������û�б���ĳ���ʹ�õĻ�)�� 
	int m_nPort;	//�˿ں�

	bool m_bIsStartUplisten;	//�Ƿ���������

	map<string, string> m_mapNametoIp; //���������ߵ����ֺ���IP��ַ

	map<string, queue<string> > m_mapNametoMessageQueue;	//����Իظ�������ҵ���Ϣ����

	map<string, SOCKET>	m_mapNameToSocket;	//�������������ֶ�Ӧ��socket����ͻ���ͬ�ŵ�socket��
};

