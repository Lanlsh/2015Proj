// ð������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

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
	cout << "����ǰ��" << endl;
	//for (int i=0; i<arraySize; i++)
	//{
	//	array[i] = rand()%10000;
	//	cout << array[i] << " ";
	//}

	//cout << endl;
	//cout << "�����" << endl;

	//��ȡ��������ʱ��
	DWORD t1, t2, t3, t4;
	t1 = GetTickCount();

	BubbleSort(array, arraySize);

	t2 = GetTickCount();

	//for (int i = 0; i<arraySize; i++)
	//{
	//	cout << array[i] << " ";
	//}



	cout << "����ʱ�䣺 " << t2 - t1 <<"����"<< endl;


	system("pause");
    return 0;
}

