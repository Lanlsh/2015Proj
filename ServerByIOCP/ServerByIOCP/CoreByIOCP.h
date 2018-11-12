#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "LanCriticalLock.h"
#include "LanEvent.h"

typedef void (CALLBACK* NOTIFYPROC)(LPVOID, PER_IO_CONTEXT*, UINT);

typedef std::list<PER_IO_CONTEXT*> IOContextList;
typedef std::list<IOCP_PARAM*>	IocpParamList;

class CCoreByIOCP
{
public:
	CCoreByIOCP();
	~CCoreByIOCP();

	//启动core层
	bool RunCore(NOTIFYPROC pNotifyProc, const UINT& nPort);

	//关闭core层
	void StopCore();

	//获取完成端口句柄
	HANDLE	GetIOCPPort() { return m_hIOCompletionPort; }

	//内存池的实现
	VOID MoveToFreeParamPool(IOCP_PARAM* pIocpParam);		// 将移除的完成端口参数添加到内存池
	VOID MoveToFreePool(PER_IO_CONTEXT* pIoContext);		// 将移除的客户端套接字添加到内存池
	IOCP_PARAM* AllocateIocpParam();						// 分配完成端口的结构体

	// 资源的释放
	VOID RemoveStaleClient(PER_IO_CONTEXT* pIoContext, BOOL bGraceful);// 溢出客户端套接字
	VOID ReleaseResource();									// 释放资源

	bool PostSend(PER_IO_CONTEXT* pIoContext);				// 投递一个发送操作
	PER_IO_CONTEXT* AllocateClientIOContext();				// 从内存池中分配一个socket结构体

	// 消息映射
	BEGIN_IO_MSG_MAP()
		IO_MESSAGE_HANDLER(EM_IORecv, OnClientReading)
		IO_MESSAGE_HANDLER(EM_IOSend, OnClientWriting)
		IO_MESSAGE_HANDLER(EM_IOAccept, OnClientAccept)
	END_IO_MSG_MAP()

	bool OnClientAccept(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);
	bool OnClientReading(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);
	bool OnClientWriting(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);

protected:
	bool InitNetEnvironment();								// 初始化网络环境
	bool InitializeIOCP();									// 初始化完成端口
	bool InitializeListenSocket();							// 初始化监听套接字

	// 投递操作
	bool PostAcceptEx(PER_IO_CONTEXT* pAcceptIoContext);	// 投递一个Accept请求
	bool PostRecv(PER_IO_CONTEXT* pIoContext);				// 投递一个接收的IO操作

	bool OnAccept(PER_IO_CONTEXT* pIoContext);
	// 将 监听套接字 绑定到完成端口中
	bool AssociateSocketWithCompletionPort(SOCKET socket, DWORD dwCompletionKey);

private:
	NOTIFYPROC m_pNotifyProc;					// 消息回调函数
	CRITICAL_SECTION m_cs;						// 关键段（临界资源）

	HANDLE m_hShutDownEvent;					// 系统退出事件通知
	HANDLE m_hIOCompletionPort;					// 完成端口句柄
	//UINT m_nThreadCnt;						    // 线程数量
	LanThreadPool* m_pWorkThreads;				// 工作者线程者指针

	PER_IO_CONTEXT* m_pListenContext;		    // 用于监听的Socket的Context信息
	SOCKET m_socListen;							// 监听套接字
	UINT m_nPort;								// 监听的端口

	IOContextList m_listAcceptExSock;			// AcceptEx 预创建的套接字，便于管理

	IOContextList m_listIoContext;				// 已经连接的客户端
	IOContextList m_listFreeIoContext;			// 断开的客户端的链表	共同实现一个内存池
	IocpParamList m_listIocpParam;				// 已经使用的参数链表
	IocpParamList m_listFreeIocpParam;			// 空闲的完成端口参数链表

	IOCP_PARAM* m_pListenIocpParam;				// 监听套接字的完成端口参数

	int m_nKeepLiveTime;						// 心跳包探测间隔时间

	LPFN_ACCEPTEX  m_lpfnAcceptEx;              // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_GETACCEPTEXSOCKADDRS   m_lpfnGetAcceptExSockAddrs;
};

