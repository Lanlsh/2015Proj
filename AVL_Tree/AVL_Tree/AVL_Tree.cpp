// AVL_Tree.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <assert.h>

using namespace std;

/*
	平衡二叉树（Self-Balacing binary tree）: 又称为“AVL树”，此AVL不同于AVL算法！！！以下简称为“AVL树”。
	“AVL树”是“BST树（Binary Search Tree）”的演变版本，也就是“AVL树”也是BST树！！！

	"AVL树"由于它任意节点的左右子树的深度差的绝对值小等于1，且权值左孩子<父节点<右孩子等的特性，使得其查询和增、删节点的时间复杂度都为O(log2n)。即即使在最差
的情况下，它的搜索和增、删节点的效率也能达到log2n!!!!

	AVL树的规范要求：
		1. 左孩子<父节点<右孩子 （权值）
		2. 此树中的任意节点的左右子树的深度差的绝对值<=1

	AVL树的创建：
		假设根节点为T增加一个节点a时：
		1. 先判断T是否存在，如果不存在，设T = a
		2. 将a先按BST树增加节点的方式增加
		3. 从增加节点的父节点开始，判断此树的的左右子树的深度差的绝对值是否<=1，若>1,则执行4步骤
		4. 通过“旋转”使该树重新达到平衡，旋转的次数为1次或多次！！设最开始出现不平衡的子树的根节点的root的旋转的种类有：
			a. 右旋(R)：左子树的深度高于右子树时，且左子树以及其子树的节点全部都仅存在于左边，如60、50、40，此时向右旋转一次即平衡
			b. 左旋(L)：右子树的深度高于左子树时，且右子树以及其子树的节点全部都仅存在于右边，如60、70、80，此时向左旋转一次即平衡
			c. 左右旋（LR）：左子树的深度高于右子树时，且新增节点a在root的左孩子的右孩子上，如60、40、50，此时先向左旋转一次得60、50、40，再向右旋转一次即平衡
			d. 右左旋（RL）：右子树的深度高于左子树时，且新增节点a在root的右孩子的左孩子上，如60、80、70，此时向右旋转一次得60、70、80，再向左旋转一次即平衡

	AVL树删除节点：
		先找到需要删除的节点，再根据需要删除节点node有无孩子节点和在删除node后的左右孩子的深度以及node的左右孩子的深度来判断（参考函数void deleteNode(int value)）
		1. 删除点为叶子节点
		2. 删除的节点有左孩子
		3. 删除的节点有右孩子
		4. 删除的节点左右孩子都有
*/

struct Tree 
{
	int value;	//权值
	Tree* pLeft;	//记录左子树节点
	Tree* pRight;	//记录右子树节点
	Tree* pParent;	//记录父节点
	Tree* pEqualValue;	//记录相同值的节点

	int leftDeep;	//记录左子树深度
	int rightDeep;	//记录右子树深度

	Tree(int n)
	{
		value = n;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		pEqualValue = NULL;
		leftDeep = 0;
		rightDeep = 0;
	}

	void AddEV(Tree* node)
	{
		//默认将值相同的节点全部放于左孩子！！
		if (node == NULL)
		{
			return;
		}

		if (pEqualValue == NULL)
		{
			pEqualValue = node;
		}
		else
		{
			Tree* pTempNode = pEqualValue;
			while(pTempNode->pLeft)
			{
				pTempNode = pTempNode->pLeft;
			}

			pTempNode->pLeft = node;
			node->pParent = pTempNode;
		}
	}

	~Tree()
	{
		cout << "释放内存： " << value << endl;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		pEqualValue = NULL;
	}
};

static vector<Tree*> m_rootTree;
static Tree* m_rootNode = NULL;

/*
Desc: 增加节点的深度
*/
void AddDeep(Tree* node)
{
	Tree* pParent = node->pParent;
	Tree* pChilde = node;
	while (pParent != NULL)
	{
		if (pParent->pLeft == pChilde)
		{
			pParent->leftDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}
		else if (pParent->pRight == pChilde)
		{
			pParent->rightDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}

		pChilde = pParent;
		pParent = pParent->pParent;
	}

}

/*
	Desc: 降低深度
*/
void subDeep(Tree* node)
{
	Tree* pParent = node->pParent;
	Tree* pChilde = node;
	while (pParent != NULL)
	{
		if (pParent->pLeft == pChilde)
		{
			pParent->leftDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}
		else if (pParent->pRight == pChilde)
		{
			pParent->rightDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}

		pChilde = pParent;
		pParent = pParent->pParent;
	}

	m_rootNode = pChilde;
}

/*
	Desc: 右旋转
	Param: node //要旋转的跟节点
*/
bool rotationRight(Tree* node)
{
	Tree* c = node->pLeft;
	Tree* b = c->pLeft;
	Tree* a = node;

	if ((a==NULL) || (b==NULL) || (c==NULL))
	{
		//assert(a && b && c);
		return false;
	}

	Tree* tempRootNode = a->pParent;
	c->pParent = tempRootNode;
	if (tempRootNode != NULL)
	{
		if (tempRootNode->pRight == a)
		{
			tempRootNode->pRight = c;
		}
		else
		{
			tempRootNode->pLeft = c;
		}
	}

	a->pLeft = c->pRight;
	if (c->pRight)
	{
		c->pRight->pParent = a;
	}

	c->pRight = a;
	a->pParent = c;

	if (a->pLeft)
	{
		a->leftDeep = a->pLeft->leftDeep >= a->pLeft->rightDeep ? (a->pLeft->leftDeep + 1) : (a->pLeft->rightDeep + 1);
	}
	else
	{
		a->leftDeep = 0;
	}
	
	c->rightDeep = c->pLeft->leftDeep >= c->pRight->rightDeep ? (c->pLeft->leftDeep + 1) : (c->pRight->rightDeep + 1);

	//判断当期旋转后，是否符合“AVL树”
	if (abs(c->leftDeep - c->rightDeep) <= 1)
	{
		return true;
	}

	return false;
}

/*
	Desc: 左旋转
	Param: node //要旋转的跟节点
*/
bool rotationLeft(Tree* node)
{
	Tree* c = node->pRight;
	Tree* b = c->pRight;
	Tree* a = node;

	if ((a == NULL) || (b == NULL) || (c == NULL))
	{
		//assert(a && b && c);
		return false;
	}

	//情况1： 当a节点的子节点只存在a的右孩子节点上时，旋转一次就OK
	Tree* tempRootNode = a->pParent;
	c->pParent = tempRootNode;
	if (tempRootNode != NULL)
	{
		if (tempRootNode->pRight == a)
		{
			tempRootNode->pRight = c;
		}
		else
		{
			tempRootNode->pLeft = c;
		}
		
	}

	a->pRight = c->pLeft;
	if (c->pLeft)
	{
		c->pLeft->pParent = a;
	}

	c->pLeft = a;
	a->pParent = c;

	if (a->pRight)
	{
		a->rightDeep = a->pRight->leftDeep >= a->pRight->rightDeep ? (a->pRight->leftDeep + 1) : (a->pRight->rightDeep + 1);
	}
	else
	{
		a->rightDeep = 0;
	}

	c->leftDeep = c->pLeft->leftDeep >= c->pLeft->rightDeep ? (c->pLeft->leftDeep + 1) : (c->pLeft->rightDeep + 1);

	//判断当期旋转后，是否符合“AVL树”
	if (abs(c->leftDeep - c->rightDeep) <= 1)
	{
		return true;
	}

	return false;
}

/*
	Desc: 先右旋，再左旋转
	Param: node //要旋转的跟节点  //此时node的深度一定是>2的
*/
bool rotationRL(Tree* node)
{
	if ((node == NULL) || (node->pRight == NULL) || (node->pRight->pLeft == NULL))
	{
		assert((node != NULL) && (node->pRight != NULL) && (node->pRight->pLeft != NULL));
		return false;
	}

	Tree* a = node;
	Tree* b = a->pRight;
	Tree* c = b->pLeft;

	//右旋
	a->pRight = c;
	c->pParent = a;
	b->pLeft = c->pRight;
	b->pParent = c;
	c->pRight = b;

	if (b->pLeft)
	{
		b->leftDeep = b->pLeft->leftDeep >= b->pLeft->rightDeep ? (b->pLeft->leftDeep+1) : (b->pLeft->rightDeep+1);
	}
	else
	{
		b->leftDeep = 0;
	}
	
	c->rightDeep = b->leftDeep >= b->rightDeep ? (b->leftDeep+1) : (b->rightDeep+1);

	//左旋
	bool bIsOK = rotationLeft(a);
	if (bIsOK)
	{
		return true;
	}

	return false;
}

/*
Desc: 先左旋，再右旋转
Param: node //要旋转的跟节点  //此时node的深度一定是>2的
*/
bool rotationLR(Tree* node)
{
	if ((node == NULL) || (node->pLeft == NULL) || (node->pLeft->pRight == NULL))
	{
		assert((node != NULL) && (node->pLeft != NULL) && (node->pLeft->pRight != NULL));
		return false;
	}

	Tree* a = node;
	Tree* b = a->pLeft;
	Tree* c = b->pRight;

	//左旋
	a->pLeft = c;
	c->pParent = a;
	b->pRight = NULL;
	b->pParent = c;
	c->pLeft = b;
	b->rightDeep = 0;
	c->leftDeep = 1;

	//右旋
	bool bIsOK = rotationRight(a);
	if (bIsOK)
	{
		return true;
	}

	return false;
}

/*
	Desc: 调整平衡性
*/
void adjuestBalance(Tree* newNode)
{
	//从新增加的节点开始，从下往上，判断树是否平衡
	//平衡条件： 左右子树的高度差的绝对值<=1
	Tree* pParent = newNode->pParent;
	while (pParent)
	{
		//设置新的根节点

		int leftDeep = pParent->leftDeep;
		int rightDeep = pParent->rightDeep;

		bool bRotationIsOk = false;
		if (leftDeep- rightDeep > 1)
		{
			if ((pParent->leftDeep <= 2) && (pParent->rightDeep <= 2))
			{
				//当pParent的深度<=2时
				//（1）右旋
				if ((pParent->pLeft->pLeft != NULL) && (pParent->pLeft->pRight == NULL))
				{
					bRotationIsOk = rotationRight(pParent);
				}
				else
				{
					//（2）左右旋
					bRotationIsOk = rotationLR(pParent);
				}
			}
			else
			{
				//当pParent的深度>2时
				if ((pParent->pLeft->pLeft->pLeft != NULL) && (pParent->pLeft->pLeft->pRight == NULL) && (pParent->pLeft->pLeft->pLeft == newNode))
				{
					//（1）直接右旋(80、70、90、60、75、55)
					bRotationIsOk = rotationRight(pParent);
				}
				else if ((pParent->pLeft->pLeft->pLeft == NULL) && (pParent->pLeft->pLeft->pRight != NULL) && (pParent->pLeft->pLeft->pRight == newNode))
				{
					//(2)先LR，再右旋(80、70、90、60、75、65)
					bRotationIsOk = rotationLR(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
				else if ((pParent->pLeft->pRight->pLeft != NULL) && (pParent->pLeft->pRight->pRight == NULL) && (pParent->pLeft->pRight->pLeft == newNode))
				{
					//(3)先RL，再右旋(80、70、90、60、75、73)
					bRotationIsOk = rotationRL(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
				else
				{
					//(4)先左旋，再右旋(80、70、90、60、75、76)
					bRotationIsOk = rotationLeft(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
			}

			//如果平衡，则退出循环，否则继续在此父节点下旋转，直至平衡
			if (bRotationIsOk)
			{
				//降低深度
				subDeep(pParent);

				//已平衡
				break;
			}

		}
		else if (leftDeep - rightDeep < -1)
		{
			if ((pParent->leftDeep <= 2) && (pParent->rightDeep <= 2))
			{
				//当pParent的深度<=2时
				//（1）右旋
				if ((pParent->pRight->pRight != NULL) && (pParent->pRight->pLeft == NULL))
				{
					bRotationIsOk = rotationLeft(pParent);
				}
				else
				{
					//（2）右左旋
					bRotationIsOk = rotationRL(pParent);
				}
			}
			else
			{
				//当pParent的深度>2时
				if ((pParent->pRight->pRight->pRight != NULL) && (pParent->pRight->pRight->pLeft == NULL) && (pParent->pRight->pRight->pRight == newNode))
				{
					//（1）直接左旋(80、70、100、90、110、116)
					bRotationIsOk = rotationLeft(pParent);
				}
				else if ((pParent->pRight->pRight->pRight == NULL) && (pParent->pRight->pRight->pLeft != NULL) && (pParent->pRight->pRight->pLeft == newNode))
				{
					//(2)先RL，再左旋(80、70、100、90、110、105)
					bRotationIsOk = rotationRL(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
				else if ((pParent->pRight->pLeft->pRight != NULL) && (pParent->pRight->pLeft->pLeft == NULL) && (pParent->pRight->pLeft->pRight == newNode))
				{
					//(3)先LR，再左旋(80、70、100、90、110、96)
					bRotationIsOk = rotationLR(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
				else
				{
					//(4)先右旋，再左旋(80、70、100、90、110、86)
					bRotationIsOk = rotationRight(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
			}

			//如果平衡，则退出循环，否则继续在此父节点下旋转，直至平衡
			if (bRotationIsOk)
			{
				//降低深度
				subDeep(pParent);

				//已平衡
				break;
			}
		}
		else
		{
			pParent = pParent->pParent;
		}

	}

}

/*
	Desc: 增加节点

*/
void addNode(int value)
{
	if (m_rootNode == NULL)
	{
		Tree* node = new Tree(value);
		m_rootTree.push_back(node);
		m_rootNode = node;
		return;
	}

	//先将节点符合二叉搜索树方式放置好，再去判断是否平衡，不平衡就调整
	bool bIsAddOk = false;
	Tree* pParent = m_rootNode;
	Tree* node = new Tree(value);
	m_rootTree.push_back(node);

	while (!bIsAddOk)
	{
		if (pParent->value > value)
		{
			if (pParent->pLeft == NULL)
			{
				pParent->pLeft = node;
				node->pParent = pParent;

				//深度+1
				pParent->leftDeep += 1;
				if (pParent->pRight == NULL)
				{
					AddDeep(pParent);
				}

				bIsAddOk = true;
			}
			else
			{
				pParent = pParent->pLeft;
			}
		}
		else if (pParent->value < value)
		{
			if (pParent->pRight == NULL)
			{
				pParent->pRight = node;
				node->pParent = pParent;

				//深度+1
				pParent->rightDeep += 1;
				if (pParent->pLeft == NULL)
				{
					AddDeep(pParent);
				}

				bIsAddOk = true;
			}
			else
			{
				pParent = pParent->pRight;
			}
		}
		else
		{
			//增加值相同的节点
			pParent->AddEV(node);
			bIsAddOk = true;
		}

	}

	//检查平衡性
	if (!bIsAddOk)
	{
		return;
	}

	//调整平衡性
	adjuestBalance(node);
}

/*
	Desc: 创建平衡二叉树（Self-balancing binary search tree）（AVL树）
*/
void creatAVLTree(int array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	for (int i=0; i<size; i++)
	{
		addNode(array[i]);
	}

}

//使用AVL树搜索值value,返回该值的节点的指针，若没找到，则返回NULL
Tree* searchValue(int value)
{
	if (m_rootNode == NULL)
	{
		return NULL;
	}

	Tree* pTempNode = m_rootNode;
	while (pTempNode)
	{
		if (pTempNode->value > value)
		{
			pTempNode = pTempNode->pLeft;
		}
		else if (pTempNode->value < value)
		{
			pTempNode = pTempNode->pRight;
		}
		else
		{
			//找到
			return pTempNode;
		}
	}

	return NULL;
}

void deleteMemory()
{
	//释放内训可以考虑使用中序遍历释放，具体参考“二叉树的前序、中序、后续遍历”实现的博客！！
	for (int i=m_rootTree.size()-1; i>=0; i--)
	{
		delete m_rootTree[i];
	}
}

void deleteMemoryByValue(int value)
{
	vector<Tree*>::iterator iter = m_rootTree.begin();
	for (; iter!=m_rootTree.end();)
	{
		if (value == (*iter)->value)
		{
			Tree* node = (*iter);
			iter = m_rootTree.erase(iter);
			delete node;
			node = NULL;
			break;
		}
		else
		{
			iter++;
		}
	}

	cout << endl << "m_rootTree.size(): " << m_rootTree.size() << endl;
}

//删除叶子节点
void deleteLeafNode(Tree* node)
{
	//判断删除节点的父节点的左子树和右子树的深度
	Tree* pParent = node->pParent;
	if (pParent)
	{
		if (pParent->pLeft == node)
		{
			if (pParent->leftDeep - pParent->rightDeep >= 0)
			{
				pParent->pLeft = NULL;
				pParent->leftDeep = 0;
			}
			else
			{
				//由于右子树深度更深，此时再删去左节点，左子树深度必定会减1，导致树不平衡，这时需要左旋
				pParent->pLeft = NULL;
				pParent->leftDeep = 0;
				rotationLeft(pParent);
			}
		}
		else
		{
			if (pParent->rightDeep - pParent->leftDeep >= 0)
			{
				pParent->pRight = NULL;
				pParent->rightDeep = 0;
			}
			else
			{
				//由于左子树深度更深，此时再删去右节点，右子树深度必定会减1，导致树不平衡，这时需要右旋
				pParent->pRight = NULL;
				pParent->rightDeep = 0;
				rotationRight(pParent);
			}
		}

		subDeep(pParent);
	}
	else
	{
		m_rootNode = NULL;
	}
}

//删除的节点只含有左孩子
void deleteNodeOnlyHaveLeftChild(Tree* node)
{
	Tree* pParent = node->pParent;
	if (pParent)
	{
		node->pLeft->pParent = pParent;
		pParent->pLeft = node->pLeft;
		pParent->leftDeep = pParent->leftDeep - 1;

		subDeep(pParent);
	}
	else
	{
		node->pLeft->pParent = NULL;
		m_rootNode = node->pLeft;
	}
}

//删除的节点只含有右孩子
void deleteNodeOnlyHaveRightChild(Tree* node)
{
	Tree* pParent = node->pParent;
	if (pParent)
	{
		node->pRight->pParent = pParent;
		pParent->pRight = node->pRight;
		pParent->rightDeep = pParent->rightDeep - 1;

		subDeep(pParent);
	}
	else
	{
		node->pRight->pParent = NULL;
		m_rootNode = node->pRight;
	}
}

//删除的节点有左右孩子
void deleteNodeHaveDoubleKid(Tree* node)
{
	Tree* pParent = node->pParent;

	//比较node的左右孩子的深度，将深度小的挂到深度大的地方
	Tree* pLeft = node->pLeft;
	Tree* pRight = node->pRight;
	if (node->leftDeep >= node->rightDeep)
	{
		//将node的右孩子全部挂在node的左孩子上
		if (pParent)
		{
			if (pParent->pLeft == node)
			{
				pParent->pLeft = pLeft;
			}
			else
			{
				pParent->pRight = pLeft;
			}
			
			pLeft->pParent = pParent;
		}
		else
		{
			pLeft->pParent = NULL;
		}

		//找到node的左孩子的最右节点
		Tree* temp = pLeft;
		while (temp->pRight)
		{
			temp = temp->pRight;
		}

		pRight->pParent = temp;
		temp->pRight = pRight;

		subDeep(pRight);
		bool bIsNeedRotation = abs(pLeft->leftDeep - pLeft->rightDeep) > 1;

		while (bIsNeedRotation)
		{
			//左旋
			rotationLeft(pLeft);
			bIsNeedRotation = abs(pLeft->pParent->leftDeep - pLeft->pParent->rightDeep) > 1;
			if (bIsNeedRotation)
			{
				//此时可能出现左子树不平衡的情况
				//右旋
				rotationRight(pLeft->pLeft);
				pLeft = pLeft->pParent;
			}
		}

		subDeep(pLeft);

	}
	else
	{
		//将node的左孩子全部挂在node的右孩子上
		if (pParent)
		{
			if (pParent->pLeft == node)
			{
				pParent->pLeft = pLeft;
			}
			else
			{
				pParent->pRight = pLeft;
			}

			pRight->pParent = pParent;
		}
		else
		{
			pRight->pParent = NULL;
		}

		//找到node的左孩子的最右节点
		Tree* temp = pRight;
		while (temp->pLeft)
		{
			temp = temp->pLeft;
		}

		pLeft->pParent = temp;
		temp->pLeft = pLeft;

		subDeep(pLeft);
		bool bIsNeedRotation = abs(pRight->leftDeep - pRight->rightDeep) > 1;
		while (bIsNeedRotation)
		{
			//右旋
			rotationRight(pRight);
			bIsNeedRotation = abs(pRight->pParent->leftDeep - pRight->pParent->rightDeep) > 1;
			if (bIsNeedRotation)
			{
				//此时可能出现右子树不平衡的情况
				//左旋
				rotationLeft(pRight->pRight);
				pRight = pRight->pRight;
			}
		}

		subDeep(pRight);
	}

}

//AVL树删除节点
void deleteNode(int value)
{
	//先从AVL树中找到该节点
	Tree* node = searchValue(value);
	if (node == NULL)
	{
		cout << endl << "AVL树中没有此节点！！！" << endl;
		return;
	}

	//已找到，则根据需要删除节点node的父节点在删除node后的左右孩子的深度和node的左右孩子的深度来判断
	//1. 删除点为叶子节点
	//2. 删除的节点有左孩子
	//3. 删除的节点有右孩子
	//4. 删除的节点左右孩子都有
	if ((node->pLeft == NULL) && (node->pRight == NULL))
	{
		//1. 删除点为叶子节点
		deleteLeafNode(node);
	}
	else if ((node->pLeft != NULL) && (node->pRight == NULL))
	{
		//2. 删除的节点有左孩子
		deleteNodeOnlyHaveLeftChild(node);
	}
	else if ((node->pLeft == NULL) && (node->pRight != NULL))
	{
		//3. 删除的节点有右孩子
		deleteNodeOnlyHaveRightChild(node);
	}
	else
	{
		//4. 删除的节点左右孩子都有
		deleteNodeHaveDoubleKid(node);
	}
	

	//释放节点的内存
	deleteMemoryByValue(value);
}


//中序遍历
void middleOrder(Tree* pRoot)
{
	if (pRoot != NULL)
	{
		middleOrder(pRoot->pLeft);
		cout << pRoot->value << " ";
		Tree* pEVNode = pRoot->pEqualValue;
		while (pEVNode)
		{
			cout << pEVNode->value << " ";
			pEVNode = pEVNode->pLeft;
		}
		middleOrder(pRoot->pRight);
	}
}


int main()
{
	int array[20] = { 0 };
	int size = sizeof(array) / sizeof(array[0]);
	cout << "随机数列：" << endl;
	for (int i=0; i<size; i++)
	{
		array[i] = rand() % 100;
		cout << array[i] << " ";
	}

	cout << endl;
	creatAVLTree(array, size);
	cout << "使用中序遍历输出如下： " << endl;
	middleOrder(m_rootNode);

	int kind = 1;
	int value = 0;
	while (kind)
	{
		cout << endl << "需要进行哪种业务： 1、增加元素  2、查询数据  3、删除数据  0、退出" << endl;
		cin >> kind;
		switch (kind)
		{
		case 1:
		{
			cout << "请输入增加的元素： " << endl;
			cin >> value;
			addNode(value);
			cout << "使用中序遍历输出如下： " << endl;
			middleOrder(m_rootNode);
			break;
		}
		case 2: 
		{
			cout << "请输入需要查询的元素： " << endl;
			cin >> value;
			Tree* node = searchValue(value);
			if (node)
			{
				cout << "已搜索到，值为： " << node->value << endl;
			}
			else
			{
				cout << "AVL树中无此数据！！" << endl;
			}

			break;
		}
		case 3:
		{
			cout << "请输入需要删除的元素： " << endl;
			cin >> value;
			deleteNode(value);
			cout << "使用中序遍历输出如下： " << endl;
			middleOrder(m_rootNode);
			break;
		}	
		}
	}

	cout << endl;
	//释放内存
	deleteMemory();
	system("pause");

    return 0;
}

