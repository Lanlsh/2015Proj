// QuitSort.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;

/*
�������򷨣� ����������Ŀǰƽ��Ч�����������㷨�����Ϊnlogn, ����Ϊnƽ��

�������򷨵�ʵ��ԭ��ѡȡ�����е�һ�����ݣ�һ��Ϊ�����һ��Ԫ�أ����������д��ڸ�Ԫ�ص����ݷ������ĺ��棬С�ڡ��򡱵����������ݷ�������ǰ�棨���Կ����ǲ��ȶ��ģ��������һ�˿�������
	1. ������A�ĳ���Ϊn�����ñ���i=0, j=n-1.
	2. ��A[0]������Ϊ�ο�����k��������Ӻ���ǰ������j--������������һ��С�ڻ����A[0]�����ݣ���������A[i] = A[j], A[j] = k;
	3. �������ǰ����������i++��,��������һ������k�����ݣ���������A[i] = A[j], A[j] = k;
	4. ѭ��2��3��ֱ��i==j
	5. ��ʱ��С��k��ֵ��Ϊһ�����飬����k����Ϊ����һ�����飬��ִ��2��3��4��ֱ������ѭ������ĳ���Ϊ1����

*/

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
		while((array[startIndex] <= key) && (startIndex < endIndex))
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
Desc: ð�������㷨��������
Param: int array[]  ���������
Param: int arraySize ���������ĳ���
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

	//cout << "����ǰ�� "<<endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " " ;
	//}

	//��ȡ��������ʱ��
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	QuickSort(a, 0, size-1);
	t2 = GetTickCount();
	cout << endl << "������������ʱ�䣺 "<< t2-t1<<"ms"<< endl;

	t3 = GetTickCount();
	BubbleSort(b, size);
	t4 = GetTickCount();

	cout << endl << "ð����������ʱ�䣺 " << t4 - t3 << "ms" << endl;

	//cout << endl << "����� " << endl;
	//for (int i = 0; i<size; i++)
	//{
	//	cout << a[i] << " " ;
	//}

	system("pause");

    return 0;
}

