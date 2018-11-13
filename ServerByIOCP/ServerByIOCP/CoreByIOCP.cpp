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
	m_nKeepLiveTime = 1000 * 60 * 3; // ������̽��һ��
	m_pListenIocpParam = new IOCP_PARAM;
	m_mapClient.clear();

	//���ô˱�־
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

	//���ô˱�־
	m_bIsExecStopCore = false;

	//��ʼ���ٽ�������
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
		cout << "����������ʧ��!!!!!!!!" << endl;
		::MessageBox(GetDesktopWindow(), szErr, L"����������ʧ��", MB_OK | MB_ICONHAND);
	}

	return bRet;
}

void CCoreByIOCP::StopCore()
{
	//���ñ�־
	m_bIsExecStopCore = true;

	if (m_socListen != INVALID_SOCKET)
	{
		SetEvent(m_hShutDownEvent);

		if (m_pWorkThreads)
		{
			for (unsigned int i = 0; i <m_pWorkThreads->GetCurrentThreadCount(); i++)
			{
				//PostQueuedCompletionStatus��������ÿ���������̶߳����͡��������������ݰ����ú�����ָʾÿ���̶߳��������������˳���
				/*
				ͨ��Ϊ�̳߳��е�ÿ���̶߳�����һ��PostQueuedCompletionStatus�����ǿ��Խ����Ƕ����ѡ�
				ÿ���̻߳��GetQueuedCompletionStatus�ķ���ֵ���м�飬�������Ӧ�ó���������ֹ����ô���ǾͿ��Խ������������������˳���
				"Ҳ����˵���ô˾�ỽ����Ӧ���̣߳��Ӷ�����Ҫ���ö������ȥ������Ӧ�̣߳�������"
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

	// ������ɶ˿�
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIOCompletionPort)
		return false;

	GetSystemInfo(&systemInfo);
	// �߳��������ƣ���ֹ�߳��������л��˷���Դ
	int m_nThreadCnt = WORKER_THREADS_PER_PROCESSOR * systemInfo.dwNumberOfProcessors;// ������������

	m_pWorkThreads = LanThreadPool::GetLanThreadPoolInstance();
	m_pWorkThreads->SetThreadPool(m_nThreadCnt, m_nThreadCnt);
	m_pWorkThreads->SetIOCP(this);
	m_pWorkThreads->Run();

	return true;
}

bool CCoreByIOCP::InitializeListenSocket()
{
	// AcceptEx �� GetAcceptExSockaddrs ��GUID�����ڵ�������ָ��
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// ��Ҫʹ���ص�IO������ʹ��WSASocket������Socket���ſ���֧���ص�IO����
	m_socListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_socListen)
		return false;

	m_pListenIocpParam->m_rourceSock = m_socListen;
	// �� �����׽��� �󶨵���ɶ˿���
	if (!AssociateSocketWithCompletionPort(m_socListen, (DWORD)(m_pListenIocpParam->m_rourceSock)))
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
	if (SOCKET_ERROR == ::bind(m_socListen, (struct sockaddr*)&servAddr, sizeof(servAddr)))
	{
		RELEASE_SOCKET(m_socListen);
		return false;
	}

	// ��ʼ����
	if (SOCKET_ERROR == listen(m_socListen, SOMAXCONN))
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
		m_listFreeIoContext.push_back(pIoContext);// ���ͷ��Ѿ������ģ����뵽�ڴ���У��´����¿ͻ������ӾͲ����ٴ���
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

VOID CCoreByIOCP::RemoveStaleClient(PER_IO_CONTEXT* pIoContext, BOOL bGraceful/*�Ƿ���ֹ����*/)
{
	LanCriticalLock cs(m_cs, "RemoveStaleClient");

	LINGER lingerStruct;

	// �������Ҫ��ֹ���ӣ�������ʱֵΪ 0 (���ŵĹر�����)
	if (!bGraceful)
	{
		//.l_onoff=1;����closesocket()����,���ǻ�������û������ϵ�ʱ��������)
		// ���.l_onoff=0;���ܺ�2.)������ͬ;
		lingerStruct.l_onoff = 1;	//���أ�����߷��� 
		lingerStruct.l_linger = 0;  //���Źر��ʱ�ޣ���������ʱ�� �룩
		setsockopt(pIoContext->m_rourceSock, SOL_SOCKET, SO_LINGER,
			(char*)&lingerStruct, sizeof(lingerStruct));
	}


	//���ÿͻ��˵���Ϣ���
	RomoveClientInfo(pIoContext->m_rourceSock);

	// �ͷ� PER_SOCKET_CONTEXT �е�����
	std::list<PER_IO_CONTEXT*>::iterator iter;
	iter = find(m_listIoContext.begin(), m_listIoContext.end(), pIoContext);
	if (iter != m_listIoContext.end())
	{
		// CancelIo ȡ�������IO���� ���ŵĹر�����׽��־��
		CancelIo((HANDLE)pIoContext->m_rourceSock);

		closesocket(pIoContext->m_rourceSock);
		pIoContext->m_rourceSock = INVALID_SOCKET;

		// �ж��Ƿ��ڽ���IO���� �ȴ���һ��IO��������ٹر�
		while (!HasOverlappedIoCompleted((LPOVERLAPPED)pIoContext))
			Sleep(0);

		// �ص������������˳���Ϣ
		m_pNotifyProc(NULL, pIoContext, NC_CLIENT_DISCONNECT);

		MoveToFreePool(pIoContext);
	}
}

VOID CCoreByIOCP::ReleaseResource()
{
	// ɾ���ؼ���
	DeleteCriticalSection(&m_cs);

	// �ͷ� ϵͳ�˳��¼����
	RELEASE_HANDLE(m_hShutDownEvent);

	// �ͷŹ������߳̾��ָ��
	if (m_pWorkThreads)
	{
		m_pWorkThreads->FreeMommery();
	}

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
		closesocket((*iter)->m_rourceSock);
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

	//��տͻ���IP��Ϣ
	m_mapClient.clear();
}

// ���յ��ͻ��˵�����
bool CCoreByIOCP::OnAccept(PER_IO_CONTEXT* pIoContext)
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
	pNewIoContext->m_rourceSock = pIoContext->m_rourceSock;
	pNewIoContext->m_resourceAddr = *RemoteSockAddr;

	// ������Ϣ���˴�Ϊ�����ϣ���һ�ν��ܵ��ͻ��˵�����
	m_pNotifyProc(NULL, pIoContext, NC_CLIENT_CONNECT);

	IOCP_PARAM* pIocpParam = AllocateIocpParam();
	pIocpParam->m_rourceSock = pNewIoContext->m_rourceSock;
	// �������ӵĿͻ��˵�socket���󶨵���ɶ˿�
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
	//���ӿͻ�����Ϣ
	AddCLientInfo(pNewIoContext->m_rourceSock, str);

	// Set KeepAlive ����������������������ƣ����ڱ�֤TCP�ĳ����ӣ������ڵײ㷢һЩ���ݣ����ᴫ��Ӧ�ò㣩
	unsigned long chOpt = 1;
	if (SOCKET_ERROR == setsockopt(pNewIoContext->m_rourceSock, SOL_SOCKET, SO_KEEPALIVE, (char*)&chOpt, sizeof(char)))
	{
		// ��������ʧ��
		MoveToFreeParamPool(pIocpParam);
		RemoveStaleClient(pNewIoContext, TRUE);
		return false;
	}

	// ���ó�ʱ��ϸ��Ϣ
	tcp_keepalive	klive;
	klive.onoff = 1; // ���ñ���
	klive.keepalivetime = m_nKeepLiveTime;
	klive.keepaliveinterval = 1000 * 10; // ���Լ��Ϊ10�� Resend if No-Reply
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

	// �������ӵ��׽���Ͷ�ݽ��ղ���
	if (!PostRecv(pNewIoContext))
	{
		MoveToFreeParamPool(pIocpParam);
	}

	LanCriticalLock cs(m_cs, "OnAccept");

	pIoContext->Reset();		// �ٴγ�ʼ���������ٴ�����
	return PostAcceptEx(pIoContext);
}



bool CCoreByIOCP::OnClientAccept(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
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

bool CCoreByIOCP::OnClientReading(PER_IO_CONTEXT* pIOContext, DWORD dwSize /*= 0*/)
{
	LanCriticalLock cs(m_cs, "OnClientReading");
	bool bRet = false;
	try
	{
		// ������յ�������
		m_pNotifyProc(NULL, pIOContext, NC_RECEIVE);

		// ��Ͷ��һ���첽������Ϣ
		bRet = PostRecv(pIOContext);
	}
	catch (...) {}

	return bRet;
}

bool CCoreByIOCP::PostAcceptEx(PER_IO_CONTEXT * pAcceptIoContext)
{
	pAcceptIoContext->m_IOType = EM_IOAccept;// ��ʼ�� IO���� Ϊ�����׽���

										  // Ϊ�Ժ�������Ŀͻ���׼����Socket�������봫ͳAccept��������
										  // ʵ�����д���һ�� �������ӳ� �������� �ڴ�أ������ȴ���һ��������socket��Ȼ��ֱ��ʹ�þ�����
	pAcceptIoContext->m_rourceSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pAcceptIoContext->m_rourceSock)
		return false;

	// Ͷ���첽 AcceptEx
	if (FALSE == m_lpfnAcceptEx(
		m_socListen,					//�����׽��֡�������Ӧ�ó���������׽����ϵȴ�����
		pAcceptIoContext->m_rourceSock,		//���������ӵ��׽��֡����׽��ֱ��벻���Ѿ��󶨻����Ѿ����ӡ�
		pAcceptIoContext->m_wsaBuf.buf,	//ָ��һ�����������û��������ڽ����½����ӵ����������ݵĵ�һ���顢�÷������ı��ص�ַ�Ϳͻ��˵�Զ�̵�ַ�����յ������ݽ���д�뵽������0ƫ�ƴ�������ַ���д�롣 �ò�������ָ��������˲�������ΪNULL��������õ�ִ�У�Ҳ�޷�ͨ��GetAcceptExSockaddrs������ñ��ػ�Զ�̵ĵ�ַ
		pAcceptIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),	//��һ��СӦ�������������ı��ص�ַ�Ĵ�С��ͻ��˵�Զ�̵�ַ�����Ǳ�׷�ӵ���������������dwReceiveDataLength���㣬AcceptEx�����ȴ������κ����ݣ����Ǿ��콨�����ӡ�
		sizeof(SOCKADDR_IN) + 16,	//Ϊ���ص�ַ��Ϣ�������ֽ�������ֵ��������ô���Э�������ַ��С��16���ֽ�
		sizeof(SOCKADDR_IN) + 16,	//ΪԶ�̵�ַ����Ϣ�������ֽ�������ֵ��������ô���Э�������ַ��С��16���ֽڡ� ��ֵ����Ϊ0��
		&(pAcceptIoContext->m_dwBytesRecv),	//ָ��һ��DWORD���ڱ�ʶ���յ����ֽ������˲���ֻ����ͬ��ģʽ�������塣�����������ERROR_IO_PENDING���ڳ�Щʱ����ɲ�������ô���DWORDû�����壬��ʱ������ô����֪ͨ�����ж�ȡ�����ֽ���
		&(pAcceptIoContext->m_overLapped))	//һ��OVERLAPPED�ṹ�����ڴ������󡣴˲�������ָ����������Ϊ��
		)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
			return false;
	}

	return true;
}

bool CCoreByIOCP::PostRecv(PER_IO_CONTEXT* pIoContext)
{
	// ��ջ��������ٴ�Ͷ��
	ZeroMemory(pIoContext->m_szBuf, MAX_BUFFER_LEN);
	ZeroMemory(&pIoContext->m_overLapped, sizeof(OVERLAPPED));
	pIoContext->m_IOType = EM_IORecv;
	DWORD dwNumBytesOfRecvd;

	ULONG ulFlags = MSG_PARTIAL;
	UINT nRet = WSARecv(
		pIoContext->m_rourceSock,
		&(pIoContext->m_wsaBuf),
		1,
		&dwNumBytesOfRecvd,// ���յ��ֽ������첽�����ķ��ؽ��һ��Ϊ0��������յ����ֽ�������ɶ˿ڻ��
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
	// ���׽��ְ󶨵���ɶ˿�
	// �ڶ�������Ϊ��ɶ˿ھ��ʱ������ֵΪ��ɶ˿ڡ�Ϊ��ʱ�������µ���ɶ˿ھ��
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