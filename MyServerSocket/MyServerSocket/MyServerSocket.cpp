// MyServerSocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MyServer.h"
using namespace std;

string GetLocalIP()
{

	//使用gethostname函数获取  -----在VS2013后已被放弃
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return "";

	char local[255] = { 0 };
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return "";

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip

	string localIP;
	char IPdotdec[16] = { 0 };
	inet_ntop(AF_INET, &addr, IPdotdec, 15);
	localIP = IPdotdec;

	WSACleanup();

	return localIP;


	//使用getaddrinfo获取本机IP
	//struct addrinfo *result = NULL;
	//struct addrinfo *ptr = NULL;
	//struct addrinfo hints;

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;

	//char hostname[255] = { 0 };
	//int dwRetval = getaddrinfo(hostname, 0, &hints, &result);
	//if (dwRetval != 0) 
	//{
	//	cout<<"获取本机IP失败"<<endl;
	//	return "";
	//}
	//else
	//{
	//	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	//	{

	//		printf("\tFlags: 0x%x\n", ptr->ai_flags);
	//		printf("\tFamily: ");
	//		switch (ptr->ai_family) {
	//		case AF_UNSPEC:
	//			printf("Unspecified\n");
	//			break;
	//		case AF_INET:
	//			printf("AF_INET (IPv4)\n");
	//			sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
	//			printf("\tIPv4 address %s\n",
	//				inet_ntoa(sockaddr_ipv4->sin_addr));
	//			break;
	//		case AF_INET6:
	//			printf("AF_INET6 (IPv6)\n");
	//			// the InetNtop function is available on Windows Vista and later
	//			// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
	//			// printf("\tIPv6 address %s\n",
	//			//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

	//			// We use WSAAddressToString since it is supported on Windows XP and later
	//			LPSOCKADDR sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
	//			// The buffer length is changed by each call to WSAAddresstoString
	//			// So we need to set it for each iteration through the loop for safety
	//			int ipbufferlength = 46;
	//			bool iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
	//				(PRSPEC_LPWSTR)(hostname), (DWORD*)(&ipbufferlength));
	//			if (iRetval)
	//				printf("WSAAddressToString failed with %u\n", WSAGetLastError());
	//			else
	//				printf("\tIPv6 address %s\n", hostname);
	//			break;
	//		case AF_NETBIOS:
	//			printf("AF_NETBIOS (NetBIOS)\n");
	//			break;
	//		default:
	//			printf("Other %ld\n", ptr->ai_family);
	//			break;
	//		}

	//	}
	//}

}

int main()
{
	MyServer server;
	char clocalIp[16] = { 0 };
	//获取本机IP
	string strIP = GetLocalIP();

	if (strIP != "")
	{
		memcpy(clocalIp, strIP.c_str(), 15 * sizeof(char));
		cout << "获取的本机IP: " << clocalIp << endl;
		server.CreatMyServer(clocalIp, 8888);
		server.AddConnectNameAndIp("良", clocalIp);
		//server.AddConnectNameAndIp("志淋", "192.168.15.64");
		//server.AddConnectNameAndIp("关羽", "192.168.15.151");
		server.StartServerAllIp();
	}

	cout << endl << "-------退出程序--------" << endl;

	system("pause");

    return 0;
}

