// 希尔排序.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

/*
	希尔排序（shell's sort）：是“插入排序”的一种又称为“缩小增量排序（Diminishing Increment Sort）”,是直接插入排序算法的一种更高效的改进版本。希尔排序是“非稳定排序算法”！！
	希尔排序稳定性解析： 由于多次插入排序，我们知道一次插入排序是稳定的，不会改变相同元素的相对顺序，但在不同的插入排序过程中，相同的元素可能在各自的插入排序中移动，最后稳定性就被打乱！！

	希尔排序是把记录按下标的一定增量分组，对每组使用直接插入排序算法排序；随着增量逐渐减少，每组包含的关键词越来越多，当增量减至1时，整个文件恰被分成1组，算法便终止！

	希尔排序平均时间复杂度为n的1.3次方，最坏时为n的平方，最好时为n！！

	希尔排序的效率跟所选的“增量序列”密密相关，但如何选出最优的增量数组还有未知！！！

	希尔排序思想：
		1. 先取小于n的整数d1作为第一增量，把文件的全部记录分组。所有距离为d1的倍数放在同一组中。先在各组内进行直接插入排序。
		2. 取第二个增量d2<d1重复1，直到所有的增量dt = 1(dt<d(t-1)...<d2<d1),即所有记录放在同一组中进行直接插入排序为止。

*/

template<typename T>
void Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}


/*
	Des：希尔排序算法
	Param:
*/
template<typename T>
void ShellSort(T array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	//自定义一组增量序列(一般为size的一半的一半...直到1)  //想要达到效果就一定要有一组合适的自定义增量序列！！！！  --这也就决定了此算法不可能去排序大规模数据量，只试用与中小型数据，而且得找到此增量序列！！！
	vector<int> incrementArray;
	incrementArray.push_back(5);
	incrementArray.push_back(3);
	incrementArray.push_back(2);
	incrementArray.push_back(1);

	//根据增量序列开始希尔排序
	for (int i= 0; i<incrementArray.size(); i++)
	{
		for (int k=0; k<size; k++)
		{
			for (int j = k; j<size; j = j + incrementArray[i])
			{
				int index = j + incrementArray[i];
				if (index >= size)
				{
					break;
				}

				if (array[j] > array[index])
				{
					Swap(array[j], array[index]);
				}
			}
		}

	}

}

int main()
{
	int a[10] = { 0 };
	int size = sizeof(a) / sizeof(a[0]);
	for (int i=0; i<size; i++)
	{
		a[i] = rand() % 100;
	}

	cout << "排序前： " << endl;
	for (int i=0; i<size; i++)
	{
		cout << a[i] << " ";
	}

	ShellSort(a, size);

	cout <<endl<< "排序后： " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << a[i] << " ";
	}

	system("pause");

    return 0;
}

