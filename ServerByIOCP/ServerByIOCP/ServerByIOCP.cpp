// ServerByIOCP.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "CoreByIOCP.h"
#include <WS2tcpip.h>

CCoreByIOCP* pIOCP;

/*
	此服务器就相当于一个中转器！！！
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
			<< "]->连接，第一条数据【" << ipInfo.nSelfID << ":" << ipInfo.nSendToID << ":" << ipInfo.buf << "】" << endl;

		// 收到后投递一个异步数据
		//PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		//pSendIoContext->m_rourceSock = pIoContext->m_rourceSock;
		//pSendIoContext->m_resourceAddr = pIoContext->m_resourceAddr;

		//pSendIoContext->m_desID = ipInfo.nSendToID;
		//memcpy(pSendIoContext->m_szBuf, ipInfo.buf, strlen(ipInfo.buf));
		//pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(ipInfo.buf));
		//if (!pIOCP->PostSend(pSendIoContext, false))
		//{
		//	//此处不给客户端发反馈消息，因为这里只是接收到客户端的刚连接时发的一部分数据，到NC_RECEIVE处理时才算完全接收到客户端发送的一次包
		//	//发送给目标地址不成功，服务器回复客户端目标地址IP或Port不正确
		//	////char szSend[] = "发送的目标地址IP或Port不正确！！！";
		//	////memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
		//	////pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
		//	////pIOCP->PostSend(pSendIoContext, true);
		//}
		break;
	}
	case NC_CLIENT_DISCONNECT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->断开连接" << endl;
		break;
	}
	case NC_TRANSMIT:
	{
		cout << "[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->数据发送成功【" << pIoContext->m_szBuf << "】" << endl;
		break;
	}
	case NC_RECEIVE:
	{
		SendToIpInfo ipInfo;
		ZeroMemory(&ipInfo, MAX_BUFFER_LEN);
		memcpy(&ipInfo, pIoContext->m_szBuf, MAX_BUFFER_LEN);
		cout << "接收到[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->【" << ipInfo.nSelfID << ":" << ipInfo.nSendToID << ":" << ipInfo.buf << "】" << endl;
		// 收到后投递一个异步数据
		//PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		//pSendIoContext->m_rourceSock = pIoContext->m_rourceSock;
		//pSendIoContext->m_resourceAddr = pIoContext->m_resourceAddr;

		//pSendIoContext->m_desID = ipInfo.nSendToID;
		//memcpy(pSendIoContext->m_szBuf, ipInfo.buf, strlen(ipInfo.buf));
		//pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(ipInfo.buf));
		//if (pIOCP->PostSend(pSendIoContext, false))
		//{
		//	char szSend[] = "目标地址已成功接收！！！";
		//	memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
		//	pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
		//	pIOCP->PostSend(pSendIoContext, true);
		//}
		//else
		//{
		//	//发送给目标地址不成功，服务器回复客户端目标地址IP或Port不正确
		//	char szSend[] = "发送的目标地址IP或Port不正确！！！";
		//	memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
		//	pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
		//	pIOCP->PostSend(pSendIoContext, true);
		//}

		break;
	}
	case NC_RECEIVE_COMPLETE:
	{
		SendToIpInfo ipInfo;
		ZeroMemory(&ipInfo, MAX_BUFFER_LEN);
		memcpy(&ipInfo, pIoContext->m_szBufCache, MAX_BUFFER_LEN);
		cout << "服务器已完整接收到您发的信息[" << inet_ntoa(pIoContext->m_resourceAddr.sin_addr) << "-" << htons(pIoContext->m_resourceAddr.sin_port)
			<< "]->【" << ipInfo.nSelfID << ":" << ipInfo.nSendToID << ":" << ipInfo.buf << "】" << endl;

		if (ipInfo.nSendToID <= 0)
		{
			break;
		}

		// 收到后投递一个异步数据
		PER_IO_CONTEXT* pSendIoContext = pIOCP->AllocateClientIOContext();
		pSendIoContext->m_rourceSock = pIoContext->m_rourceSock;
		pSendIoContext->m_resourceAddr = pIoContext->m_resourceAddr;

		pSendIoContext->m_desID = ipInfo.nSendToID;
		memcpy(pSendIoContext->m_szBuf, ipInfo.buf, strlen(ipInfo.buf));
		pSendIoContext->m_dwBytesSend = 0;
		if (pIOCP->PostSend(pSendIoContext, false))
		{
			char szSend[] = "目标地址已成功接收！！！";
			memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
			pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
			pIOCP->PostSend(pSendIoContext, true);
		}
		else
		{
			//发送给目标地址不成功，服务器回复客户端目标地址IP或Port不正确
			char szSend[] = "发送的目标地址IP或Port不正确！！！";
			memcpy(pSendIoContext->m_szBuf, szSend, strlen(szSend));
			pSendIoContext->m_dwBytesSend = static_cast<WORD>(strlen(szSend));
			pIOCP->PostSend(pSendIoContext, true);
		}

		break;
	}
		
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

