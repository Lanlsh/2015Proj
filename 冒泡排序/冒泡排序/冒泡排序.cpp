// 冒泡排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

/*
Desc: 冒泡排序算法排序数组
Param: int array[]  传入的数组
Param: int arraySize 传入的数组的长度
*/
template<typename T>
void BubbleSort(T array[], int arraySize)
{
	if ((array == NULL) || (arraySize <= 1))
	{
		return ;
	}

	for (int i=0; i<arraySize-1; i++)
	{
		for (int j=0; j<arraySize-1-i; j++)
		{
			int temp = array[j];

			if (array[j] > array[j + 1])
			{
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
		}

	}
};


int main()
{
	int array[100000] = {0};
	int arraySize = sizeof(array) / sizeof(array[0]);
	cout << "排序前：" << endl;
	//for (int i=0; i<arraySize; i++)
	//{
	//	array[i] = rand()%10000;
	//	cout << array[i] << " ";
	//}

	//cout << endl;
	//cout << "排序后：" << endl;

	//获取程序运行时间
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	BubbleSort(array, arraySize);

	t2 = GetTickCount();

	//for (int i = 0; i<arraySize; i++)
	//{
	//	cout << array[i] << " ";
	//}



	cout << "运行时间： " << t2 - t1 <<"毫秒"<< endl;


	system("pause");
    return 0;
}

