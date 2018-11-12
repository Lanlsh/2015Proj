#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "IOCPDef.h"
#include "Mapper.h"

typedef void (CALLBACK* NOTIFYPROC)(LPVOID, PER_IO_CONTEXT*, UINT);

typedef std::list<PER_IO_CONTEXT*> IOContextList;
typedef std::list<IOCP_PARAM*>	IocpParamList;

class CIOCPServer
{
public:
	CIOCPServer();
	~CIOCPServer();

	bool StartIOCP(NOTIFYPROC pNotifyProc, const UINT& nPort);
	bool Stop();

	bool PostSend(PER_IO_CONTEXT* pIoContext);				// Ͷ��һ�����Ͳ���
	PER_IO_CONTEXT* AllocateClientIOContext();				// ���ڴ���з���һ��socket�ṹ��

protected:
	static unsigned __stdcall ThreadPoolFunc(LPVOID lpParam);

	bool InitNetEnvironment();								// ��ʼ�����绷��
	bool InitializeIOCP();									// ��ʼ����ɶ˿�
	bool InitializeListenSocket();							// ��ʼ�������׽���
	
	// �ڴ�ص�ʵ��
	VOID MoveToFreeParamPool(IOCP_PARAM* pIocpParam);		// ���Ƴ�����ɶ˿ڲ�����ӵ��ڴ��
	VOID MoveToFreePool(PER_IO_CONTEXT* pIoContext);		// ���Ƴ��Ŀͻ����׽�����ӵ��ڴ��
	IOCP_PARAM* AllocateIocpParam();						// ������ɶ˿ڵĽṹ��

	// Ͷ�ݲ���
	bool PostAcceptEx(PER_IO_CONTEXT* pAcceptIoContext);	// Ͷ��һ��Accept����
	bool PostRecv(PER_IO_CONTEXT* pIoContext);				// Ͷ��һ�����յ�IO����

	bool OnAccept(PER_IO_CONTEXT* pIoContext);
	bool AssociateSocketWithCompletionPort(SOCKET socket, DWORD dwCompletionKey);
	
	// ��Դ���ͷ�
	VOID RemoveStaleClient(PER_IO_CONTEXT* pIoContext, BOOL bGraceful);// ����ͻ����׽���
	VOID ReleaseResource();									// �ͷ���Դ

	// ��Ϣӳ��
	BEGIN_IO_MSG_MAP()
		IO_MESSAGE_HANDLER(IORecv, OnClientReading)
		IO_MESSAGE_HANDLER(IOSend, OnClientWriting)
		IO_MESSAGE_HANDLER(IOAccept, OnClientAccept)
	END_IO_MSG_MAP()

	bool OnClientAccept(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);
	bool OnClientReading(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);
	bool OnClientWriting(PER_IO_CONTEXT* pIOContext, DWORD dwSize = 0);

private:
	NOTIFYPROC m_pNotifyProc;					// ��Ϣ�ص�����
	static CRITICAL_SECTION m_cs;				// �ؼ��Σ��ٽ���Դ��

	HANDLE m_hShutDownEvent;					// ϵͳ�˳��¼�֪ͨ
	HANDLE m_hIOCompletionPort;					// ��ɶ˿ھ��
	UINT m_nThreadCnt;						    // �߳�����
	HANDLE* m_pWorkThreads;						// �������߳���ָ��

	PER_IO_CONTEXT* m_pListenContext;		    // ���ڼ�����Socket��Context��Ϣ
	SOCKET m_socListen;							// �����׽���
	UINT m_nPort;								// �����Ķ˿�

	IOContextList m_listAcceptExSock;			// AcceptEx Ԥ�������׽��֣����ڹ���

	IOContextList m_listIoContext;				// �Ѿ����ӵĿͻ���
	IOContextList m_listFreeIoContext;			// �Ͽ��Ŀͻ��˵�����	��ͬʵ��һ���ڴ��
	IocpParamList m_listIocpParam;				// �Ѿ�ʹ�õĲ�������
	IocpParamList m_listFreeIocpParam;			// ���е���ɶ˿ڲ�������

	IOCP_PARAM* m_pListenIocpParam;				// �����׽��ֵ���ɶ˿ڲ���

	int m_nKeepLiveTime;						// ������̽����ʱ��

	LPFN_ACCEPTEX  m_lpfnAcceptEx;              // AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
	LPFN_GETACCEPTEXSOCKADDRS   m_lpfnGetAcceptExSockAddrs;
};
