// ������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <Windows.h>

using namespace std;

/*
	������ ��һ�����ݷ��������У�������ȫ����������ʽ���С��������Ϊ������ѡ��͡�С���ѡ����������ǲ��ȶ��ģ����� �������ƽ�����Ӷ�Ϊnlogn,��ú����ʱ��ʱ�临�Ӷȶ���nlogn
	����ѣ� �����ڵ�>=�ӽڵ㣬����ֵ�ڸ��ڵ㡣
	С���ѣ� �����ڵ�<=�ӽڵ㣬��С��ֵ�ڸ��ڵ㡣

	�������ʵ��ԭ���Դ����Ϊ������ 
		1. ��ʼ������ѣ�ʹ��ǰ���������ѵĶ��塣
		2. �����ڵ������һ��Ҷ�ӽڵ㻥��λ�ã� int temp = A[0]; A[0] = A[n-1]; A[n-1] = temp;
		3. �ٽ�ʣ��A[0]~A[n-2]����1��2�����������ֱ������Ԫ��ֻʣ1��Ԫ��ʱ����

		ע�� A[i]������ΪA[2i+1],�Һ���ΪA[2i+2]

	����ѵ�ʵ�ֹ��̣�
		ǰ����Ҫ���ģ� ����һ�����ݵĳ���Ϊn���򽫴����������δ��������A�С� ��ڵ��λ���������е��±�Ϊx����A[i]������ΪA[2i+1]��A[i]���Һ���ΪA[2i+2]����0<=i<=n/2-1������������
		1. ���ú���AdjustHeap������ʼ���ѣ�ʹ���������ѵ�������
		2. ���ѵĸ��ڵ�Ԫ�������һ������ǰѭ���е����һ����Ҷ�ӽڵ�Ԫ��λ�û���
		3. ִ��1��2��ֱ������Ԫ�ظ���ֻ��1������
*/

/*
Des: ����������㷨
Param��array  ��Ҫ���������
Param��size ��Ҫ���������ĳ���
Param��element ����ǰ����Ҫ���ɡ�����ѵĸ��ڵ㡱�������е��±�
*/

template<typename T>
void AdjustHeap(T array[], int size, int element)
{
	/*
		�ڴ˵����Ĵ����element������Ӧ��Ϊ�Ѿ������õĴ����
	*/
	int pIndex = element;
	int lIndex = 2 * pIndex + 1;
	int rIndex = 2 * pIndex + 2;

	//ʹ�ڵ�elementΪ���ڵ㣬�����������ϴ󶥶�����
	while (lIndex < size)
	{
		//����и��ڵ�
		if (rIndex < size)
		{
			//������ڵ�>=���� && ���ڵ�>=�Һ��ӣ�������ѭ��
			if ((array[pIndex] >= array[lIndex]) && (array[pIndex] >= array[rIndex]))
			{
				return;
			}

			//�ȱȽ����Һ��ӣ�ȡ���ĺ����ڸ��ڵ�Ƚϣ�������ڵ�<�ú��ӣ�����ú��ӽ���λ�ã�������������ֻ�ı���һ��������ֻ��Ҫ�������������¾�OK��������
			if (array[lIndex] >= array[rIndex])
			{
				//�ж��Ƿ����������������С�����������򽻻�λ��
				if (array[pIndex] < array[lIndex])
				{
					T temp = array[pIndex];
					array[pIndex] = array[lIndex];
					array[lIndex] = temp;

					//�ص㣡��������
					pIndex = lIndex;
				}
			}
			else
			{
				//�ж��Ƿ����������������С�����������򽻻�λ��
				if (array[pIndex] < array[rIndex])
				{
					T temp = array[pIndex];
					array[pIndex] = array[rIndex];
					array[rIndex] = temp;

					//�ص㣡��������
					pIndex = rIndex;
				}
			}
		}
		else 
		{
			//������ڵ�>=����, ������ѭ��
			if (array[pIndex] >= array[lIndex])
			{
				return;
			}

			//�ж��Ƿ����������������С�����������򽻻�λ��
			if (array[pIndex] < array[lIndex])
			{
				T temp = array[pIndex];
				array[pIndex] = array[lIndex];
				array[lIndex] = temp;

				//�ص㣡��������
				pIndex = lIndex;
			}
		}


		//����
		lIndex = 2 * pIndex + 1;
		rIndex = 2 * pIndex + 2;
	}

}

/*
Des: �������㷨
Param��array  ��Ҫ���������
Param��size ��Ҫ���������ĳ���
*/
template<typename T>
void HeapSort(T array[], int size)
{
	if (size <= 1)
	{
		return;
	}

	//�ȹ���һ�ŷ��ϴ󶥶ѵ���
	for (int i=size/2 -1; i>=0; i--)
	{
		//����С��������ʼ����
		AdjustHeap(array, size, i);
	}

	//�ѵĸ��ڵ�Ͷѵ����һ��Ҷ�ӽڵ㽻��Ԫ�ؽ���Ԫ��
	while (size)
	{
		T temp = array[size-1];
		array[size - 1] = array[0];
		array[0] = temp;

		size--;
		//�ٵ�������Ϊ��һ���Ѹ��ڵ�ֵ�ı䣬�������϶��µ�������ѣ�
		AdjustHeap(array, size, 0);
	}
}

/*
Des: ���������㷨
Param: array ����
Param: start �����������ʼλ��
Param: end ����������յ�λ��
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
		//������Ӻ���ǰ�������ҵ���һ��С����key��ֵ
		while ((array[endIndex] > key) && (start < endIndex))
		{
			endIndex--;
		}

		array[startIndex] = array[endIndex];
		//startIndex = end;

		//�������ǰ�����������ҵ���һ������key��ֵ
		while ((array[startIndex] <= key) && (startIndex < endIndex))
		{
			startIndex++;
		}

		array[endIndex] = array[startIndex];
	}

	//��¼���ֵ
	array[startIndex] = key;

	//����ʣ�������
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

	cout << "����ǰ�� "<<endl;
	for (int i = 0; i<size; i++)
	{
		cout << b[i] << " " ;
	}

	//��ȡ��������ʱ��
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	QuickSort(a, 0, size);
	t2 = GetTickCount();
	cout << endl << "������������ʱ�䣺 " << t2 - t1 << "ms" << endl;

	t3 = GetTickCount();
	HeapSort(b, size);

	t4 = GetTickCount();


	cout << endl << "����������ʱ�䣺 " << t4 - t3 << "ms" << endl;

	cout << endl << "����� " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << b[i] << " " ;
	}

	system("pause");

    return 0;
}

