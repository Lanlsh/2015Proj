#pragma once
#include "stdafx.h"
namespace Common
{
	//服务器向刚连接的客户端发送问好消息
	void ServerSayHelloToClient(SOCKET clientSocket)
	{
		//连续发送6条问好语句
		for (int i = 0; i<6; i++)
		{
			if (send(clientSocket, "Server say hello to you!\n", 14, 0) == -1)
			{
				//如何优雅的关闭windows的socket，请看https://blog.csdn.net/qq_16490557/article/details/36172769
				//int closesocket(SOCKET s)的作用是关闭指定的socket，并且回收其所有的资源。
				//int shutdown(SOCKET s, int how)则是禁止在指定的socket s上禁止进行由how指定的操作，但并不对资源进行回收，shutdown之后而closesocket之前s还不能再次connect或者WSAConnect.
				//int WSASendDisconnect(SOCKET s, LPWSABUF lpOutboundDisconnectData)则和shutdown基本类似，稍有不同的就是WSASendDisconnect函数多了一个lpOutboundDisconnectData参数，可以允许发送“断开数据”（disconnect data）.但MSDN上写了“The native implementation of TCP / IP on Windows does not support disconnect data.”，所以一般我们就用shutdown函数就行了。
				closesocket(clientSocket);
				return;
			}

		}

		//shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);
	}

	void SetAccept(const SOCKET serverSocket, sockaddr* their_addr, SOCKET& newSocket, char* IPdotdec, bool& bIsAcceptSuccess)
	{
		int sin_size = sizeof(struct sockaddr_in);
		//accept  是阻塞的，如果对应的IP没有连接上来，就会在这一直卡住！！
		//想不卡住，则启动一个线程专门用来监听！！！！
		//或者当你第一次调用 socket() 建 立套接字描述符的时候，内核就将它设置为阻塞。如果你不想套接字阻塞， 你就要调用函数 fcntl() （但这不是好办法）
		//如果你让你的程序在忙等状 态查询套接字的数据，你将浪费大量的 CPU 时间。更好的解决之道是用 select() 去查询是否有数据要读进来
		//cout << "子线程ID: " << GetCurrentThreadId() << endl;
		newSocket = accept(serverSocket, their_addr, &sin_size);		//此时放在子线程中，在子线程中一直阻塞！！！！！
		
		if (newSocket == -1)
		{
			//cout << "serverSocket: " << serverSocket << endl;
			//cout << "accept失败！！！" << endl;
			bIsAcceptSuccess = false;
			//cout << "离开子线程---子线程ID： " << GetCurrentThreadId() << endl;
			return;
		}

		bIsAcceptSuccess = true;
		printf("server: 连接成功！！ %s\n", IPdotdec);
		cout << "accept    sck_cliend: " << newSocket << endl;
		//cout<<"离开子线程---子线程ID： "<< GetCurrentThreadId() << endl;

	}

	//创建监听accept线程，并执行
	void CreateListenAccepThread(const SOCKET serverSocket, sockaddr * their_addr, SOCKET& newSocket, char* IPdotdec, bool& bIsAcceptSuccess)
	{
		//标准约定std::thread构造时向函数对象传递实际参数的拷贝（支持移动语义），而不是转发实际参数（你可以通过打印实际参数和形式参数的地址来检测这一行为）。即fun的形式参数是被a的拷贝初始化的，所以形式参数a并不是main函数内a的引用，在fun内赋值自然不会改变main函数内a的值。
		//如果你希望“以引用的方式传参”，常用解决方案之一是使用std::ref，即std::thread(fun, std::ref(a)); !!!!!!!!!
		std::thread thrd(&Common::SetAccept, serverSocket, their_addr, std::ref(newSocket), IPdotdec, std::ref(bIsAcceptSuccess));		//C++中的thread的参数不能直接为引用
		thrd.detach();
	}

}