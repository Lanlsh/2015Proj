#include "IOCPServer.h"
#include <process.h>
#include <stdio.h>
#include <tchar.h>
#include "Lock.h"

CRITICAL_SECTION CIOCPServer::m_cs;

CIOCPServer::CIOCPServer()
{
	m_hShutDownEvent = NULL;
	m_hIOCompletionPort = NULL;
	m_nThreadCnt = 0;
	m_pWorkThreads = NULL;
	m_nPort = 0;
	m_lpfnAcceptEx = NULL;
	m_pListenContext = new PER_IO_CONTEXT;
	m_nKeepLiveTime = 1000 * 60 * 3; // ������̽��һ��
	m_pListenIocpParam = new IOCP_PARAM;
}


CIOCPServer::~CIOCPServer()
{
}

bool CIOCPServer::StartIOCP(NOTIFYPROC pNotifyProc, const UINT& nPort)
{
	m_nPort = nPort;
	m_pNotifyProc = pNotifyProc;
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
		_stprintf(szErr, _T("Error code:%d"), GetLastError());
		::MessageBox(GetDesktopWindow(), szErr, L"����������ʧ��", MB_OK | MB_ICONHAND);
	}
	
	return bRet;
}

bool CIOCPServer::Stop()
{
	if (m_socListen != INVALID_SOCKET)
	{
		SetEvent(m_hShutDownEvent);

		for (unsigned int i = 0; i < m_nThreadCnt; i++)
		{
			//PostQueuedCompletionStatus��������ÿ���������̶߳����͡��������������ݰ����ú�����ָʾÿ���̶߳��������������˳���
			/*
			ͨ��Ϊ�̳߳��е�ÿ���̶߳�����һ��PostQueuedCompletionStatus�����ǿ��Խ����Ƕ����ѡ�
			ÿ���̻߳��GetQueuedCompletionStatus�ķ���ֵ���м�飬�������Ӧ�ó���������ֹ����ô���ǾͿ��Խ������������������˳���
			"Ҳ����˵���ô˾�ỽ����Ӧ���̣߳��Ӷ�����Ҫ���ö������ȥ������Ӧ�̣߳�������"
			*/
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}

		WaitForMultipleObjects(m_nThreadCnt, m_pWorkThreads, TRUE, INFINITE);

		ReleaseResource();
	}
	return true;
}

bool CIOCPServer::InitNetEnvironment()
{
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2,2),&wsaData))
		return false;
	return true;
}

bool CIOCPServer::InitializeIOCP()
{
	SYSTEM_INFO systemInfo;
	UINT nThreadID;

	// ������ɶ˿�
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIOCompletionPort)
		return false;
	
	GetSystemInfo(&systemInfo);
	// �߳��������ƣ���ֹ�߳��������л��˷���Դ
	m_nThreadCnt = WORKER_THREADS_PER_PROCESSOR * systemInfo.dwNumberOfProcessors + 2;// ������������+2

	m_pWorkThreads = new HANDLE[m_nThreadCnt];
	for (unsigned int i = 0; i < m_nThreadCnt; i++)
	{
		m_pWorkThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadPoolFunc, (void*)this, 0, &nThreadID);
		if (NULL == m_pWorkThreads[i])
		{
			CloseHandle(m_hIOCompletionPort);
			return false;
		}
	}

	return true;
}

bool CIOCPServer::InitializeListenSocket()
{
	// AcceptEx �� GetAcceptExSockaddrs ��GUID�����ڵ�������ָ��
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// ��Ҫʹ���ص�IO������ʹ��WSASocket������Socket���ſ���֧���ص�IO����
	m_socListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_socListen)
		return false;

	m_pListenIocpParam->m_sock = m_socListen;
	// �� �����׽��� �󶨵���ɶ˿���
	if (!AssociateSocketWithCompletionPort(m_socListen,(DWORD)m_pListenIocpParam))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// ��
	SOCKADDR_IN servAddr;
	ZeroMemory(&servAddr, sizeof(SOCKADDR_IN));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(m_nPort);
	if (SOCKET_ERROR == bind(m_socListen,(struct sockaddr*)&servAddr,sizeof(servAddr)))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// ��ʼ����
	if (SOCKET_ERROR == listen(m_socListen,SOMAXCONN))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// ʹ��AcceptEx��������Ϊ���������WinSock2�淶֮���΢�������ṩ����չ����
	// ������Ҫ�����ȡһ��AcceptEx������ָ��
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

	// ��ȡGetAcceptExSockAddrs����ָ�룬Ҳ��ͬ��
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

	// ΪAcceptEx ׼��������Ȼ��Ͷ��AcceptEx I/O����
	for (int i = 0; i < MAX_POST_ACCEPT; i++)
	{
		PER_IO_CONTEXT* pAcceptIoContext = new PER_IO_CONTEXT;
		pAcceptIoContext->Clear();
		if (FALSE == PostAcceptEx(pAcceptIoContext))
		{
			this->RemoveStaleClient(pAcceptIoContext,TRUE);
			this->ReleaseResource();
			return false;
		}
		m_listAcceptExSock.push_back(pAcceptIoContext);
	}

	return true;
}


bool CIOCPServer::PostAcceptEx(PER_IO_CONTEXT* pAcceptIoContext)
{
	pAcceptIoContext->m_ioType = IOAccept;// ��ʼ�� IO���� Ϊ�����׽���

	// Ϊ�Ժ�������Ŀͻ���׼����Socket�������봫ͳAccept��������
	// ʵ�����д���һ�� �������ӳ� �������� �ڴ�أ������ȴ���һ��������socket��Ȼ��ֱ��ʹ�þ�����
	pAcceptIoContext->m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pAcceptIoContext->m_sock)
		return false;

	// Ͷ���첽 AcceptEx
	if (FALSE == m_lpfnAcceptEx(
		m_socListen,					//�����׽��֡�������Ӧ�ó���������׽����ϵȴ�����
		pAcceptIoContext->m_sock,		//���������ӵ��׽��֡����׽��ֱ��벻���Ѿ��󶨻����Ѿ����ӡ�
		pAcceptIoContext->m_wsaBuf.buf,	//ָ��һ�����������û��������ڽ����½����ӵ����������ݵĵ�һ���顢�÷������ı��ص�ַ�Ϳͻ��˵�Զ�̵�ַ�����յ������ݽ���д�뵽������0ƫ�ƴ�������ַ���д�롣 �ò�������ָ��������˲�������ΪNULL��������õ�ִ�У�Ҳ�޷�ͨ��GetAcceptExSockaddrs������ñ��ػ�Զ�̵ĵ�ַ
		pAcceptIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),	//��һ��СӦ�������������ı��ص�ַ�Ĵ�С��ͻ��˵�Զ�̵�ַ�����Ǳ�׷�ӵ���������������dwReceiveDataLength���㣬AcceptEx�����ȴ������κ����ݣ����Ǿ��콨�����ӡ�
		sizeof(SOCKADDR_IN) + 16,	//Ϊ���ص�ַ��Ϣ�������ֽ�������ֵ��������ô���Э�������ַ��С��16���ֽ�
		sizeof(SOCKADDR_IN) + 16,	//ΪԶ�̵�ַ����Ϣ�������ֽ�������ֵ��������ô���Э�������ַ��С��16���ֽڡ� ��ֵ����Ϊ0��
		&(pAcceptIoContext->m_dwBytesRecv),	//ָ��һ��DWORD���ڱ�ʶ���յ����ֽ������˲���ֻ����ͬ��ģʽ�������塣�����������ERROR_IO_PENDING���ڳ�Щʱ����ɲ�������ô���DWORDû�����壬��ʱ������ô����֪ͨ�����ж�ȡ�����ֽ���
		&(pAcceptIoContext->m_ol))	//һ��OVERLAPPED�ṹ�����ڴ������󡣴˲�������ָ����������Ϊ��
		)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
			return false;
	}

	return true;
}

unsigned __stdcall CIOCPServer::ThreadPoolFunc(LPVOID lpParam)
{
	CIOCPServer* pThis = (CIOCPServer*)lpParam;
	OVERLAPPED*	pOverlapped = NULL;
	DWORD dwIoSize = 0;
	BOOL bRet = FALSE;
	DWORD dwErr = 0;
	IOCP_PARAM* pIocpParam = NULL;
	PER_IO_CONTEXT* pIoContext = NULL;

	// ѭ������ֱ�� �˳��¼� ���źŵ������ͽ����˳�
	while (WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hShutDownEvent, 10))
	{
		bRet = GetQueuedCompletionStatus(//  ����ɶ˿��л�ȡ��Ϣ
			pThis->m_hIOCompletionPort,
			&dwIoSize,
			(PULONG_PTR)&pIocpParam,
			&pOverlapped,
			INFINITE);

		if (EXIT_CODE == pIocpParam)
			break;

		pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_ol);

		if (!bRet)// ���������Ϣ
		{
			dwErr = GetLastError();
			if (WAIT_TIMEOUT == dwErr)// ��ʱ
			{
				// ��ʱ��ͨ������һ����Ϣ���ж��Ƿ���ߣ�������socket��Ͷ��WSARecv�����
				// ��Ϊ����ͻ��������쳣�Ͽ�(����ͻ��˱������߰ε����ߵ�)��ʱ�򣬷����������޷��յ��ͻ��˶Ͽ���֪ͨ��
				if (-1 == send(pIocpParam->m_sock, "", 0, 0))
				{
					pThis->MoveToFreeParamPool(pIocpParam);
					pThis->RemoveStaleClient(pIoContext,FALSE);
				}
				continue;
			}
			if(ERROR_NETNAME_DELETED == dwErr)// �ͻ����쳣�˳�
			{
				pThis->MoveToFreeParamPool(pIocpParam);
				pThis->RemoveStaleClient(pIoContext, FALSE);
				continue;
			}
	
			break;// ��ɶ˿ڳ��ִ���
		}
	
		// ��ʽ������յ������� ��ȡ���յ�������
		// CONTAINING_RECORD�귵�ظ����ṹ���͵Ľṹʵ���� ����ַ �Ͱ����ṹ���ֶεĵ�ַ��
		if (bRet && 0 == dwIoSize)
		{
			// �ͻ��˶Ͽ����ӣ��ͷ���Դ
			pThis->MoveToFreeParamPool(pIocpParam);
			pThis->RemoveStaleClient(pIoContext, FALSE);
			continue;
		}

		if (bRet && NULL != pIoContext && NULL != pIocpParam)
		{
			try
			{
				pThis->ProcessIOMessage(pIoContext->m_ioType, pIoContext, dwIoSize);
			}
			catch (...) {}
		}
	}
	return 0;
}

// ���յ��ͻ��˵�����
bool CIOCPServer::OnAccept(PER_IO_CONTEXT* pIoContext)
{
	SOCKADDR_IN* RemoteSockAddr = NULL;
	SOCKADDR_IN* LocalSockAddr = NULL;
	int nLen = sizeof(SOCKADDR_IN);

	///////////////////////////////////////////////////////////////////////////
	// 1. m_lpfnGetAcceptExSockAddrs ȡ�ÿͻ��˺ͱ��ض˵ĵ�ַ��Ϣ �� �ͻ��˷����ĵ�һ������
	// ����ͻ���ֻ�������˶�������Ϣ���ǲ�����յ���
	this->m_lpfnGetAcceptExSockAddrs(
		pIoContext->m_wsaBuf.buf,						// ��һ����Ϣ
		pIoContext->m_wsaBuf.len - ((nLen + 16) * 2),
		nLen + 16, nLen + 16,
		(sockaddr**)&LocalSockAddr, &nLen,				// ������Ϣ
		(sockaddr**)&RemoteSockAddr, &nLen);			// �ͻ�����Ϣ
	
	PER_IO_CONTEXT* pNewIoContext = AllocateClientIOContext();
	pNewIoContext->m_sock = pIoContext->m_sock;
	pNewIoContext->m_addr = *RemoteSockAddr;

	// ������Ϣ���˴�Ϊ�����ϣ���һ�ν��ܵ��ͻ��˵�����
	m_pNotifyProc(NULL, pIoContext, NC_CLIENT_CONNECT);

	IOCP_PARAM* pIocpParam = AllocateIocpParam();
	pIocpParam->m_sock = pNewIoContext->m_sock;
	// �������ӵĿͻ��˵�socket���󶨵���ɶ˿�
	if (!AssociateSocketWithCompletionPort(pNewIoContext->m_sock,(DWORD)pIocpParam))
	{
		closesocket(m_socListen);
		closesocket(pNewIoContext->m_sock);

		delete pNewIoContext;
		delete pIocpParam;
		pNewIoContext = NULL;
		pIocpParam = NULL;
		return false;
	}

	// Set KeepAlive ����������������������ƣ����ڱ�֤TCP�ĳ����ӣ������ڵײ㷢һЩ���ݣ����ᴫ��Ӧ�ò㣩
	unsigned long chOpt = 1;
	if (SOCKET_ERROR == setsockopt(pNewIoContext->m_sock,SOL_SOCKET,SO_KEEPALIVE,(char*)&chOpt,sizeof(char)))
	{
		// ��������ʧ��
		MoveToFreeParamPool(pIocpParam);
		RemoveStaleClient(pNewIoContext,TRUE);
		return false;
	}

	// ���ó�ʱ��ϸ��Ϣ
	tcp_keepalive	klive;
	klive.onoff = 1; // ���ñ���
	klive.keepalivetime = m_nKeepLiveTime;
	klive.keepaliveinterval = 1000 * 10; // ���Լ��Ϊ10�� Resend if No-Reply
	WSAIoctl
	(
		pNewIoContext->m_sock,
		SIO_KEEPALIVE_VALS,
		&klive,
		sizeof(tcp_keepalive),
		NULL,
		0,
		(unsigned long *)&chOpt,
		0,
		NULL
	);

	// �������ӵ��׽���Ͷ�ݽ��ղ���
	if (!PostRecv(pNewIoContext))
	{
		MoveToFreeParamPool(pIocpParam);
	}

	CLock cs(m_cs, "OnAccept");

	pIoContext->Clear();		// �ٴγ�ʼ���������ٴ�����
	return PostAcceptEx(pIoContext);
}



bool CIOCPServer::OnClientAccept(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	bool bRet = false;
	try
	{
		pIOContext->m_dwBytesRecv = dwSize;
		// ... ����һЩ���յ��Ĳ���
		bRet = OnAccept(pIOContext);
	}
	catch (...) {}

	return bRet;
}

bool CIOCPServer::OnClientReading(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	CLock cs(m_cs, "OnClientReading");
	bool bRet = false;
	try
	{
		// ������յ�������
		m_pNotifyProc(NULL, pIOContext, NC_RECEIVE);

		// ��Ͷ��һ���첽������Ϣ
		bRet = PostRecv(pIOContext);
	}
	catch (...){}

	return bRet;
}

bool CIOCPServer::PostRecv(PER_IO_CONTEXT* pIoContext)
{
	// ��ջ��������ٴ�Ͷ��
	ZeroMemory(pIoContext->m_szBuf, MAX_BUFFER_LEN);
	ZeroMemory(&pIoContext->m_ol, sizeof(OVERLAPPED));
	pIoContext->m_ioType = IORecv;
	DWORD dwNumBytesOfRecvd;

	ULONG ulFlags = MSG_PARTIAL;
	UINT nRet = WSARecv(
		pIoContext->m_sock,
		&(pIoContext->m_wsaBuf),
		1,
		&dwNumBytesOfRecvd,// ���յ��ֽ������첽�����ķ��ؽ��һ��Ϊ0��������յ����ֽ�������ɶ˿ڻ��
		&(ulFlags),
		&(pIoContext->m_ol),
		NULL);
	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		RemoveStaleClient(pIoContext, FALSE);
		return false;
	}
	return true;
}


bool CIOCPServer::OnClientWriting(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	bool bRet = false;
	try 
	{
		// �첽���͵ķ��صĴ���ɹ��Ľ���Ƿ� ���� Ҫ���͵����ݴ�С��δ������ɣ�����ʱ�ط�
		// ���ʹ��CRCУ��֮�ڵģ������Ͻ��ԣ������ڽṹ���з�һ�������CRCֵ�������Ǽ�������������
		if (dwSize != pIOContext->m_dwBytesSend)
		{
			bRet = PostSend(pIOContext);
		}
		else// �Ѿ����ͳɹ������ṹ��Ż��ڴ��
		{
			m_pNotifyProc(NULL, pIOContext, NC_TRANSMIT);
			MoveToFreePool(pIOContext);
		}
	}
	catch (...) {}

	return bRet;
}



bool CIOCPServer::PostSend(PER_IO_CONTEXT* pIoContext)
{
	pIoContext->m_wsaBuf.buf = pIoContext->m_szBuf;
	pIoContext->m_wsaBuf.len = strlen(pIoContext->m_szBuf);
	pIoContext->m_ioType = IOSend;
	ULONG ulFlags = MSG_PARTIAL;

	INT nRet = WSASend(
		pIoContext->m_sock,
		&pIoContext->m_wsaBuf,
		1,
		&(pIoContext->m_wsaBuf.len),
		ulFlags,
		&(pIoContext->m_ol),
		NULL);
	if (SOCKET_ERROR == nRet && WSA_IO_PENDING != WSAGetLastError())
	{
		RemoveStaleClient(pIoContext, FALSE);
		return false;
	}
	return true;
}

bool CIOCPServer::AssociateSocketWithCompletionPort(SOCKET socket, DWORD dwCompletionKey)
{
	// ���׽��ְ󶨵���ɶ˿�
	// �ڶ�������Ϊ��ɶ˿ھ��ʱ������ֵΪ��ɶ˿ڡ�Ϊ��ʱ�������µ���ɶ˿ھ��
	HANDLE hTmp = CreateIoCompletionPort((HANDLE)socket, m_hIOCompletionPort, dwCompletionKey, 0);
	return hTmp == m_hIOCompletionPort;
}


PER_IO_CONTEXT* CIOCPServer::AllocateClientIOContext()
{
	CLock cs(this->m_cs, "AllocateSocketContext");

	PER_IO_CONTEXT* pIoContext = NULL;
	if (!m_listFreeIoContext.empty())
	{
		pIoContext = m_listFreeIoContext.front();
		m_listFreeIoContext.remove(pIoContext);
	}
	else
	{
		pIoContext = new PER_IO_CONTEXT;
	}
	
	m_listIoContext.push_back(pIoContext);
	if (pIoContext != NULL)//һ�㶼�Ǳ�����˵�
	{
		// �˴�������
		pIoContext->Clear();
	}

	return pIoContext;
}


IOCP_PARAM* CIOCPServer::AllocateIocpParam()
{
	CLock cs(m_cs, "AllocateIocpParam");

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
		pIocpParam->m_sock = INVALID_SOCKET;
	}
	return pIocpParam;
}

VOID CIOCPServer::RemoveStaleClient(PER_IO_CONTEXT* pIoContext, BOOL bGraceful/*�Ƿ���ֹ����*/)
{
	CLock cs(m_cs, "RemoveStaleClient");

	LINGER lingerStruct;

	// �������Ҫ��ֹ���ӣ�������ʱֵΪ 0 (���ŵĹر�����)
	if (!bGraceful)
	{
		//.l_onoff=1;����closesocket()����,���ǻ�������û������ϵ�ʱ��������)
		// ���.l_onoff=0;���ܺ�2.)������ͬ;
		lingerStruct.l_onoff = 1;	//���أ�����߷��� 
		lingerStruct.l_linger = 0;  //���Źر��ʱ�ޣ���������ʱ�� �룩
		setsockopt(pIoContext->m_sock, SOL_SOCKET, SO_LINGER,
			(char*)&lingerStruct, sizeof(lingerStruct));
	}

	// �ͷ� PER_SOCKET_CONTEXT �е�����
	std::list<PER_IO_CONTEXT*>::iterator iter;
	iter = find(m_listIoContext.begin(), m_listIoContext.end(), pIoContext);
	if (iter != m_listIoContext.end())
	{
		// CancelIo ȡ�������IO���� ���ŵĹر�����׽��־��
		CancelIo((HANDLE)pIoContext->m_sock);

		closesocket(pIoContext->m_sock);
		pIoContext->m_sock = INVALID_SOCKET;

		// �ж��Ƿ��ڽ���IO���� �ȴ���һ��IO��������ٹر�
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)pIoContext))
			Sleep(0);

		// �ص������������˳���Ϣ
		m_pNotifyProc(NULL, pIoContext, NC_CLIENT_DISCONNECT);

		MoveToFreePool(pIoContext);
	}
}


VOID CIOCPServer::MoveToFreeParamPool(IOCP_PARAM* pIocpParam)
{
	CLock cs(m_cs, "MoveToFreeParamPool");

	IocpParamList::iterator iter;
	iter = find(m_listIocpParam.begin(), m_listIocpParam.end(), pIocpParam);
	if (iter != m_listIocpParam.end())
	{
		m_listFreeIocpParam.push_back(pIocpParam);
		m_listIocpParam.remove(pIocpParam);
	}
}

VOID CIOCPServer::MoveToFreePool(PER_IO_CONTEXT* pIoContext)
{
	CLock cs(m_cs, "MoveToFreePool");

	IOContextList::iterator iter;
	iter = find(m_listIoContext.begin(), m_listIoContext.end(), pIoContext);

	if (iter != m_listIoContext.end())
	{
		m_listFreeIoContext.push_back(pIoContext);// ���ͷ��Ѿ������ģ����뵽�ڴ���У��´����¿ͻ������ӾͲ����ٴ���
		m_listIoContext.remove(pIoContext);
	}
}


VOID CIOCPServer::ReleaseResource()
{
	// ɾ���ؼ���
	DeleteCriticalSection(&m_cs);

	// �ͷ� ϵͳ�˳��¼����
	RELEASE_HANDLE(m_hShutDownEvent);

	// �ͷŹ������߳̾��ָ��
	for (unsigned int i = 0; i < m_nThreadCnt; i++)
	{
		RELEASE_HANDLE(m_pWorkThreads[i]);
	}
	RELEASE(m_pWorkThreads);

	// �ر�IOCP���
	RELEASE_HANDLE(m_hIOCompletionPort);

	// �رռ����׽���
	RELEASE_SOCKET(m_socListen);


	// ɾ�������׽��ֵ���ɶ˿ڲ���
	delete m_pListenIocpParam;

	IOContextList::iterator iter;

	// ������е��׽���
	iter = m_listFreeIoContext.begin();
	while (iter != m_listFreeIoContext.end())
	{
		delete *iter;
		++iter;
	}
	m_listFreeIoContext.clear();

	// �������ӵ��׽���
	iter = m_listIoContext.begin();
	while (iter != m_listIoContext.end())
	{
		closesocket((*iter)->m_sock);
		delete *iter;
		++iter;
	}
	m_listIoContext.clear();

	// ����Ԥ�������׽���
	iter = m_listAcceptExSock.begin();
	while (iter != m_listAcceptExSock.end())
	{
		delete *iter;
		++iter;
	}
}