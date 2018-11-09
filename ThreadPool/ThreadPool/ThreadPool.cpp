// ThreadPool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TaskDeliveryManager.h"

#define SLEEPTIME 0

int threadArray[100] = {0};
int recordCount[100] = { 0 };

int dealTaskCount = 0;

mutex mtx_dealTC;

void Count()
{
	mtx_dealTC.lock();
	dealTaskCount++;
	mtx_dealTC.unlock();
}

void Fun_1()
{
	int threadId = GetCurrentThreadId();
	//cout << endl << threadId << ": Say 11111111111" << endl;
	Count();
	bool bIsFind = false;
	int idx = 0;
	for (int i=0; i<100; i++)
	{
		if (threadArray[i] == threadId)
		{
			recordCount[i]++;
			bIsFind = true;
			break;
		}
	}

	if (!bIsFind)
	{
		for (int i = 0; i < 100; i++)
		{
			if (threadArray[i] == 0)
			{
				threadArray[i] = threadId;
				recordCount[i]++;
				break;
			}
		}
	}

	Sleep(SLEEPTIME);
}

void Fun_2()
{
	int threadId = GetCurrentThreadId();
	//cout << endl << threadId << ": Say 222222222222222" << endl;
	Count();
	bool bIsFind = false;
	int idx = 0;
	for (int i = 0; i < 100; i++)
	{
		if (threadArray[i] == threadId)
		{
			recordCount[i]++;
			bIsFind = true;
			break;
		}
	}

	if (!bIsFind)
	{
		for (int i = 0; i < 100; i++)
		{
			if (threadArray[i] == 0)
			{
				threadArray[i] = threadId;
				recordCount[i]++;
				break;
			}
		}
	}
	Sleep(SLEEPTIME);
}

void Fun_3()
{
	int threadId = GetCurrentThreadId();
	//cout <<endl<< threadId << ": Say 3333333333333" << endl;
	Count();
	bool bIsFind = false;
	int idx = 0;
	for (int i = 0; i < 100; i++)
	{
		if (threadArray[i] == threadId)
		{
			recordCount[i]++;
			bIsFind = true;
			break;
		}
	}

	if (!bIsFind)
	{
		for (int i = 0; i < 10; i++)
		{
			if (threadArray[i] == 0)
			{
				threadArray[i] = threadId;
				recordCount[i]++;
				break;
			}
		}
	}
	Sleep(SLEEPTIME);
}

void Fun_4()
{
	int threadId = GetCurrentThreadId();
	//cout << endl << threadId << ": Say 444444444444444" << endl;
	Count();
	bool bIsFind = false;
	int idx = 0;
	for (int i = 0; i < 100; i++)
	{
		if (threadArray[i] == threadId)
		{
			recordCount[i]++;
			bIsFind = true;
			break;
		}
	}

	if (!bIsFind)
	{
		for (int i = 0; i < 10; i++)
		{
			if (threadArray[i] == 0)
			{
				threadArray[i] = threadId;
				recordCount[i]++;
				break;
			}
		}
	}
	Sleep(SLEEPTIME);
}

void Say()
{
	cout << "Hello" << endl;
}

TaskDeliveryManager* pTaskManager = NULL;

void CreateTask()
{
	for (int i = 0; i < 300000; i++)
	{
		switch (i % 4)
		{
		case 0:
			pTaskManager->AddTask(Fun_1);
			break;
		case 1:
			pTaskManager->AddTask(Fun_2);
			break;
		case 2:
			pTaskManager->AddTask(Fun_3);
			break;
		case 3:
			pTaskManager->AddTask(Fun_4);
			break;
		}
	}

}

void StopTask()
{
	
	while (1)
	{
		//char cY;
		//cin >> cY;
		//if (cY == 'y')
		//{
			Sleep(1000);
			pTaskManager->StopTask();
			for (int i = 0; i < 10; i++)
			{
				if (threadArray[i] == 0)
				{
					break;
				}
				cout << endl << threadArray[i] << ": " << recordCount[i] << endl;
			}

			cout<<endl<<"dealTaskCount: "<< dealTaskCount<<endl;

			break;
	/*	}*/

		//Sleep(1000);

	}

}

int main()
{
	pTaskManager = new TaskDeliveryManager();

	std::thread td_CreatTask(CreateTask);
	td_CreatTask.detach();

	std::thread td_Iuput(StopTask);
	td_Iuput.detach();

	pTaskManager->RunTask();

	delete pTaskManager;
	pTaskManager = NULL;

	system("pause");

    return 0;
}

