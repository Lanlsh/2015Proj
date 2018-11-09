// 归并排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include "windows.h"

using namespace std;


/*
	归并排序（Merge Sort）：归并排序是建立在归并操作上的一种有效的排序算法，该算法是采用分治法（Divide and Conquer）的一个非常典型的应用。将已有的子序列合并，得到完全有序的
序列；即先使每个子序列有序，再使子序列段间有序。若将两个有序表合并成一个有序表，称为“二路归并”。

	归并排序是“稳定的排序”，即相等的元素的顺序不会发生改变。

	特点：
		1. 归并排序速度“仅次于“快速排序（不稳定）””，为稳定排序算法，一般用于对总体无序，但各子项相对有序的数列。
		2. 归并排序的比较次数小于快速排序的比较次数，移动次数一般多于快速排序的移动次数！！！

	归并排序的时间复杂度为：nlogn, 最好和最差都是nlogn。  //稳定的！！！！

	归并排序的思路：
		1. 对于一组数列，先将相邻两个数字进行归并操作（merge），形成floor(n/2)个序列，排序后每个序列包含两个元素（可能也为1个元素）
		2. 将1序列的子序列再次归并，形成floor(n/4)个序列，排序后每个序列包含4个元素(可能为3个元素（奇数数列时）)。
		3. 重复2，知道所有子序列合并成一个序列
*/


/*
	Des: 归并算法： 用于归并两个子序列
	Param: array 排列数组
	Param: start_1	第一列子段的初始位置
	Param: end_1	第一列子段的末位置
	Param: start_2	第二列子段的初始位置
	Param: end_2	第二列子段的末位置
	Param: size		整个数组的长度
*/
template<typename T>
void Merge(T array[], int start_1, int end_1, int start_2, int end_2, int size)
{
	if (start_2 >= size)
	{
		return;
	}

	//定义一个临时的缓存数组，用于存放归并后的值
	int length = end_2 - start_1 + 1;
	T* tempArray = new T[length];
	memset(tempArray, 0, length * sizeof(T));

	//先定义一个数组，长度为end_1-start_1，用于存放排序后的值
	memset(tempArray, 0, sizeof(T)*length);

	//比较两子序列，将较小值先放于临时数组
	int x = start_1;	//用于标记序列1中剩余元素的起始下标
	int y = start_2;	//用于标记序列2中剩余元素的起始下标
	int index_temp = 0;		//用于标记temp已记录的数量

	while ((y <= end_2) && (y < size) && (x <= end_1))
	{
		if (array[x] <= array[y])
		{
			tempArray[index_temp++] = array[x++];
		}
		else
		{
			tempArray[index_temp++] = array[y++];
		}
	}

	//检测是否还有未加到临时数组的元素，加入到临时数组中
	if (x <= end_1)
	{
		while (x <= end_1)
		{
			tempArray[index_temp++] = array[x++];
		}
	}
	else if (y <= end_2)
	{
		while ((y <= end_2) && (y < size))
		{
			tempArray[index_temp++] = array[y++];
		}
	}

	//将排序好的临时数组的变量copy到array中
	for (int i = start_1; i<=end_2; i++)
	{
		if (i >= size)
		{
			break;
		}

		array[i] = tempArray[i - start_1];
	}

	delete[] tempArray;
	tempArray = NULL;
}

/*
	Des：Merge Sort
	Param: array 排序数组
	Param: size	要排序的长度
*/
template<typename T>
void MergeSort(T array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	//先进行最小子序列排序
	int startIndex = 0;
	int endIndex = startIndex + 1;
	while (endIndex < size)
	{
		if (array[startIndex] > array[endIndex])
		{
			T temp = array[startIndex];
			array[startIndex] = array[endIndex];
			array[endIndex] = temp;
		}

		startIndex = endIndex + 1;
		endIndex = startIndex + 1;
	}

	//合并子序列
	int n = 2;	//子序列的长度
	int start_1 = 0;
	int end_1 = start_1 + n - 1;
	int start_2 = end_1 + 1;
	int end_2 = start_2 + n - 1;
	while (start_2 < size)
	{
		int tempStart_1 = start_1;
		int tempEnd_1 = end_1;
		int tempStart_2 = start_2;
		int tempEnd_2 = end_2;
		while (tempStart_2 < size)
		{
			Merge(array, tempStart_1, tempEnd_1, tempStart_2, tempEnd_2, size);

			tempStart_1 = tempEnd_2 + 1;
			tempEnd_1 = tempStart_1 + n - 1;
			tempStart_2 = tempEnd_1 + 1;
			tempEnd_2 = tempStart_2 + n - 1;
		}

		n = n * 2;
		start_1 = 0;
		end_1 = start_1 + n - 1;
		start_2 = end_1 + 1;
		end_2 = start_2 + n - 1;
	}
}

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
		while ((array[startIndex] <= key) && (startIndex < endIndex))
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
Des: 构建大根堆算法
Param：array  需要排序的数组
Param：size 需要排序的数组的长度
Param：element “当前”需要构成“大根堆的根节点”在数组中的下标
*/

template<typename T>
void AdjustHeap(T array[], int size, int element)
{
	/*
	在此调整的大根堆element的子树应该为已经调整好的大根堆
	*/
	int pIndex = element;
	int lIndex = 2 * pIndex + 1;
	int rIndex = 2 * pIndex + 2;

	//使节点element为根节点，让这子树符合大顶堆条件
	while (lIndex < size)
	{
		//如果有父节点
		if (rIndex < size)
		{
			//如果父节点>=左孩子 && 父节点>=右孩子，则跳出循环
			if ((array[pIndex] >= array[lIndex]) && (array[pIndex] >= array[rIndex]))
			{
				return;
			}

			//先比较左右孩子，取最大的孩子于父节点比较，如果父节点<该孩子，则与该孩子交换位置！！！（这样就只改变了一颗子树，只需要将此子树调整下就OK！！！）
			if (array[lIndex] >= array[rIndex])
			{
				//判断是否大等于左子树，如果小于左子树，则交换位置
				if (array[pIndex] < array[lIndex])
				{
					T temp = array[pIndex];
					array[pIndex] = array[lIndex];
					array[lIndex] = temp;

					//重点！！！！！
					pIndex = lIndex;
				}
			}
			else
			{
				//判断是否大等于右子树，如果小于右子树，则交换位置
				if (array[pIndex] < array[rIndex])
				{
					T temp = array[pIndex];
					array[pIndex] = array[rIndex];
					array[rIndex] = temp;

					//重点！！！！！
					pIndex = rIndex;
				}
			}
		}
		else
		{
			//如果父节点>=左孩子, 则跳出循环
			if (array[pIndex] >= array[lIndex])
			{
				return;
			}

			//判断是否大等于左子树，如果小于左子树，则交换位置
			if (array[pIndex] < array[lIndex])
			{
				T temp = array[pIndex];
				array[pIndex] = array[lIndex];
				array[lIndex] = temp;

				//重点！！！！！
				pIndex = lIndex;
			}
		}


		//重新
		lIndex = 2 * pIndex + 1;
		rIndex = 2 * pIndex + 2;
	}

}

/*
Des: 堆排序算法
Param：array  需要排序的数组
Param：size 需要排序的数组的长度
*/
template<typename T>
void HeapSort(T array[], int size)
{
	if (size <= 1)
	{
		return;
	}

	//先构造一颗符合大顶堆的树
	for (int i = size / 2 - 1; i >= 0; i--)
	{
		//从最小的子树开始构造
		AdjustHeap(array, size, i);
	}

	//堆的根节点和堆的最后一个叶子节点交换元素交换元素
	while (size)
	{
		T temp = array[size - 1];
		array[size - 1] = array[0];
		array[0] = temp;

		size--;
		//再调整（因为就一个堆根节点值改变，所以至上而下调整大根堆）
		AdjustHeap(array, size, 0);
	}
}

template<typename T>
void BoubleSort(T array[], int arraySize)
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
}

int main()
{
	int a[10000000] = { 0 };
	int b[10000000] = { 0 };
	int c[10000000] = { 0 };
	int d[10000000] = { 0 };
	int size = sizeof(a) / sizeof(a[0]);
	for (int i=0; i<size; i++)
	{
		a[i] = rand() % 100000;
		b[i] = a[i];
		c[i] = a[i];
		d[i] = a[i];
	}

	cout <<endl<< "数据量： " << size << endl << endl;
	//cout << "排序前： " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " ";
	//}
	//获取程序运行时间
	DWORD t1, t2, t3, t4, t5, t6, t7, t8;

	t1 = GetTickCount();
	QuickSort(a, 0, size - 1);
	t2 = GetTickCount();
	cout << endl << "快速排序所花时间： " << t2 - t1 << "ms" << endl;

	t3 = GetTickCount();
	HeapSort(b, size);
	t4 = GetTickCount();

	cout << endl << "堆排序所花时间： " << t4 - t3 << "ms" << endl;

	t5 = GetTickCount();
	MergeSort(c, size);
	t6 = GetTickCount();

	cout << endl << "归并排序所花时间： " << t6 - t5 << "ms" << endl;

	//t7 = GetTickCount();
	//BoubleSort(d, size);
	//t8 = GetTickCount();

	//cout << endl << "冒泡排序所花时间： " << t8 - t7 << "ms" << endl;

	//cout << endl << "排序后： " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << c[i] << " ";
	//}

	system("pause");

    return 0;
}

