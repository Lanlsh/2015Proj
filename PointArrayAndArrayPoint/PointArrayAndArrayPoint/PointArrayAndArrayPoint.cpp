// PointArrayAndArrayPoint.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include <iostream>

using namespace std;

int main()
{
	int(*a)[3];	//数组指针
	int* b[2];	//指针数组

	int B1[] = { 0, 1 };
	int B2[] = { 10, 11 };
	int B3[][3] = {0, 1, 10, 11};

	int c1 = 0;
	int c2 = 1;
	b[0] = &B1[0];
	b[1] = &B1[1];
	*b[0] = B2[0];

	//b = B1;
//	c = B3;

	//cout << "b[0]: " << b[0] << endl;
	//cout << "b[1]: " << b[1] << endl;
	//cout << "*b[0]: " << *b[0] << endl;
	//cout << "*b[1]: " << *b[1] << endl;

	//a = B3;

	//int d = a[0][2];
	//int e = a[1][2];

	//cout << "d: " << d << endl;
	//cout << "e: " << e << endl;

	system("pause");

    return 0;
}

