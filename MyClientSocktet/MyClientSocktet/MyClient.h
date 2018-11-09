#pragma once
#include "stdafx.h"
#include<queue>

using namespace std;

class MyClient
{
public:
	MyClient();
	~MyClient();

	void CreateClient(char* connectIp, int connectPort);

	void StartConnection();

	void ConnectToServer(SOCKET sclient, sockaddr * sckAddr, int nNameLength);

	void ReceiveInput(VOID);

private:
	char m_cIp[16];	//Ҫ���ӵ�Server��IP	

	//������� bind() ��ʱ����ҪС�ĵ���һ�������ǣ���Ҫ����С�� 1024�Ķ˿ںš�����С��1024�Ķ˿ںŶ���ϵͳ�����������ѡ���1024 ��65535�Ķ˿�(�������û�б���ĳ���ʹ�õĻ�)�� 
	int m_nPort;	//Ҫ���ӵ�Server�Ķ˿ں�

	bool m_bIsServerDisConnect;	//�Ƿ�Ϊ�������Ͽ�

	bool m_bIsLooping;	//	�Ƿ�һֱѭ��

	//������Ϣ������������Ϣ����
	queue<string> m_qeSendMessage;

};

