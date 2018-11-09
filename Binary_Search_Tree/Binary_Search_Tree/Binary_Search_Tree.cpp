// Binary_Search_Tree.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>

using namespace std;
/*
	二叉搜索树（Binary Search Tree），也称为“二叉查找树”，是指一颗空树或者具有下列性质的二叉树：
		1. 任意节点的左子树不空，则左子树上所有节点的值“小于”它的跟节点的值。
		2. 任意节点的右子树不空，则右子树上所有节点的值“大于”它的跟节点的值。
		3. 任意节点的左子树、右子树也都为二叉查找树。

	二叉查找树的特点：左子树<根节点<右子树！！！！
	
	注意： 若数列中有相同元素，且要使用二叉查找树查找，此时可以“人为将相同元素置于深度最深的节点处(此时的数其实不符合二叉查找树了)”！！！
（一般如果有相同元素，不会使用“二叉搜索树”查找！）

	二叉查找树的优势：其查找、插入的时间复杂度较低，为O(log2n), 最坏为O(n)。
	二叉查找树的应用场景：二叉查找树时基础的线性数据结构，用于“构建更为抽象的数据结构，如集合、multiset、关联数组等”。

	二叉搜索树实现：
		1. 先构建一颗二叉搜索树，使树符合二叉搜索树的性质（使用链表实现）。本次讲解的数列是“没有相同元素的数列”！！！
		2. 给定一个搜索值value，如果值等于根节点，则查找到！！
		3. 如果value 小于根节点，则比较根节点的左子树。
		4. 如果value 大于根节点，则比较根节点的右子树。
		5. 循环2、3、4，直到搜索完或找到。
*/

struct Tree
{
	int value;
	Tree* pLeft;
	Tree* pRight;
};

/*
	Desc: 二叉查找树增加元素
	Param: pRootTree 跟节点
	Param：value 需要增加的元素的值
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
	Desc: 创建二叉查找树
	Param: vTree 树节点的集合
	Param: array 查询数组
	Param: size 数组长度
	reutrn: 二叉查找树的跟节点
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
	Desc: 二叉查找树查询
	Param: array 查询数组
	Param: size 数组长度
	Param：searchValue 查询的值
	return：如果没找到，返回-1
*/
int BST(int array[], int size, int searchValue)
{
	Tree* pBST = CreateBinarySearchTree(array, size);
	while (pBST)
	{
		if (pBST->value == searchValue)
		{
			cout << "已经找到！！！" << endl;
			//没有释放内存！！！！
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

	//没有释放内存！！！！
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

