// ServerByIOCP.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "CoreByIOCP.h"

CCoreByIOCP* pIOCP;

void CALLBACK NotifyProc(LPVOID lparam, PER_IO_CONTEXT* pIoContext, UINT uFlag)
{
	switch (uFlag)
	{
	case NC_CLIENT_CONNECT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_addr.sin_addr) << "-" << htons(pIoContext->m_addr.sin_port)
			<< "]->连接，第一条数据【" << pIoContext->m_szBuf << "】" << endl;
		break;
	}
	case NC_CLIENT_DISCONNECT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_addr.sin_addr) << "-" << htons(pIoContext->m_addr.sin_port)
			<< "]->断开连接" << endl;
		break;
	}
	case NC_TRANSMIT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_addr.sin_addr) << "-" << htons(pIoContext->m_addr.sin_port)
			<< "]->数据发送成功【" << pIoContext->m_szBuf << "】" << endl;
		break;
	}
	case NC_RECEIVE:
	{
		cout << "接收到[" << inet_ntoa(pIoContext->m_addr.sin_addr) << "-" << htons(pIoContext->m_addr.sin_port)
			<< "]->【" << pIoContext->m_szBuf << "】" << endl;
		// 收到后投递一个异步数据
		PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		pSendIoContext->m_sock = pIoContext->m_sock;
		pSendIoContext->m_addr = pIoContext->m_addr;
		char szSend[] = "我已收到你的数据";
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
		printf("输入 quit 退出IOCP服务器: \n");
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

