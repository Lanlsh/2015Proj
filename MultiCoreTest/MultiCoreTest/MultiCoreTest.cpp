// MultiCoreTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <thread>

using namespace std;

const int printValue = 10000000;
const int exitValue = 100000000;

void Thread_1()
{
	cout << endl<<"启动第1条线程++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  第1条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "   第1条线程--------------------------退出" << endl;
			break;
		}
	}
}

void Thread_2()
{
	cout << endl << "启动第2条线程++++++++++++++++++" << endl;
	int j = 0;
	j++;
	while (true)
	{
		j++;
		if (j % printValue == 0)
		{
			cout << "    第2条线程: " << j << endl;
		}

		if (j == exitValue)
		{
			cout << "第2条线程--------------------退出" << endl;
			break;
		}
	}
}

void Thread_3()
{
	cout << endl << "启动第3条线程+++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  第3条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第3条线程--------------------退出" << endl;
			break;
		}
	}
}

void Thread_4()
{
	cout << endl << "启动第4条线程+++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    第4条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第4条线程--------------------------退出" << endl;
			break;
		}
	}
}

void Thread_5()
{
	cout << endl << "启动第5条线程++++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   第5条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第5条线程---------------------退出" << endl;
			break;
		}
	}
}

void Thread_6()
{
	cout << endl << "启动第6条线程++++++++++++++++++" << endl;
	int i = 0;
	
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   第6条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第6条线程-----------------------退出" << endl;
			break;
		}
	}
}

void Thread_7()
{
	cout << endl << "启动第7条线程++++++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    第7条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第7条线程----------------------退出" << endl;
			break;
		}
	}
}

void Thread_8()
{
	cout << endl << "启动第8条线程+++++++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    第8条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第8条线程---------------------退出" << endl;
			break;
		}
	}
}


void Thread_9()
{
	cout << endl << "启动第9条线程+++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   第9条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第9条线程--------------------退出" << endl;
			break;
		}
	}
}

void Thread_10()
{
	cout << endl << "启动第10条线程+++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   第10条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第10条线程---------------退出" << endl;
			break;
		}
	}
}

void Thread_11()
{
	cout << endl << "启动第11条线程+++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  第11条线程: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "第11条线程---------------退出" << endl;
			break;
		}
	}
}

int main()
{
	std::thread thrd1(&Thread_1);
	std::thread thrd2(&Thread_2);
	std::thread thrd3(&Thread_3);
	std::thread thrd4(&Thread_4);
	std::thread thrd5(&Thread_5);
	std::thread thrd6(&Thread_6);
	std::thread thrd7(&Thread_7);
	std::thread thrd8(&Thread_8);
	std::thread thrd9(&Thread_9);
	std::thread thrd10(&Thread_10);
	std::thread thrd11(&Thread_11);

	thrd1.detach();
	thrd2.detach();
	thrd3.detach();
	thrd4.detach();
	thrd5.detach();
	thrd6.detach();
	thrd7.detach();
	thrd8.detach();
	thrd9.detach();
	thrd10.detach();
	thrd11.detach();

	system("pause");
    return 0;
}

