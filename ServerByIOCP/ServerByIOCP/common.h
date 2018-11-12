#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include<memory.h>
#include<windows.h>
#include <MSWSock.h>
#include <MSTcpIP.h>//������������ͷ�ļ�
#include <list>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define SRV_IP "192.168.15.112"

#define MAX_BUFFER_LEN					4096		// �������һ��Ϊ8k
#define WORKER_THREADS_PER_PROCESSOR	2
#define MAX_POST_ACCEPT					10			// ͬʱͶ��AcceptEx����������
#define EXIT_CODE						NULL			

// �ͷ�ָ���
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// �ͷž����
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// �ͷ�Socket��
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

////////////////////////////////////////////////////////////////////
#define	NC_CLIENT_CONNECT		0x0001	//�ͻ�������
#define	NC_CLIENT_DISCONNECT	0x0002	//�ͻ��˶Ͽ�
#define	NC_TRANSMIT				0x0003	//��������
#define	NC_RECEIVE				0x0004	//��������
#define NC_RECEIVE_COMPLETE		0x0005	// ��������

////////////////////////////////////////////////////////////////////
//����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum EM_IOType
{
	EM_IOAccept,						 // ��־Ͷ�ݵ� Accept��ʼ������
	EM_IOSend,							 // ��־Ͷ�ݵ��� ���Ͳ���(д)
	EM_IORecv,							 // ��־Ͷ�ݵ��� ���ղ���(��)
	EM_IOIdle					   	     // ���ڳ�ʼ����������
}IOType;


//====================================================================================
//				��IO���ݽṹ�嶨��(����ÿһ���ص������Ĳ���)
//====================================================================================

struct PER_IO_CONTEXT
{
	OVERLAPPED		m_overLapped;								//����IOCP�ĺ������ݽṹ!!!!!!
	IOType			m_IOType;									//���յ���IO��Ҫ���������
	WSABUF			m_wsaBuf;                                   // WSA���͵Ļ����������ڸ��ص�������������
	SOCKET			m_sock;										// ���յ������ӵ��׽��� 
	SOCKADDR_IN		m_addr;										// �׽��ֵ�ַ��Ϣ
	char			m_szBuf[MAX_BUFFER_LEN];					//�����WSABUF�������ַ��Ļ�����
	DWORD			m_dwBytesSend;								// ���͵��ֽ���
	DWORD			m_dwBytesRecv;								// ���յ��ֽ���

	void Init()
	{
		ZeroMemory(&m_overLapped, sizeof(OVERLAPPED));
		ZeroMemory(m_szBuf, MAX_BUFFER_LEN);
		ZeroMemory(&m_addr, sizeof(SOCKADDR_IN));
		m_IOType = EM_IOIdle;
		m_sock = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuf;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_dwBytesSend = 0;
		m_dwBytesRecv = 0;
	}

	void Reset()
	{
		ZeroMemory(&m_overLapped, sizeof(OVERLAPPED));
		ZeroMemory(m_szBuf, MAX_BUFFER_LEN);
		ZeroMemory(&m_addr, sizeof(SOCKADDR_IN));
		m_IOType = EM_IOIdle;
		m_sock = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuf;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_dwBytesSend = 0;
		m_dwBytesRecv = 0;
	}
};

const int const_nMsgSize = sizeof(char) * sizeof(PER_IO_CONTEXT);

struct IOCP_PARAM					 // ��ɶ˿ڴ��ݵĲ���
{
	SOCKET m_sock;
};
