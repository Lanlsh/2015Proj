// MyClientSocktet.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MyClient.h"


int main()
{
	MyClient client;
	client.CreateClient("192.168.15.112", 8888);
	client.StartConnection();

    return 0;
}

