// 堆排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <Windows.h>

using namespace std;

/*
	堆排序： 将一组数据放在数组中，按照完全二叉树的形式排列。堆排序分为“大根堆”和“小根堆”。堆排序是不稳定的！！！ 堆排序的平均复杂度为nlogn,最好和最差时的时间复杂度都是nlogn
	大根堆： 即根节点>=子节点，最大的值在根节点。
	小根堆： 即根节点<=子节点，最小的值在根节点。

	堆排序的实现原理（以大根堆为例）： 
		1. 初始化大根堆，使当前树满足大根堆的定义。
		2. 将根节点与最后一个叶子节点互换位置， int temp = A[0]; A[0] = A[n-1]; A[n-1] = temp;
		3. 再将剩余A[0]~A[n-2]按照1、2步骤进行排序，直到数组元素只剩1个元素时结束

		注： A[i]的左孩子为A[2i+1],右孩子为A[2i+2]

	大根堆的实现过程：
		前期需要理解的： 假设一组数据的长度为n，则将此组数据依次存放于数组A中。 设节点的位置在数组中的下标为x，则A[i]的左孩子为A[2i+1]，A[i]的右孩子为A[2i+2]，（0<=i<=n/2-1）！！！！！
		1. 调用函数AdjustHeap（）初始化堆，使堆满足大根堆的条件。
		2. 将堆的根节点元素与最后一个（当前循环中的最后一个）叶子节点元素位置互换
		3. 执行1、2，直到数组元素个数只有1个结束
*/

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
	for (int i=size/2 -1; i>=0; i--)
	{
		//从最小的子树开始构造
		AdjustHeap(array, size, i);
	}

	//堆的根节点和堆的最后一个叶子节点交换元素交换元素
	while (size)
	{
		T temp = array[size-1];
		array[size - 1] = array[0];
		array[0] = temp;

		size--;
		//再调整（因为就一个堆根节点值改变，所以至上而下调整大根堆）
		AdjustHeap(array, size, 0);
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

int main()
{
	int a[20] = { 0 };
	int b[20] = { 0 };
	int size = sizeof(a) / sizeof(a[0]);
	for (int i = 0; i<size; i++)
	{
		a[i] = rand() % 100;
		b[i] = a[i];
	}

	cout << "排序前： "<<endl;
	for (int i = 0; i<size; i++)
	{
		cout << b[i] << " " ;
	}

	//获取程序运行时间
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	QuickSort(a, 0, size);
	t2 = GetTickCount();
	cout << endl << "快速排序所花时间： " << t2 - t1 << "ms" << endl;

	t3 = GetTickCount();
	HeapSort(b, size);

	t4 = GetTickCount();


	cout << endl << "堆排序所花时间： " << t4 - t3 << "ms" << endl;

	cout << endl << "排序后： " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << b[i] << " " ;
	}

	system("pause");

    return 0;
}

