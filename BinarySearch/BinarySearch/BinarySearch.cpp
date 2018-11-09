// BinarySearch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*
	二分查找法（Binary Search），又称为“折半查找法”，它是一种效率较高的查找方法。但是，折半查找法要求线性表必须采用顺序存储结构，而且表中元素按关键字有序排序
	
	使用此查找法前提：
		1. 查找的存储结构必须为顺序存储结构，即按顺序依次存储元素的结构
		2. 该线性表中的元素必须为“已排序”。

	二分查找法的时间复杂度：
		1. 二分查找法的基本思想就是不断比较表最中间的值，然后将表的范围分半去搜索，所以时间复杂度就是while循环的次数！！
		2. 总共为n个元素，渐渐跟下去就是n, n/2, n/4,....n/(2的k次方), 其中k是循环的次数。由于n/(2的k次方)取整后要>=1,即搜索到最后的字表最多为1个元素，所以，
	令n/(2的k次方) = 1，可得"k = log2n",所以时间复杂度可以表示为O(log2n)。 
		3. 根据1、2可得，二分查找法时间复杂度最快为O(1), 最慢为O(log2n)。

	查找过程（假设表中的元素是按升序排列的）：
		1. 首先选取表中间的元素a[n/2]与需要查询的值x进行比较，如果量值相等，表明已找到，退出搜索
		2. 若没有找到，将表按照n/2的前半部和后半部分为两个子表a1、a2，同时比较x是大于a[n/2]还是小于a[n/2]
		3. 若x小于a[n/2],则表a1作为下一轮排序表；若x大于a[n/2]，则表a2作为下一轮排序表
		4. 重复1、2、3，直到找到元素或者字表不存在为止。
*/

/*
	Desc: 折半查找法（Binary Search）
	Param: array 查询的线性表
	Param: searchValue 需要查询的值
	Param：size 线性表array的长度
	return: 返回值为查找到的元素在数组中的下标，如果没有没有找到，返回-1
*/
template<typename T>
int BinarySearch(T array[], T searchValue, int size)
{
	if ((array == NULL) || (size <= 0))
	{
		return -1;
	}

	int high = size - 1; //查询的终止位置
	int low = 0;	//查询的起始位置
	int middle = low + (high - low) / 2; //查询的子表中的中间值的位置

	while ((high - low) >= 0)
	{
		//刚好要找的值为字表中的中间值
		if (array[middle] == searchValue)
		{
			return middle;
		}

		//若不为子表中的中间值，则判断为哪一字表
		if (searchValue < array[middle])
		{
			high = middle - 1;
			middle = low + (high - low) / 2;
		}
		else
		{
			low = middle + 1;
			middle = low + (high - low) / 2;
		}
	}

	return -1;
}

template<typename T>
void QuickSort(T array[], int startIndex, int endIndex)
{
	if (array == NULL)
	{
		return;
	}

	if ((endIndex - startIndex) <= 0)
	{
		return;
	}

	T element = array[startIndex];
	int tempStartIndex = startIndex;
	int tempEndIndex = endIndex;
	while (tempEndIndex > tempStartIndex)
	{
		//从子表尾往前搜索，找到第一个小等于element的下标
		while (tempEndIndex > tempStartIndex)
		{
			if (array[tempEndIndex] <= element)
			{
				array[tempStartIndex] = array[tempEndIndex];
				break;
			}

			tempEndIndex--;
		}

		//从子表头往后搜索，找到第一个大于element的下标
		while (tempStartIndex < tempEndIndex)
		{
			if (array[tempStartIndex] > element)
			{
				array[tempEndIndex] = array[tempStartIndex];
				break;
			}

			tempStartIndex++;
		}

	}

	array[tempStartIndex] = element;

	QuickSort(array, startIndex, tempStartIndex - 1);
	QuickSort(array, tempStartIndex+1, endIndex);
}

int main()
{
	int array[20] = {0};
	int size = sizeof(array) / sizeof(array[0]);
	for (int i = 0; i<size; i++)
	{
		array[i] = rand()%100;
	}

	//排序
	QuickSort(array, 0, size - 1);

	cout << "线性表信息如下： " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << array[i] << " ";
	}
	
	int biaozhi = 1;
	while (biaozhi)
	{
		int data = 0;
		cout <<endl<< "请输入查询数据： " << endl;
		cin >> data;


		int index = BinarySearch(array, data, size);

		cout << endl << "查询结果如下： " << endl;
		cout << "数据在线性表中的下标为：" << index << endl;

		system("pause");

		cout << endl << "是否还需要查询(1继续，0退出): ";
		cin >> biaozhi;
	}


    return 0;
}

