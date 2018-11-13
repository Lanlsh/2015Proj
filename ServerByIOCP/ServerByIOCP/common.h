#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include<memory.h>
#include<windows.h>
#include <MSWSock.h>
#include <MSTcpIP.h>//用于心跳包的头文件
#include <list>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define SRV_IP "192.168.15.112"

#define MAX_BUFFER_LEN					4096		// 江湖规矩一般为8k
#define WORKER_THREADS_PER_PROCESSOR	2
#define MAX_POST_ACCEPT					10			// 同时投递AcceptEx的请求数量
#define EXIT_CODE						NULL			

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

////////////////////////////////////////////////////////////////////
#define	NC_CLIENT_CONNECT		0x0001	//客户端连接
#define	NC_CLIENT_DISCONNECT	0x0002	//客户端断开
#define	NC_TRANSMIT				0x0003	//发送数据
#define	NC_RECEIVE				0x0004	//接收数据
#define NC_RECEIVE_COMPLETE		0x0005	// 完整接收

////////////////////////////////////////////////////////////////////
//在完成端口上投递的I/O操作的类型
typedef enum EM_IOType
{
	EM_IOAccept,						 // 标志投递的 Accept初始化操作
	EM_IOSend,							 // 标志投递的是 发送操作(写)
	EM_IORecv,							 // 标志投递的是 接收操作(读)
	EM_IOIdle					   	     // 用于初始化，无意义
}IOType;


//====================================================================================
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//====================================================================================

//结构体里可以定义一个指针，设置大小，用来传参！！！！
struct PER_IO_CONTEXT
{
	OVERLAPPED		m_overLapped;								//用于IOCP的核心数据结构!!!!!!
	IOType			m_IOType;									//接收到的IO需要处理的类型
	WSABUF			m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	SOCKET			m_rourceSock;								// 接收到的连接的套接字 
	SOCKADDR_IN		m_resourceAddr;										// 发送源套接字地址信息
	char			m_szBuf[MAX_BUFFER_LEN];					//这个是WSABUF里具体存字符的缓冲区
	DWORD			m_dwBytesSend;								// 发送的字节数
	DWORD			m_dwBytesRecv;								// 接收的字节数

	string			m_desAddr;									// 发送到目标的套接字地址信息 //格式为“IP:Port”

	void Init()
	{
		ZeroMemory(&m_overLapped, sizeof(OVERLAPPED));
		ZeroMemory(m_szBuf, MAX_BUFFER_LEN);
		ZeroMemory(&m_resourceAddr, sizeof(SOCKADDR_IN));
		ZeroMemory(&m_resourceAddr.sin_zero, 8);
		m_IOType = EM_IOIdle;
		m_rourceSock = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuf;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_dwBytesSend = 0;
		m_dwBytesRecv = 0;
		m_desAddr.clear();
	}

	void Reset()
	{
		ZeroMemory(&m_overLapped, sizeof(OVERLAPPED));
		ZeroMemory(m_szBuf, MAX_BUFFER_LEN);
		ZeroMemory(&m_resourceAddr, sizeof(SOCKADDR_IN));
		ZeroMemory(&m_resourceAddr.sin_zero, 8);
		m_IOType = EM_IOIdle;
		m_rourceSock = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuf;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_dwBytesSend = 0;
		m_dwBytesRecv = 0;
		m_desAddr.clear();
	}
};

const int const_nMsgSize = sizeof(char) * sizeof(PER_IO_CONTEXT);

// 完成端口传递的参数
struct IOCP_PARAM					 
{
	SOCKET m_rourceSock;
};


/*
描述： 客户端发送消息头部解析结构体
sin_addr: IP地址
sin_port: 端口号
*/
const int nMessageBufMaxSize = MAX_BUFFER_LEN - 22;	//发送消息的文字信息最大的字节数
typedef struct ST_SendToIpInfo
{
	char sin_addr[16];
	char sin_port[6];
	char buf[nMessageBufMaxSize];
}SendToIpInfo;
