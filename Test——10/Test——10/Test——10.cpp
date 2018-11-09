// Test——10.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <windows.h>
using namespace std;

typedef void(*Task)();

void Fun()
{
	cout << "66666666666666" << endl;
}

class A
{
public:
	A()
	{
		cout << "ssssss" << endl;
	}

	~A()
	{
		cout << "9999999999999" << endl;
	}

	void SetTask(Task task)
	{
		m_task = task;
		std::thread td(&A::RunTask, this);
		td.detach();
	}

	void RunTask()
	{
		Sleep(2000);
		cout << "888888888888" << endl;
		m_task();
	}

private:
	Task m_task;
};

int main()
{
	A a;
	Task task = Fun;
	a.SetTask(task);
	task = NULL;
	cout << "000000000000000" << endl;
	system("pause");
	return 0;
}

