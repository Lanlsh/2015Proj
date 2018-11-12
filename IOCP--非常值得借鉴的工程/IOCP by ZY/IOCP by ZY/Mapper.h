#ifndef __IO_MAPPER__
#define __IO_MAPPER__

#define net_msg

/*
__declspec(novtable) ��C++�нӿ��й㷺Ӧ��. �����׿���������Ϊ�ںܶ�ط������������
Ϊ�˺�. ����˵ATL�ģ����е�ATL_NO_VTABLE, ��ʵ����__declspec(novtable).

__declspec(novtable) �������಻Ҫ���麯�����Լ����麯����ĳ�ʼ������, �������Խ�ʡ��
��ʱ��Ϳռ�.���������һ������������ʵ��, ��Ϊû���麯����, ���޷����麯�����е���.

���, __declspec(novtable)һ����Ӧ���ڽӿ�(��ʵ���ǰ������麯������), ��Ϊ�ӿڰ����Ķ�
�Ǵ��麯��,����������ʵ��. ���ǰ� __declspec(novtable)Ӧ�õ��ӿ�����, ��Щ�ӿ���Ͳ���
�����麯����ͳ�ʼ���麯����Ĵ�����. ������������Լ������Լ����麯����ͳ�ʼ������.

*/

class __declspec(novtable) CIOMessageMap
{
public: 
	virtual bool ProcessIOMessage(IOType clientIO, PER_IO_CONTEXT* pIOContext, DWORD dwSize) = 0;
};

#define BEGIN_IO_MSG_MAP() \
public: \
	bool ProcessIOMessage(IOType clientIO, PER_IO_CONTEXT* pIOContext, DWORD dwSize) \
	{ \
		bool bRet = false;

#define IO_MESSAGE_HANDLER(msg, func) \
		if(msg == clientIO) \
			bRet = func(pIOContext, dwSize);

#define END_IO_MSG_MAP() \
		return bRet; \
	}

#endif // !__IO_MAPPER__
