// ServerByIOCP.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "CoreByIOCP.h"
#include <WS2tcpip.h>

CCoreByIOCP* pIOCP;

/*
	�˷��������൱��һ����ת��������
*/
void CALLBACK NotifyProc(LPVOID lparam, PER_IO_CONTEXT* pIoContext, UINT uFlag)
{
	switch (uFlag)
	{
	case NC_CLIENT_CONNECT:
	{
		SendToIpInfo ipInfo;
		ZeroMemory(&ipInfo, MAX_BUFFER_LEN);
		memcpy(&ipInfo, pIoContext->m_szBuf, MAX_BUFFER_LEN);
		cout << "[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->���ӣ���һ�����ݡ�" << ipInfo.sin_IP << ":" << ipInfo.sin_port << ":" << ipInfo.buf << "��" << endl;

		// �յ���Ͷ��һ���첽����
		PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		pSendIoContext->m_rourceSock = pIoContext->m_rourceSock;
		pSendIoContext->m_resourceAddr = pIoContext->m_resourceAddr;

		string strAddr = ipInfo.sin_IP;
		string strPort = ipInfo.sin_port;
		pSendIoContext->m_desAddr = strAddr + ":" + strPort;
		memcpy(pSendIoContext->m_szBuf, ipInfo.buf, strlen(ipInfo.buf));
		pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(ipInfo.buf));
		pIOCP->PostSend(pSendIoContext);
		break;
	}
	case NC_CLIENT_DISCONNECT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->�Ͽ�����" << endl;
		break;
	}
	case NC_TRANSMIT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->���ݷ��ͳɹ���" << pIoContext->m_szBuf << "��" << endl;
		break;
	}
	case NC_RECEIVE:
	{
		SendToIpInfo ipInfo;
		ZeroMemory(&ipInfo, MAX_BUFFER_LEN);
		memcpy(&ipInfo, pIoContext->m_szBuf, MAX_BUFFER_LEN);
		cout << "���յ�[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->��" << ipInfo.sin_IP<<":"<<ipInfo.sin_port<<":"<<ipInfo.buf<< "��" << endl;
		// �յ���Ͷ��һ���첽����
		PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		pSendIoContext->m_rourceSock = pIoContext->m_rourceSock;
		pSendIoContext->m_resourceAddr = pIoContext->m_resourceAddr;
		char szSend[] = "�ѷ��ͳɹ�������";
		memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
		pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
		pIOCP->PostSend(pSendIoContext);
		break;
	}
	case NC_RECEIVE_COMPLETE:
		break;
	default:
		break;
	}
	return;
}

int main()
{
	pIOCP = new CCoreByIOCP;
	pIOCP->RunCore(NotifyProc, 8888);
	char szIn[32];
	while (1)
	{
		printf("���� quit �˳�IOCP������: \n");
		memset(szIn, 0, sizeof(szIn));
		scanf_s("%s", szIn, 32);
		if (strcmp(szIn, "quit") == 0)
		{
			pIOCP->StopCore();
			break;
		}
	}

	system("pause");

    return 0;
}

