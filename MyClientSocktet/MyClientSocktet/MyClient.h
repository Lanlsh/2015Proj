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
	char m_cIp[16];	//要连接的Server的IP	

	//在你调用 bind() 的时候，你要小心的另一件事情是：不要采用小于 1024的端口号。所有小于1024的端口号都被系统保留！你可以选择从1024 到65535的端口(如果它们没有被别的程序使用的话)。 
	int m_nPort;	//要连接的Server的端口号

	bool m_bIsServerDisConnect;	//是否为服务器断开

	bool m_bIsLooping;	//	是否一直循环

	//发送消息给服务器的信息队列
	queue<string> m_qeSendMessage;

};

