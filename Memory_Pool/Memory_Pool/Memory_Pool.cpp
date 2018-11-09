// Memory_Pool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream> 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
using namespace std;

struct ST_MemoryPool
{
	char* pStart;	//总内存块的起始位置
	char* pUseStart;	//使用时的位置
	int totalSize;	//内存大小
	int useSize;	//已使用大小

	ST_MemoryPool(int size)
	{
		totalSize = size;
		useSize = 0;
		pStart = (char*)malloc((totalSize) * sizeof(char)); 
		pUseStart = pStart;

		cout << "构造函数： size: " << totalSize << endl;
	}

	~ST_MemoryPool()
	{
		cout << "执行析构函数" << endl;
		if (pStart)
		{
			free(pStart);
			pStart = NULL;
			pUseStart = NULL;
			totalSize = 0;
			useSize = 0;

			cout << "释放内存池内存 " << endl;
		}
	}

	void UsingSize(int n)
	{
		if (useSize+n > totalSize)
		{
			assert(useSize + n <= totalSize);
			return;
		}

		pUseStart = pUseStart + n;
		useSize += n;
	}

	void* operator new(size_t size)
	{
		if (size <= 0)
		{
			return NULL;
		}

		cout << "new分配内存： " << sizeof(ST_MemoryPool) * sizeof(char) << endl;
		return (void*)malloc(sizeof(ST_MemoryPool) * sizeof(char));
	}

	void operator delete(void* st)
	{
		if (st == NULL)
		{
			return;
		}

		free(st);
		st = NULL;
		cout << "delete释放结构体内存 " << endl;
	}

};

int main()
{
	ST_MemoryPool* st_mp = new ST_MemoryPool(1024);
	cout << "st_mp.totalSize: " << st_mp->totalSize << endl;
	int* a = (int*)(st_mp-> pUseStart);
	*a = 200;
	st_mp->UsingSize(100);
	float* b = (float*)(st_mp->pUseStart);
	*b = 1000;
	st_mp->UsingSize(300);

	double* c = (double*)(st_mp->pUseStart);
	*c = 1500;
	st_mp->UsingSize(600);

	int* aa = (int*)(st_mp->pStart);
	cout << "aa: " << *aa << endl;

	float* bb = (float*)(st_mp->pStart+100);
	cout << "bb: " << *bb << endl;

	double* cc = (double*)(st_mp->pStart + 100+300);
	cout << "cc: " << *cc << endl;

	delete st_mp;
	system("pause");

    return 0;
}

