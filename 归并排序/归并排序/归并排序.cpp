// �鲢����.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include "windows.h"

using namespace std;


/*
	�鲢����Merge Sort�����鲢�����ǽ����ڹ鲢�����ϵ�һ����Ч�������㷨�����㷨�ǲ��÷��η���Divide and Conquer����һ���ǳ����͵�Ӧ�á������е������кϲ����õ���ȫ�����
���У�����ʹÿ��������������ʹ�����жμ������������������ϲ���һ���������Ϊ����·�鲢����

	�鲢�����ǡ��ȶ������򡱣�����ȵ�Ԫ�ص�˳�򲻻ᷢ���ı䡣

	�ص㣺
		1. �鲢�����ٶȡ������ڡ��������򣨲��ȶ���������Ϊ�ȶ������㷨��һ�����ڶ��������򣬵������������������С�
		2. �鲢����ıȽϴ���С�ڿ�������ıȽϴ������ƶ�����һ����ڿ���������ƶ�����������

	�鲢�����ʱ�临�Ӷ�Ϊ��nlogn, ��ú�����nlogn��  //�ȶ��ģ�������

	�鲢�����˼·��
		1. ����һ�����У��Ƚ������������ֽ��й鲢������merge�����γ�floor(n/2)�����У������ÿ�����а�������Ԫ�أ�����ҲΪ1��Ԫ�أ�
		2. ��1���е��������ٴι鲢���γ�floor(n/4)�����У������ÿ�����а���4��Ԫ��(����Ϊ3��Ԫ�أ���������ʱ��)��
		3. �ظ�2��֪�����������кϲ���һ������
*/


/*
	Des: �鲢�㷨�� ���ڹ鲢����������
	Param: array ��������
	Param: start_1	��һ���Ӷεĳ�ʼλ��
	Param: end_1	��һ���Ӷε�ĩλ��
	Param: start_2	�ڶ����Ӷεĳ�ʼλ��
	Param: end_2	�ڶ����Ӷε�ĩλ��
	Param: size		��������ĳ���
*/
template<typename T>
void Merge(T array[], int start_1, int end_1, int start_2, int end_2, int size)
{
	if (start_2 >= size)
	{
		return;
	}

	//����һ����ʱ�Ļ������飬���ڴ�Ź鲢���ֵ
	int length = end_2 - start_1 + 1;
	T* tempArray = new T[length];
	memset(tempArray, 0, length * sizeof(T));

	//�ȶ���һ�����飬����Ϊend_1-start_1�����ڴ��������ֵ
	memset(tempArray, 0, sizeof(T)*length);

	//�Ƚ��������У�����Сֵ�ȷ�����ʱ����
	int x = start_1;	//���ڱ������1��ʣ��Ԫ�ص���ʼ�±�
	int y = start_2;	//���ڱ������2��ʣ��Ԫ�ص���ʼ�±�
	int index_temp = 0;		//���ڱ��temp�Ѽ�¼������

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

	//����Ƿ���δ�ӵ���ʱ�����Ԫ�أ����뵽��ʱ������
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

	//������õ���ʱ����ı���copy��array��
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
	Des��Merge Sort
	Param: array ��������
	Param: size	Ҫ����ĳ���
*/
template<typename T>
void MergeSort(T array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	//�Ƚ�����С����������
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

	//�ϲ�������
	int n = 2;	//�����еĳ���
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
	for (int i = size / 2 - 1; i >= 0; i--)
	{
		//����С��������ʼ����
		AdjustHeap(array, size, i);
	}

	//�ѵĸ��ڵ�Ͷѵ����һ��Ҷ�ӽڵ㽻��Ԫ�ؽ���Ԫ��
	while (size)
	{
		T temp = array[size - 1];
		array[size - 1] = array[0];
		array[0] = temp;

		size--;
		//�ٵ�������Ϊ��һ���Ѹ��ڵ�ֵ�ı䣬�������϶��µ�������ѣ�
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

	cout <<endl<< "�������� " << size << endl << endl;
	//cout << "����ǰ�� " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " ";
	//}
	//��ȡ��������ʱ��
	DWORD t1, t2, t3, t4, t5, t6, t7, t8;

	t1 = GetTickCount();
	QuickSort(a, 0, size - 1);
	t2 = GetTickCount();
	cout << endl << "������������ʱ�䣺 " << t2 - t1 << "ms" << endl;

	t3 = GetTickCount();
	HeapSort(b, size);
	t4 = GetTickCount();

	cout << endl << "����������ʱ�䣺 " << t4 - t3 << "ms" << endl;

	t5 = GetTickCount();
	MergeSort(c, size);
	t6 = GetTickCount();

	cout << endl << "�鲢��������ʱ�䣺 " << t6 - t5 << "ms" << endl;

	//t7 = GetTickCount();
	//BoubleSort(d, size);
	//t8 = GetTickCount();

	//cout << endl << "ð����������ʱ�䣺 " << t8 - t7 << "ms" << endl;

	//cout << endl << "����� " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << c[i] << " ";
	//}

	system("pause");

    return 0;
}

