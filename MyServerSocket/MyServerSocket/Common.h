#pragma once
#include "stdafx.h"
namespace Common
{
	//������������ӵĿͻ��˷����ʺ���Ϣ
	void ServerSayHelloToClient(SOCKET clientSocket)
	{
		//��������6���ʺ����
		for (int i = 0; i<6; i++)
		{
			if (send(clientSocket, "Server say hello to you!\n", 14, 0) == -1)
			{
				//������ŵĹر�windows��socket���뿴https://blog.csdn.net/qq_16490557/article/details/36172769
				//int closesocket(SOCKET s)�������ǹر�ָ����socket�����һ��������е���Դ��
				//int shutdown(SOCKET s, int how)���ǽ�ֹ��ָ����socket s�Ͻ�ֹ������howָ���Ĳ���������������Դ���л��գ�shutdown֮���closesocket֮ǰs�������ٴ�connect����WSAConnect.
				//int WSASendDisconnect(SOCKET s, LPWSABUF lpOutboundDisconnectData)���shutdown�������ƣ����в�ͬ�ľ���WSASendDisconnect��������һ��lpOutboundDisconnectData���������������͡��Ͽ����ݡ���disconnect data��.��MSDN��д�ˡ�The native implementation of TCP / IP on Windows does not support disconnect data.��������һ�����Ǿ���shutdown���������ˡ�
				closesocket(clientSocket);
				return;
			}

		}

		//shutdown(clientSocket, SD_SEND);
		closesocket(clientSocket);
	}

	void SetAccept(const SOCKET serverSocket, sockaddr* their_addr, SOCKET& newSocket, char* IPdotdec, bool& bIsAcceptSuccess)
	{
		int sin_size = sizeof(struct sockaddr_in);
		//accept  �������ģ������Ӧ��IPû�������������ͻ�����һֱ��ס����
		//�벻��ס��������һ���߳�ר������������������
		//���ߵ����һ�ε��� socket() �� ���׽�����������ʱ���ں˾ͽ�������Ϊ����������㲻���׽��������� ���Ҫ���ú��� fcntl() �����ⲻ�Ǻð취��
		//���������ĳ�����æ��״ ̬��ѯ�׽��ֵ����ݣ��㽫�˷Ѵ����� CPU ʱ�䡣���õĽ��֮������ select() ȥ��ѯ�Ƿ�������Ҫ������
		//cout << "���߳�ID: " << GetCurrentThreadId() << endl;
		newSocket = accept(serverSocket, their_addr, &sin_size);		//��ʱ�������߳��У������߳���һֱ��������������
		
		if (newSocket == -1)
		{
			//cout << "serverSocket: " << serverSocket << endl;
			//cout << "acceptʧ�ܣ�����" << endl;
			bIsAcceptSuccess = false;
			//cout << "�뿪���߳�---���߳�ID�� " << GetCurrentThreadId() << endl;
			return;
		}

		bIsAcceptSuccess = true;
		printf("server: ���ӳɹ����� %s\n", IPdotdec);
		cout << "accept    sck_cliend: " << newSocket << endl;
		//cout<<"�뿪���߳�---���߳�ID�� "<< GetCurrentThreadId() << endl;

	}

	//��������accept�̣߳���ִ��
	void CreateListenAccepThread(const SOCKET serverSocket, sockaddr * their_addr, SOCKET& newSocket, char* IPdotdec, bool& bIsAcceptSuccess)
	{
		//��׼Լ��std::thread����ʱ�������󴫵�ʵ�ʲ����Ŀ�����֧���ƶ����壩��������ת��ʵ�ʲ����������ͨ����ӡʵ�ʲ�������ʽ�����ĵ�ַ�������һ��Ϊ������fun����ʽ�����Ǳ�a�Ŀ�����ʼ���ģ�������ʽ����a������main������a�����ã���fun�ڸ�ֵ��Ȼ����ı�main������a��ֵ��
		//�����ϣ���������õķ�ʽ���Ρ������ý������֮һ��ʹ��std::ref����std::thread(fun, std::ref(a)); !!!!!!!!!
		std::thread thrd(&Common::SetAccept, serverSocket, their_addr, std::ref(newSocket), IPdotdec, std::ref(bIsAcceptSuccess));		//C++�е�thread�Ĳ�������ֱ��Ϊ����
		thrd.detach();
	}

}