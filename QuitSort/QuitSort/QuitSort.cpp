// QuitSort.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;

/*
快速排序法： 快速排序法是目前平均效率最快的排序算法，最快为nlogn, 最慢为n平方

快速排序法的实现原理：选取数组中的一个数据（一般为数组第一个元素），将数组中大于该元素的数据放在它的后面，小于“或”等于它的数据放在它的前面（所以快牌是不稳定的），即完成一趟快速排序
	1. 设数组A的长度为n，设置变量i=0, j=n-1.
	2. 设A[0]的数据为参考数据k，将数组从后往前搜索（j--），搜索到第一个小于或等于A[0]的数据，交换数据A[i] = A[j], A[j] = k;
	3. 将数组从前往后搜索（i++）,搜索到第一个大于k的数据，交换数据A[i] = A[j], A[j] = k;
	4. 循环2、3，直到i==j
	5. 此时将小于k的值作为一个数组，大于k的作为另外一个数组，再执行2、3、4，直到搜索循环数组的长度为1结束

*/

/*
	Des: 快速排序算法
	Param: array 数组
	Param: start 数组的排序起始位置
	Param: end 数组的排序终点位置
*/
template<typename T>
void QuickSort(T array[], int start, int end)
{
	if (start >= end)
		return;

	T key = array[start];
	int startIndex = start;
	int endIndex = end;
	
	while (startIndex < endIndex)
	{
		//将数组从后往前搜索，找到第一个小等于key的值
		while ((array[endIndex] > key) && (start < endIndex))
		{
			endIndex--;
		}

		array[startIndex] = array[endIndex];
		//startIndex = end;

		//将数组从前往后搜索，找到第一个大于key的值
		while((array[startIndex] <= key) && (startIndex < endIndex))
		{
			startIndex++;
		}

		array[endIndex] = array[startIndex];
	}

	//记录轴的值
	array[startIndex] = key;

	//排序剩余的数据
	QuickSort(array, start, startIndex - 1);
	QuickSort(array, startIndex + 1, end);
}


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
		return;
	}

	for (int i = 0; i<arraySize - 1; i++)
	{
		for (int j = 0; j<arraySize - 1 - i; j++)
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
	int a[100000] = { 0 };
	int b[100000] = { 0 };
	int size = sizeof(a)/sizeof(a[0]);
	for (int i=0; i<size; i++)
	{
		a[i] = rand() % 10000;
		b[i] = a[i];
	}

	//cout << "排序前： "<<endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " " ;
	//}

	//获取程序运行时间
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	QuickSort(a, 0, size-1);
	t2 = GetTickCount();
	cout << endl << "快速排序所花时间： "<< t2-t1<<"ms"<< endl;

	t3 = GetTickCount();
	BubbleSort(b, size);
	t4 = GetTickCount();

	cout << endl << "冒泡排序所花时间： " << t4 - t3 << "ms" << endl;

	//cout << endl << "排序后： " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " " ;
	//}

	system("pause");

    return 0;
}

