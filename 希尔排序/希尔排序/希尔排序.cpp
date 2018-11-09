// ϣ������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

/*
	ϣ������shell's sort�����ǡ��������򡱵�һ���ֳ�Ϊ����С��������Diminishing Increment Sort����,��ֱ�Ӳ��������㷨��һ�ָ���Ч�ĸĽ��汾��ϣ�������ǡ����ȶ������㷨������
	ϣ�������ȶ��Խ����� ���ڶ�β�����������֪��һ�β����������ȶ��ģ�����ı���ͬԪ�ص����˳�򣬵��ڲ�ͬ�Ĳ�����������У���ͬ��Ԫ�ؿ����ڸ��ԵĲ����������ƶ�������ȶ��Ծͱ����ң���

	ϣ�������ǰѼ�¼���±��һ���������飬��ÿ��ʹ��ֱ�Ӳ��������㷨�������������𽥼��٣�ÿ������Ĺؼ���Խ��Խ�࣬����������1ʱ�������ļ�ǡ���ֳ�1�飬�㷨����ֹ��

	ϣ������ƽ��ʱ�临�Ӷ�Ϊn��1.3�η����ʱΪn��ƽ�������ʱΪn����

	ϣ�������Ч�ʸ���ѡ�ġ��������С�������أ������ѡ�����ŵ��������黹��δ֪������

	ϣ������˼�룺
		1. ��ȡС��n������d1��Ϊ��һ���������ļ���ȫ����¼���顣���о���Ϊd1�ı�������ͬһ���С����ڸ����ڽ���ֱ�Ӳ�������
		2. ȡ�ڶ�������d2<d1�ظ�1��ֱ�����е�����dt = 1(dt<d(t-1)...<d2<d1),�����м�¼����ͬһ���н���ֱ�Ӳ�������Ϊֹ��

*/

template<typename T>
void Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}


/*
	Des��ϣ�������㷨
	Param:
*/
template<typename T>
void ShellSort(T array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	//�Զ���һ����������(һ��Ϊsize��һ���һ��...ֱ��1)  //��Ҫ�ﵽЧ����һ��Ҫ��һ����ʵ��Զ����������У�������  --��Ҳ�;����˴��㷨������ȥ������ģ��������ֻ��������С�����ݣ����ҵ��ҵ����������У�����
	vector<int> incrementArray;
	incrementArray.push_back(5);
	incrementArray.push_back(3);
	incrementArray.push_back(2);
	incrementArray.push_back(1);

	//�����������п�ʼϣ������
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

	cout << "����ǰ�� " << endl;
	for (int i=0; i<size; i++)
	{
		cout << a[i] << " ";
	}

	ShellSort(a, size);

	cout <<endl<< "����� " << endl;
	for (int i = 0; i<size; i++)
	{
		cout << a[i] << " ";
	}

	system("pause");

    return 0;
}

