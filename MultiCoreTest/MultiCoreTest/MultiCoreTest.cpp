// MultiCoreTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <thread>

using namespace std;

const int printValue = 10000000;
const int exitValue = 100000000;

void Thread_1()
{
	cout << endl<<"������1���߳�++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  ��1���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "   ��1���߳�--------------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_2()
{
	cout << endl << "������2���߳�++++++++++++++++++" << endl;
	int j = 0;
	j++;
	while (true)
	{
		j++;
		if (j % printValue == 0)
		{
			cout << "    ��2���߳�: " << j << endl;
		}

		if (j == exitValue)
		{
			cout << "��2���߳�--------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_3()
{
	cout << endl << "������3���߳�+++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  ��3���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��3���߳�--------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_4()
{
	cout << endl << "������4���߳�+++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    ��4���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��4���߳�--------------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_5()
{
	cout << endl << "������5���߳�++++++++++++++++++++++++" << endl;
	int i = 0;
	i++;
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   ��5���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��5���߳�---------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_6()
{
	cout << endl << "������6���߳�++++++++++++++++++" << endl;
	int i = 0;
	
	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   ��6���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��6���߳�-----------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_7()
{
	cout << endl << "������7���߳�++++++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    ��7���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��7���߳�----------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_8()
{
	cout << endl << "������8���߳�+++++++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "    ��8���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��8���߳�---------------------�˳�" << endl;
			break;
		}
	}
}


void Thread_9()
{
	cout << endl << "������9���߳�+++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   ��9���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��9���߳�--------------------�˳�" << endl;
			break;
		}
	}
}

void Thread_10()
{
	cout << endl << "������10���߳�+++++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "   ��10���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��10���߳�---------------�˳�" << endl;
			break;
		}
	}
}

void Thread_11()
{
	cout << endl << "������11���߳�+++++++++++++" << endl;
	int i = 0;

	while (true)
	{
		i++;
		if (i % printValue == 0)
		{
			cout << "  ��11���߳�: " << i << endl;
		}

		if (i == exitValue)
		{
			cout << "��11���߳�---------------�˳�" << endl;
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

