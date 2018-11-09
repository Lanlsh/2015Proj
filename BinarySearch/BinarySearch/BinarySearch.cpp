// BinarySearch.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/*
	���ֲ��ҷ���Binary Search�����ֳ�Ϊ���۰���ҷ���������һ��Ч�ʽϸߵĲ��ҷ��������ǣ��۰���ҷ�Ҫ�����Ա�������˳��洢�ṹ�����ұ���Ԫ�ذ��ؼ�����������
	
	ʹ�ô˲��ҷ�ǰ�᣺
		1. ���ҵĴ洢�ṹ����Ϊ˳��洢�ṹ������˳�����δ洢Ԫ�صĽṹ
		2. �����Ա��е�Ԫ�ر���Ϊ�������򡱡�

	���ֲ��ҷ���ʱ�临�Ӷȣ�
		1. ���ֲ��ҷ��Ļ���˼����ǲ��ϱȽϱ����м��ֵ��Ȼ�󽫱�ķ�Χ�ְ�ȥ����������ʱ�临�ӶȾ���whileѭ���Ĵ�������
		2. �ܹ�Ϊn��Ԫ�أ���������ȥ����n, n/2, n/4,....n/(2��k�η�), ����k��ѭ���Ĵ���������n/(2��k�η�)ȡ����Ҫ>=1,�������������ֱ����Ϊ1��Ԫ�أ����ԣ�
	��n/(2��k�η�) = 1���ɵ�"k = log2n",����ʱ�临�Ӷȿ��Ա�ʾΪO(log2n)�� 
		3. ����1��2�ɵã����ֲ��ҷ�ʱ�临�Ӷ����ΪO(1), ����ΪO(log2n)��

	���ҹ��̣�������е�Ԫ���ǰ��������еģ���
		1. ����ѡȡ���м��Ԫ��a[n/2]����Ҫ��ѯ��ֵx���бȽϣ������ֵ��ȣ��������ҵ����˳�����
		2. ��û���ҵ���������n/2��ǰ�벿�ͺ�벿��Ϊ�����ӱ�a1��a2��ͬʱ�Ƚ�x�Ǵ���a[n/2]����С��a[n/2]
		3. ��xС��a[n/2],���a1��Ϊ��һ���������x����a[n/2]�����a2��Ϊ��һ�������
		4. �ظ�1��2��3��ֱ���ҵ�Ԫ�ػ����ֱ�����Ϊֹ��
*/

/*
	Desc: �۰���ҷ���Binary Search��
	Param: array ��ѯ�����Ա�
	Param: searchValue ��Ҫ��ѯ��ֵ
	Param��size ���Ա�array�ĳ���
	return: ����ֵΪ���ҵ���Ԫ���������е��±꣬���û��û���ҵ�������-1
*/
template<typename T>
int BinarySearch(T array[], T searchValue, int size)
{
	if ((array == NULL) || (size <= 0))
	{
		return -1;
	}

	int high = size - 1; //��ѯ����ֹλ��
	int low = 0;	//��ѯ����ʼλ��
	int middle = low + (high - low) / 2; //��ѯ���ӱ��е��м�ֵ��λ��

	while ((high - low) >= 0)
	{
		//�պ�Ҫ�ҵ�ֵΪ�ֱ��е��м�ֵ
		if (array[middle] == searchValue)
		{
			return middle;
		}

		//����Ϊ�ӱ��е��м�ֵ�����ж�Ϊ��һ�ֱ�
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
		//���ӱ�β��ǰ�������ҵ���һ��С����element���±�
		while (tempEndIndex > tempStartIndex)
		{
			if (array[tempEndIndex] <= element)
			{
				array[tempStartIndex] = array[tempEndIndex];
				break;
			}

			tempEndIndex--;
		}

		//���ӱ�ͷ�����������ҵ���һ������element���±�
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

	//����
	QuickSort(array, 0, size - 1);

	cout << "���Ա���Ϣ���£� " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << array[i] << " ";
	}
	
	int biaozhi = 1;
	while (biaozhi)
	{
		int data = 0;
		cout <<endl<< "�������ѯ���ݣ� " << endl;
		cin >> data;


		int index = BinarySearch(array, data, size);

		cout << endl << "��ѯ������£� " << endl;
		cout << "���������Ա��е��±�Ϊ��" << index << endl;

		system("pause");

		cout << endl << "�Ƿ���Ҫ��ѯ(1������0�˳�): ";
		cin >> biaozhi;
	}


    return 0;
}

