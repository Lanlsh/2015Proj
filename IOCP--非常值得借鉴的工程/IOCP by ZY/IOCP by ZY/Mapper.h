#ifndef __IO_MAPPER__
#define __IO_MAPPER__

#define net_msg

/*
__declspec(novtable) 在C++中接口中广泛应用. 不容易看到它是因为在很多地方它都被定义成
为了宏. 比如说ATL活动模板库中的ATL_NO_VTABLE, 其实就是__declspec(novtable).

__declspec(novtable) 就是让类不要有虚函数表以及对虚函数表的初始化代码, 这样可以节省运
行时间和空间.但是这个类一定不允许生成实例, 因为没有虚函数表, 就无法对虚函数进行调用.

因此, __declspec(novtable)一般是应用于接口(其实就是包含纯虚函数的类), 因为接口包含的都
是纯虚函数,不可能生成实例. 我们把 __declspec(novtable)应用到接口类中, 这些接口类就不用
包含虚函数表和初始化虚函数表的代码了. 它的派生类会自己包含自己的虚函数表和初始化代码.

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
