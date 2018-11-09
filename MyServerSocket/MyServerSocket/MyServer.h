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

	//创建Server,并绑定IP和端口
	void CreatMyServer(char* ip, int port);

	//启动Server对strName的ip的监听和发送以及接受的连接
	void StartServerByName(string strName);

	//启动Server对所有的已存在的IP进行监听和发送以及接受的连接
	void StartServerAllIp();

	//启动监听
	void StartListen();

	//增加连接着的名字和其IP地址 （若存在，则覆盖IP）
	void AddConnectNameAndIp(string strName, string strIp);

	//增加服务器发送给连接者的消息队列（若存在，则不替换）
	void AddServerToConnectQueueByName(string strName);

	//接受客户端连接
	void AcceptFromClient(SOCKET& socket, sockaddr* sckAddr, char* IPdotdec, bool& bIsAcceptSuccess, bool& bIsNeedCreateAcceptThread);

	//处理与客户端的信息接收和发送
	void DoRecvAndSendMessage(SOCKET& sck_cliend, sockaddr* their_addr, bool bAcceptIsSuccess, string strName, char* IPdotdec);

	void ReceiveInput(VOID);

private:
	SOCKET m_sck;	//socket文件描述符
	char m_cIp[16];	//	ip	

	//在你调用 bind() 的时候，你要小心的另一件事情是：不要采用小于 1024的端口号。所有小于1024的端口号都被系统保留！你可以选择从1024 到65535的端口(如果它们没有被别的程序使用的话)。 
	int m_nPort;	//端口号

	bool m_bIsStartUplisten;	//是否启动监听

	map<string, string> m_mapNametoIp; //保存连接者的名字和其IP地址

	map<string, queue<string> > m_mapNametoMessageQueue;	//保存对回复各个玩家的消息队列

	map<string, SOCKET>	m_mapNameToSocket;	//保存连接者名字对应的socket（与客户端同信的socket）
};

