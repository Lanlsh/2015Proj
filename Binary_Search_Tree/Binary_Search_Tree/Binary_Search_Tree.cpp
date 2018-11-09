// Binary_Search_Tree.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>

using namespace std;
/*
	������������Binary Search Tree����Ҳ��Ϊ�����������������ָһ�ſ������߾����������ʵĶ�������
		1. ����ڵ�����������գ��������������нڵ��ֵ��С�ڡ����ĸ��ڵ��ֵ��
		2. ����ڵ�����������գ��������������нڵ��ֵ�����ڡ����ĸ��ڵ��ֵ��
		3. ����ڵ����������������Ҳ��Ϊ�����������

	������������ص㣺������<���ڵ�<��������������
	
	ע�⣺ ������������ͬԪ�أ���Ҫʹ�ö�����������ң���ʱ���ԡ���Ϊ����ͬԪ�������������Ľڵ㴦(��ʱ������ʵ�����϶����������)��������
��һ���������ͬԪ�أ�����ʹ�á����������������ң���

	��������������ƣ�����ҡ������ʱ�临�ӶȽϵͣ�ΪO(log2n), �ΪO(n)��
	�����������Ӧ�ó��������������ʱ�������������ݽṹ�����ڡ�������Ϊ��������ݽṹ���缯�ϡ�multiset����������ȡ���

	����������ʵ�֣�
		1. �ȹ���һ�Ŷ�����������ʹ�����϶��������������ʣ�ʹ������ʵ�֣������ν���������ǡ�û����ͬԪ�ص����С�������
		2. ����һ������ֵvalue�����ֵ���ڸ��ڵ㣬����ҵ�����
		3. ���value С�ڸ��ڵ㣬��Ƚϸ��ڵ����������
		4. ���value ���ڸ��ڵ㣬��Ƚϸ��ڵ����������
		5. ѭ��2��3��4��ֱ����������ҵ���
*/

struct Tree
{
	int value;
	Tree* pLeft;
	Tree* pRight;
};

/*
	Desc: �������������Ԫ��
	Param: pRootTree ���ڵ�
	Param��value ��Ҫ���ӵ�Ԫ�ص�ֵ
*/
void AddElementToBST(Tree* pRootTree, int value)
{
	if (pRootTree == NULL)
	{
		return;
	}

	if (pRootTree->value > value)
	{
		if (pRootTree->pLeft == NULL)
		{
			Tree* pNewTree = new Tree();
			pNewTree->value = value;
			pNewTree->pLeft = NULL;
			pNewTree->pRight = NULL;

			pRootTree->pLeft = pNewTree;
		}
		else
		{
			AddElementToBST(pRootTree->pLeft, value);
		}
			
	}
	else if (pRootTree->value < value)
	{
		if(pRootTree->pRight == NULL)
		{ 
			Tree* pNewTree = new Tree();
			pNewTree->value = value;
			pNewTree->pLeft = NULL;
			pNewTree->pRight = NULL;

			pRootTree->pRight = pNewTree;
		}
		else
		{
			AddElementToBST(pRootTree->pRight, value);
		}
			
	}
	else
	{
		assert(pRootTree->value == value);
	}
}

/*
	Desc: �������������
	Param: vTree ���ڵ�ļ���
	Param: array ��ѯ����
	Param: size ���鳤��
	reutrn: ����������ĸ��ڵ�
*/
Tree* CreateBinarySearchTree(int array[], int size)
{
	if ((size <= 0) || (array == NULL))
	{
		return NULL;
	}

	Tree* pRootTree = NULL;
	for (int i=0; i<size; i++)
	{
		if (pRootTree == NULL)
		{
			pRootTree = new Tree();
			pRootTree->value = array[i];
			pRootTree->pLeft = NULL;
			pRootTree->pRight = NULL;
		}
		else
			AddElementToBST(pRootTree, array[i]);
	}

	return pRootTree;
}

/*
	Desc: �����������ѯ
	Param: array ��ѯ����
	Param: size ���鳤��
	Param��searchValue ��ѯ��ֵ
	return�����û�ҵ�������-1
*/
int BST(int array[], int size, int searchValue)
{
	Tree* pBST = CreateBinarySearchTree(array, size);
	while (pBST)
	{
		if (pBST->value == searchValue)
		{
			cout << "�Ѿ��ҵ�������" << endl;
			//û���ͷ��ڴ棡������
			return 1;
		}
		else if (pBST->value < searchValue)
		{
			pBST = pBST->pRight;
		}
		else
		{
			pBST = pBST->pLeft;
		}
	}

	//û���ͷ��ڴ棡������
	return -1;
}

int main()
{
	int array[20] = { 0 };
	int size = sizeof(array) / sizeof(array[0]);
	for (int i=0; i<size; i++)
	{
		array[i] = rand() % 100;
		cout << array[i] << " ";
	}

	cout << endl;

	char cAgain = 'y';
	while (cAgain == 'y')
	{
		int searchValue = 0;
		cout << "Input search value: ";
		cin >> searchValue;
		int find = BST(array, size, searchValue);
		if (find == -1)
		{
			cout << "There have not the value " << searchValue << endl;
		}

		cout << "Do you need search again? (y/n): ";
		cin >> cAgain;
	}

    return 0;
}

