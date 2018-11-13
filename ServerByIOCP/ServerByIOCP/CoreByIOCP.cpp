#include "stdafx.h"
#include "CoreByIOCP.h"
#include "LanThreadPool.h"

CCoreByIOCP::CCoreByIOCP()
{
	m_hShutDownEvent = NULL;
	m_hIOCompletionPort = NULL;
	//m_nThreadCnt = 0;
	m_pWorkThreads = NULL;
	m_nPort = 0;
	m_lpfnAcceptEx = NULL;
	m_pListenContext = new PER_IO_CONTEXT();
	m_nKeepLiveTime = 1000 * 60 * 3; // 三分钟探测一次
	m_pListenIocpParam = new IOCP_PARAM;
	m_mapClient.clear();

	//重置此标志
	m_bIsExecStopCore = false;
}


CCoreByIOCP::~CCoreByIOCP()
{
	if (!m_bIsExecStopCore)
	{
		StopCore();
	}
}

bool CCoreByIOCP::RunCore(NOTIFYPROC pNotifyProc, const UINT& nPort)
{
	m_nPort = nPort;
	m_pNotifyProc = pNotifyProc;

	//重置此标志
	m_bIsExecStopCore = false;

	//初始化临界区对象
	InitializeCriticalSection(&m_cs);

	bool bRet = false;
	do
	{
		if (NULL == (m_hShutDownEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
			break;
		if (!InitNetEnvironment())
			break;
		if (!InitializeIOCP())
			break;
		if (!InitializeListenSocket())
			break;
		bRet = true;
	} while (FALSE);

	if (!bRet)
	{
		TCHAR szErr[32];
		cout << "启动服务器失败!!!!!!!!" << endl;
		::MessageBox(GetDesktopWindow(), szErr, L"启动服务器失败", MB_OK | MB_ICONHAND);
	}

	return bRet;
}

void CCoreByIOCP::StopCore()
{
	//重置标志
	m_bIsExecStopCore = true;

	if (m_socListen != INVALID_SOCKET)
	{
		SetEvent(m_hShutDownEvent);

		if (m_pWorkThreads)
		{
			for (unsigned int i = 0; i <m_pWorkThreads->GetCurrentThreadCount(); i++)
			{
				//PostQueuedCompletionStatus函数，向每个工作者线程都发送—个特殊的完成数据包。该函数会指示每个线程都“立即结束并退出”
				/*
				通过为线程池中的每个线程都调用一次PostQueuedCompletionStatus，我们可以将它们都唤醒。
				每个线程会对GetQueuedCompletionStatus的返回值进行检查，如果发现应用程序正在终止，那么它们就可以进行清理工作并正常地退出。
				"也就是说调用此句会唤醒相应的线程，从而不需要调用额外语句去唤醒相应线程！！！！"
				*/
				PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
			}
		}

		ReleaseResource();
	}

}

bool CCoreByIOCP::InitNetEnvironment()
{
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
		return false;

	return true;
}

bool CCoreByIOCP::InitializeIOCP()
{
	SYSTEM_INFO systemInfo;

	// 创建完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIOCompletionPort)
		return false;

	GetSystemInfo(&systemInfo);
	// 线程数的限制，防止线程上下文切换浪费资源
	int m_nThreadCnt = WORKER_THREADS_PER_PROCESSOR * systemInfo.dwNumberOfProcessors;// 核心数的两倍

	m_pWorkThreads = LanThreadPool::GetLanThreadPoolInstance();
	m_pWorkThreads->SetThreadPool(m_nThreadCnt, m_nThreadCnt);
	m_pWorkThreads->SetIOCP(this);
	m_pWorkThreads->Run();

	return true;
}

bool CCoreByIOCP::InitializeListenSocket()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// 需要使用重叠IO，必须使用WSASocket来建立Socket，才可以支持重叠IO操作
	m_socListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_socListen)
		return false;

	m_pListenIocpParam->m_rourceSock = m_socListen;
	// 将 监听套接字 绑定到完成端口中
	if (!AssociateSocketWithCompletionPort(m_socListen, (DWORD)(m_pListenIocpParam->m_rourceSock)))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// 绑定
	SOCKADDR_IN servAddr;
	ZeroMemory(&servAddr, sizeof(SOCKADDR_IN));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(m_nPort);
	if (SOCKET_ERROR == ::bind(m_socListen, (struct sockaddr*)&servAddr, sizeof(servAddr)))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// 开始监听
	if (SOCKET_ERROR == listen(m_socListen, SOMAXCONN))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下AcceptEx函数的指针
	DWORD dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(
		m_socListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
		&dwBytes, NULL, NULL))
	{
		this->ReleaseResource();
		return false;
	}

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if (SOCKET_ERROR == WSAIoctl(
		m_socListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs),
		&m_lpfnGetAcceptExSockAddrs,
		sizeof(m_lpfnGetAcceptExSockAddrs),
		&dwBytes,
		NULL,
		NULL))
	{
		this->ReleaseResource();
		return false;
	}

	// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
	for (int i = 0; i < MAX_POST_ACCEPT; i++)
	{
		PER_IO_CONTEXT* pAcceptIoContext = new PER_IO_CONTEXT();
		pAcceptIoContext->Init();
		if (FALSE == PostAcceptEx(pAcceptIoContext))
		{
			this->RemoveStaleClient(pAcceptIoContext, TRUE);
			this->ReleaseResource();
			return false;
		}
		m_listAcceptExSock.push_back(pAcceptIoContext);
	}

	return true;
}

VOID CCoreByIOCP::MoveToFreeParamPool(IOCP_PARAM* pIocpParam)
{
	LanCriticalLock cs(m_cs, "MoveToFreeParamPool");

	IocpParamList::iterator iter;
	iter = find(m_listIocpParam.begin(), m_listIocpParam.end(), pIocpParam);
	if (iter != m_listIocpParam.end())
	{
		m_listFreeIocpParam.push_back(pIocpParam);
		m_listIocpParam.remove(pIocpParam);
	}
}

VOID CCoreByIOCP::MoveToFreePool(PER_IO_CONTEXT* pIoContext)
{
	LanCriticalLock cs(m_cs, "MoveToFreePool");

	IOContextList::iterator iter;
	iter = find(m_listIoContext.begin(), m_listIoContext.end(), pIoContext);

	if (iter != m_listIoContext.end())
	{
		m_listFreeIoContext.push_back(pIoContext);// 不释放已经创建的，加入到内存池中，下次有新客户端连接就不用再创建
		m_listIoContext.remove(pIoContext);
	}
}

IOCP_PARAM* CCoreByIOCP::AllocateIocpParam()
{
	LanCriticalLock cs(m_cs, "AllocateIocpParam");

	IOCP_PARAM* pIocpParam = NULL;
	if (!m_listFreeIocpParam.empty())
	{
		pIocpParam = m_listFreeIocpParam.front();
		m_listFreeIocpParam.remove(pIocpParam);
	}
	else
	{
		pIocpParam = new IOCP_PARAM;
	}

	m_listIocpParam.push_back(pIocpParam);
	if (pIocpParam != NULL)
	{
		pIocpParam->m_rourceSock = INVALID_SOCKET;
	}
	return pIocpParam;
}

VOID CCoreByIOCP::RemoveStaleClient(PER_IO_CONTEXT* pIoContext, BOOL bGraceful/*是否中止连接*/)
{
	LanCriticalLock cs(m_cs, "RemoveStaleClient");

	LINGER lingerStruct;

	// 如果我们要中止连接，设置延时值为 0 (优雅的关闭连接)
	if (!bGraceful)
	{
		//.l_onoff=1;（在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
		// 如果.l_onoff=0;则功能和2.)作用相同;
		lingerStruct.l_onoff = 1;	//开关，零或者非零 
		lingerStruct.l_linger = 0;  //优雅关闭最长时限（允许逗留的时限 秒）
		setsockopt(pIoContext->m_rourceSock, SOL_SOCKET, SO_LINGER,
			(char*)&lingerStruct, sizeof(lingerStruct));
	}


	//将该客户端的信息清除
	RomoveClientInfo(pIoContext->m_rourceSock);

	// 释放 PER_SOCKET_CONTEXT 中的数据
	std::list<PER_IO_CONTEXT*>::iterator iter;
	iter = find(m_listIoContext.begin(), m_listIoContext.end(), pIoContext);
	if (iter != m_listIoContext.end())
	{
		// CancelIo 取消挂起的IO操作 优雅的关闭这个套接字句柄
		CancelIo((HANDLE)pIoContext->m_rourceSock);

		closesocket(pIoContext->m_rourceSock);
		pIoContext->m_rourceSock = INVALID_SOCKET;

		// 判断是否还在进行IO操作 等待上一个IO操作完成再关闭
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)pIoContext))
			Sleep(0);

		// 回调函数，发送退出消息
		m_pNotifyProc(NULL, pIoContext, NC_CLIENT_DISCONNECT);

		MoveToFreePool(pIoContext);
	}
}

VOID CCoreByIOCP::ReleaseResource()
{
	// 删除关键段
	DeleteCriticalSection(&m_cs);

	// 释放 系统退出事件句柄
	RELEASE_HANDLE(m_hShutDownEvent);

	// 释放工作者线程句柄指针
	if (m_pWorkThreads)
	{
		m_pWorkThreads->FreeMommery();
	}

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);

	// 关闭监听套接字
	RELEASE_SOCKET(m_socListen);


	// 删除监听套接字的完成端口参数
	delete m_pListenIocpParam;

	IOContextList::iterator iter;

	// 清理空闲的套接字
	iter = m_listFreeIoContext.begin();
	while (iter != m_listFreeIoContext.end())
	{
		delete *iter;
		++iter;
	}
	m_listFreeIoContext.clear();

	// 清理连接的套接字
	iter = m_listIoContext.begin();
	while (iter != m_listIoContext.end())
	{
		closesocket((*iter)->m_rourceSock);
		delete *iter;
		++iter;
	}
	m_listIoContext.clear();

	// 清理预创建的套接字
	iter = m_listAcceptExSock.begin();
	while (iter != m_listAcceptExSock.end())
	{
		delete *iter;
		++iter;
	}

	//清空客户端IP信息
	m_mapClient.clear();
}

// 接收到客户端的连接
bool CCoreByIOCP::OnAccept(PER_IO_CONTEXT* pIoContext)
{
	SOCKADDR_IN* RemoteSockAddr = NULL;
	SOCKADDR_IN* LocalSockAddr = NULL;
	int nLen = sizeof(SOCKADDR_IN);

	///////////////////////////////////////////////////////////////////////////
	// 1. m_lpfnGetAcceptExSockAddrs 取得客户端和本地端的地址信息 与 客户端发来的第一组数据
	// 如果客户端只是连接了而不发消息，是不会接收到的
	this->m_lpfnGetAcceptExSockAddrs(
		pIoContext->m_wsaBuf.buf,						// 第一条信息
		pIoContext->m_wsaBuf.len - ((nLen + 16) * 2),
		nLen + 16, nLen + 16,
		(sockaddr**)&LocalSockAddr, &nLen,				// 本地信息
		(sockaddr**)&RemoteSockAddr, &nLen);			// 客户端信息

	PER_IO_CONTEXT* pNewIoContext = AllocateClientIOContext();
	pNewIoContext->m_rourceSock = pIoContext->m_rourceSock;
	pNewIoContext->m_resourceAddr = *RemoteSockAddr;

	// 处理消息，此处为连接上，第一次接受到客户端的数据
	m_pNotifyProc(NULL, pIoContext, NC_CLIENT_CONNECT);

	IOCP_PARAM* pIocpParam = AllocateIocpParam();
	pIocpParam->m_rourceSock = pNewIoContext->m_rourceSock;
	// 将新连接的客户端的socket，绑定到完成端口
	if (!AssociateSocketWithCompletionPort(pNewIoContext->m_rourceSock, (DWORD)pIocpParam->m_rourceSock))
	{
		closesocket(m_socListen);
		closesocket(pNewIoContext->m_rourceSock);

		delete pNewIoContext;
		delete pIocpParam;
		pNewIoContext = NULL;
		pIocpParam = NULL;
		return false;
	}

	SendToIpInfo ipInfo;
	ZeroMemory(&ipInfo, MAX_BUFFER_LEN);
	memcpy(&ipInfo, pIoContext->m_szBuf, MAX_BUFFER_LEN);
	string strIP = ipInfo.sin_IP;
	string strPort = ipInfo.sin_port;
	string str = strIP + ":" + strPort;
	//增加客户端信息
	AddCLientInfo(pNewIoContext->m_rourceSock, str);

	// Set KeepAlive 设置心跳包，开启保活机制，用于保证TCP的长连接（它会在底层发一些数据，不会传到应用层）
	unsigned long chOpt = 1;
	if (SOCKET_ERROR == setsockopt(pNewIoContext->m_rourceSock, SOL_SOCKET, SO_KEEPALIVE, (char*)&chOpt, sizeof(char)))
	{
		// 心跳激活失败
		MoveToFreeParamPool(pIocpParam);
		RemoveStaleClient(pNewIoContext, TRUE);
		return false;
	}

	// 设置超时详细信息
	tcp_keepalive	klive;
	klive.onoff = 1; // 启用保活
	klive.keepalivetime = m_nKeepLiveTime;
	klive.keepaliveinterval = 1000 * 10; // 重试间隔为10秒 Resend if No-Reply
	WSAIoctl
	(
		pNewIoContext->m_rourceSock,
		SIO_KEEPALIVE_VALS,
		&klive,
		sizeof(tcp_keepalive),
		NULL,
		0,
		(unsigned long *)&chOpt,
		0,
		NULL
	);

	// 给新连接的套接字投递接收操作
	if (!PostRecv(pNewIoContext))
	{
		MoveToFreeParamPool(pIocpParam);
	}

	LanCriticalLock cs(m_cs, "OnAccept");

	pIoContext->Reset();		// 再次初始化，便于再次利用
	return PostAcceptEx(pIoContext);
}



bool CCoreByIOCP::OnClientAccept(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	bool bRet = false;
	try
	{
		pIOContext->m_dwBytesRecv = dwSize;
		// ... 处理一些接收到的操作
		bRet = OnAccept(pIOContext);
	}
	catch (...) {}

	return bRet;
}

bool CCoreByIOCP::OnClientReading(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	LanCriticalLock cs(m_cs, "OnClientReading");
	bool bRet = false;
	try
	{
		// 处理接收到的数据
		m_pNotifyProc(NULL, pIOContext, NC_RECEIVE);

		// 再投递一个异步接收消息
		bRet = PostRecv(pIOContext);
	}
	catch (...) {}

	return bRet;
}

bool CCoreByIOCP::PostAcceptEx(PER_IO_CONTEXT * pAcceptIoContext)
{
	pAcceptIoContext->m_IOType = EM_IOAccept;// 初始化 IO类型 为接收套接字

										  // 为以后新连入的客户端准备好Socket（这是与传统Accept最大的区别）
										  // 实际上市创建一个 网络连接池 ，类似于 内存池，我们先创建一定数量的socket，然后直接使用就是了
	pAcceptIoContext->m_rourceSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pAcceptIoContext->m_rourceSock)
		return false;

	// 投递异步 AcceptEx
	if (FALSE == m_lpfnAcceptEx(
		m_socListen,					//侦听套接字。服务器应用程序在这个套接字上等待连接
		pAcceptIoContext->m_rourceSock,		//将用于连接的套接字。此套接字必须不能已经绑定或者已经连接。
		pAcceptIoContext->m_wsaBuf.buf,	//指向一个缓冲区，该缓冲区用于接收新建连接的所发送数据的第一个块、该服务器的本地地址和客户端的远程地址。接收到的数据将被写入到缓冲区0偏移处，而地址随后写入。 该参数必须指定，如果此参数设置为NULL，将不会得到执行，也无法通过GetAcceptExSockaddrs函数获得本地或远程的地址
		pAcceptIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),	//这一大小应不包括服务器的本地地址的大小或客户端的远程地址，他们被追加到输出缓冲区。如果dwReceiveDataLength是零，AcceptEx将不等待接收任何数据，而是尽快建立连接。
		sizeof(SOCKADDR_IN) + 16,	//为本地地址信息保留的字节数。此值必须比所用传输协议的最大地址大小长16个字节
		sizeof(SOCKADDR_IN) + 16,	//为远程地址的信息保留的字节数。此值必须比所用传输协议的最大地址大小长16个字节。 该值不能为0。
		&(pAcceptIoContext->m_dwBytesRecv),	//指向一个DWORD用于标识接收到的字节数。此参数只有在同步模式下有意义。如果函数返回ERROR_IO_PENDING并在迟些时候完成操作，那么这个DWORD没有意义，这时你必须获得从完成通知机制中读取操作字节数
		&(pAcceptIoContext->m_overLapped))	//一个OVERLAPPED结构，用于处理请求。此参数必须指定，它不能为空
		)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
			return false;
	}

	return true;
}

bool CCoreByIOCP::PostRecv(PER_IO_CONTEXT* pIoContext)
{
	// 清空缓冲区，再次投递
	ZeroMemory(pIoContext->m_szBuf, MAX_BUFFER_LEN);
	ZeroMemory(&pIoContext->m_overLapped, sizeof(OVERLAPPED));
	pIoContext->m_IOType = EM_IORecv;
	DWORD dwNumBytesOfRecvd;

	ULONG ulFlags = MSG_PARTIAL;
	UINT nRet = WSARecv(
		pIoContext->m_rourceSock,
		&(pIoContext->m_wsaBuf),
		1,
		&dwNumBytesOfRecvd,// 接收的字节数，异步操作的返回结果一般为0，具体接收到的字节数在完成端口获得
		&(ulFlags),
		&(pIoContext->m_overLapped),
		NULL);
	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		RemoveStaleClient(pIoContext, FALSE);
		return false;
	}
	return true;
}


bool CCoreByIOCP::OnClientWriting(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	bool bRet = false;
	try
	{
		// 异步发送的返回的传输成功的结果是否 少于 要求发送的数据大小（未发送完成），此时重发
		// 最好使用CRC校验之内的，更加严谨性（可以在结构体中放一个计算的CRC值），但是计算会更消耗性能
		if (dwSize != pIOContext->m_dwBytesSend)
		{
			bRet = PostSend(pIOContext);
		}
		else// 已经发送成功，将结构体放回内存池
		{
			m_pNotifyProc(NULL, pIOContext, NC_TRANSMIT);
			MoveToFreePool(pIOContext);
		}
	}
	catch (...) {}

	return bRet;
}



bool CCoreByIOCP::PostSend(PER_IO_CONTEXT* pIoContext)
{
	pIoContext->m_wsaBuf.buf = pIoContext->m_szBuf;
	pIoContext->m_wsaBuf.len = static_cast<ULONG>(strlen(pIoContext->m_szBuf));
	pIoContext->m_IOType = EM_IOSend;
	ULONG ulFlags = MSG_PARTIAL;

	SOCKET	socket;
	GetClientSOCKET(pIoContext->m_desAddr, socket);
	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	INT nRet = WSASend(
		pIoContext->m_rourceSock,
		&pIoContext->m_wsaBuf,
		1,
		&(pIoContext->m_wsaBuf.len),
		ulFlags,
		&(pIoContext->m_overLapped),
		NULL);

	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		RemoveStaleClient(pIoContext, FALSE);
		return false;
	}

	return true;
}

bool CCoreByIOCP::AssociateSocketWithCompletionPort(SOCKET socket, DWORD dwCompletionKey)
{
	// 绑定套接字绑定到完成端口
	// 第二个参数为完成端口句柄时，返回值为完成端口。为空时，返回新的完成端口句柄
	HANDLE hTmp = CreateIoCompletionPort((HANDLE)socket, m_hIOCompletionPort, dwCompletionKey, 0);
	return hTmp == m_hIOCompletionPort;
}

void CCoreByIOCP::AddCLientInfo(SOCKET socket, string str)
{
	m_mapClient[socket] = str;
}

void CCoreByIOCP::RomoveClientInfo(SOCKET socket)
{
	m_mapClient.erase(socket);
}

void CCoreByIOCP::GetClientSOCKET(string str, SOCKET & socket)
{
	std::map<SOCKET, string>::iterator iter = m_mapClient.begin();
	for (; iter!=m_mapClient.end(); iter++)
	{
		if (strcmp((iter->second).c_str(), str.c_str()))
		{
			socket =  iter->first;
			return;
		}
	}

	socket = INVALID_SOCKET;
}

PER_IO_CONTEXT* CCoreByIOCP::AllocateClientIOContext()
{
	LanCriticalLock cs(this->m_cs, "AllocateSocketContext");

	PER_IO_CONTEXT* pIoContext = NULL;
	if (!m_listFreeIoContext.empty())
	{
		pIoContext = m_listFreeIoContext.front();
		m_listFreeIoContext.remove(pIoContext);
	}
	else
	{
		pIoContext = new PER_IO_CONTEXT();
	}

	m_listIoContext.push_back(pIoContext);
	if (pIoContext != NULL)
	{
		pIoContext->Init();
	}

	return pIoContext;
}